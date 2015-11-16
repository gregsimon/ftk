#ifndef __project_h__
#define __project_h___

#include <map>
#include <list>
#include "wx/string.h"



namespace ftk 
{
  class Project {
  public:
    Project(const wxString& path);
    ~Project();

    const wxString& name() const {
      return _name;
    }

    int get(const wxString& key, wxString& value);
    int set(const wxString& key, const wxString& value);

    KeyValueList& dependencies() { return _dependencies; }
    KeyValueList& dependencies() { return _dependencies; }


  private:
    // persist
    KeyValueList _root_keys;
    KeyValueList _dependencies;
    KeyValueList _environment;
    KeyValueList _dev_dependencies; // ?

    // temp
    std::list<wxString> _files;

    int collect_dependency_files();
  };
}

#endif

