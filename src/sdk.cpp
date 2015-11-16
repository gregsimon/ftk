
#include "wx/wxprec.h"

#include "sdk.h"

#include <wx/filename.h>
#include <wx/dir.h>

#nclude "../third_party/yaml/include/yaml.h"

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
      package_fn.AppendDir(package_fn);

      Sdk::Package p(package_fn.GetFullPath());
      p.name = filename;

      wxLogDebug(" package: %s", (const char*)filename);

      

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


  }

  bool Sdk::Package::is_valid()
  {
    return version.size() > 0; // TODO
  }

}