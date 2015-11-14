#ifndef __project_h__
#define __project_h___

#include <list>


namespace ftk 
{
  class Project {
  public:
    Project(const wxString& path);
    ~Project();

    const wxString& name() const {
      return _name;
    }

  private:
    wxString _name;
    std::list<wxString> _files;    
  };
}

#endif

