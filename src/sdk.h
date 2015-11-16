
#ifndef __sdk_h__
#define __sdk_h__

#include <list>
#include "key_value_list.h"
#include "wx/filename.h"





namespace ftk
{
  class Package
  {
  public:
    Package(const wxFileName&);
    bool is_valid();

    wxString name;
    wxString description;
    wxString version;
    wxString author;
    wxString homepage;
    KeyValueList env;
    KeyValueList dependencies;
    wxString readme;
  };
  typedef std::list<Package> PackageList;

  class Sdk
  {
  public:
    Sdk();
    ~Sdk();

    int load(const wxString& path);
    const PackageList& packages() const { return _packages; }

  private:

    wxString _path;
    PackageList _packages;
  };
}

#endif
