#ifndef __settings_h__
#define __settings_h__

#include <wx/fileconf.h>

namespace ftk 
{
  class Settings {
  public:
    Settings(const wxString& path);
    ~Settings();

  private:
    wxString _sdk_path;
    wxString _dart_path;
  };
}

#endif

