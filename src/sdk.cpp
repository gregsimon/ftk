
#include "wx/wxprec.h"

#include "sdk.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <stack>

#include "../third_party/yaml/include/yaml.h"

namespace ftk
{
  Sdk* currentSDK() {
    static Sdk sdk;
    return &sdk;
  }


  Sdk::Sdk()
  {
  }

  Sdk::~Sdk()
  {
  }


  int Sdk::load(const wxString& path)
  {
    _packages.clear();
    _path = path;

    // The packages are in 'sdk_root/packages
    wxFileName packages_path;
    packages_path.AssignDir(path);
    packages_path.AppendDir("packages");

    wxDir packages_dir(packages_path.GetFullPath());

    if (!packages_dir.IsOpened()) {
      wxLogError("unable to open packages dir in sdk [%s]", packages_path.GetFullPath());
      return -1;
    }

    // iterate subfolders of the packages.
    wxString filename;
    bool cont = packages_dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont) {
      wxFileName package_fn(packages_path);
      package_fn.AppendDir(filename);

      Package p(package_fn.GetFullPath());

      //wxLogDebug(" package: %s", (const char*)filename);

      if (p.is_valid())
        _packages.push_back(p);

      cont = packages_dir.GetNext(&filename);
    }

    return 0;
  }

  Package::Package(const wxFileName& fn)
  {
    // This folder may *not* be a package, but we'll attempt
    // to collect data from it and make a final decision
    // parse 

    // Parse the pubspec.yaml file
    wxFileName yamlpath(fn);
    yamlpath.SetName("pubspec.yaml");
    if (FILE *f = fopen((const char*)yamlpath.GetFullPath(), "r"))
    {
      yaml_parser_t parser;
      yaml_parser_initialize(&parser);
      yaml_parser_set_input_file(&parser, f);

      yaml_document_t doc;
      yaml_parser_load(&parser, &doc);
      yaml_node_t* root_node = yaml_document_get_root_node(&doc);

      for (yaml_node_pair_t * i = root_node->data.mapping.pairs.start; i < root_node->data.mapping.pairs.top; ++i)
      {
        yaml_node_t * key = yaml_document_get_node(&doc, i->key);
        yaml_node_t * value = yaml_document_get_node(&doc, i->value);

        if (value->type == YAML_MAPPING_NODE) {
          for (yaml_node_pair_t * j = value->data.mapping.pairs.start; j < value->data.mapping.pairs.top; ++j)
          {
            yaml_node_t * key2 = yaml_document_get_node(&doc, j->key);
            yaml_node_t * value2 = yaml_document_get_node(&doc, j->value);

            if (!strcmp((const char*)key->data.scalar.value, "dependencies"))
              dependencies[(const char*)key2->data.scalar.value] = (const char*)value2->data.scalar.value;
            else if (!strcmp((const char*)key->data.scalar.value, "env"))
              env[(const char*)key2->data.scalar.value] = (const char*)value2->data.scalar.value;
          }
        }
        else {
          //wxLogDebug("%s = %s", (const char*)key->data.scalar.value,
          //  (const char*)value->data.scalar.value);
          if (!strcmp((const char*)key->data.scalar.value, "name"))
            name = (const char*)value->data.scalar.value;
          else if (!strcmp((const char*)key->data.scalar.value, "author"))
            author = (const char*)value->data.scalar.value;
          else if (!strcmp((const char*)key->data.scalar.value, "description"))
            description = (const char*)value->data.scalar.value;
          else if (!strcmp((const char*)key->data.scalar.value, "version"))
            version = (const char*)value->data.scalar.value;
          else if (!strcmp((const char*)key->data.scalar.value, "homepage"))
            homepage = (const char*)value->data.scalar.value;
        }
       
      }

      yaml_document_delete(&doc);
      yaml_parser_delete(&parser);
      fclose(f);
    }

  }

  bool Package::is_valid()
  {
    return name.size() > 0 && version.size() > 0; // TODO
  }

}