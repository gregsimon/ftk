
#ifndef __sdk_h__
#define __sdk_h__

#include <list>
#include "key_value_list.h"
#include "wx/filename.h"

namespace ftk
{

  class Sdk
  {
  public:
    Sdk();
    ~Sdk();

    int load(const wxString& path);

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
      wxString env;
      KeyValueList dependencies;
      wxString readme;
    };

  private:

    wxString _path;
    std::list<Package> _packages;
  };
}

#endif
