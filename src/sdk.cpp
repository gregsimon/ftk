
#include "wx/wxprec.h"

#include "sdk.h"

#include <wx/filename.h>
#include <wx/dir.h>

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

      Sdk::Package p(package_fn.GetFullPath());
      p.name = filename;

      //wxLogDebug(" package: %s", (const char*)filename);

      

      if (p.is_valid())
        _packages.push_back(p);

      cont = packages_dir.GetNext(&filename);
    }

    return 0;
  }

  Sdk::Package::Package(const wxFileName& fn)
  {
    wxLogDebug("TODO : Load package from [%s]", (const char*)fn.GetFullPath());

    // This folder may *not* be a package, but we'll attempt
    // to collect data from it and make a final decision
    // parse 

    // Parse the pubspec.yaml file
    wxFileName yamlpath(fn);
    yamlpath.SetName("pubspec.yaml");
    if (FILE *f = fopen((const char*)yamlpath.GetFullPath(), "r"))
    {
      yaml_parser_t parser;
      yaml_event_t  event;
      yaml_parser_initialize(&parser);
      yaml_parser_set_input_file(&parser, f);

      do {
        if (!yaml_parser_parse(&parser, &event)) {
          wxLogDebug("Parser error %d\n", parser.error);
        }

        switch (event.type)
        {
        case YAML_NO_EVENT: wxLogDebug("No event!"); break;
          /* Stream start/end */
        case YAML_STREAM_START_EVENT: wxLogDebug("STREAM START"); break;
        case YAML_STREAM_END_EVENT:   wxLogDebug("STREAM END");   break;
          /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT: wxLogDebug("<b>Start Document</b>"); break;
        case YAML_DOCUMENT_END_EVENT:   wxLogDebug("<b>End Document</b>");   break;
        case YAML_SEQUENCE_START_EVENT: wxLogDebug("<b>Start Sequence</b>"); break;
        case YAML_SEQUENCE_END_EVENT:   wxLogDebug("<b>End Sequence</b>");   break;
        case YAML_MAPPING_START_EVENT:  wxLogDebug("<b>Start Mapping</b>");  break;
        case YAML_MAPPING_END_EVENT:    wxLogDebug("<b>End Mapping</b>");    break;
          /* Data */
        case YAML_ALIAS_EVENT:  wxLogDebug("Got alias (anchor %s)", (const char*)event.data.alias.anchor); break;
        case YAML_SCALAR_EVENT: 
          wxLogDebug("Got scalar (value %s)", (const char*)event.data.scalar.value); break;
        }
        if (event.type != YAML_STREAM_END_EVENT)
          yaml_event_delete(&event);
      } while (event.type != YAML_STREAM_END_EVENT);
      yaml_event_delete(&event);


      yaml_parser_delete(&parser);
      fclose(f);
    }

  }

  bool Sdk::Package::is_valid()
  {
    return version.size() > 0; // TODO
  }

}