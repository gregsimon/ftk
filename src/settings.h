#ifndef __settings_h__
#define __settings_h__

#include "wx/fileconf.h"

// The user settings

namespace ftk 
{
  class Settings {
  public:
    Settings(const wxString& path);
    ~Settings();


    void set_sdk_path(const wxString& s) { _sdk_path = s; save();  }
    void set_dart_path(const wxString& s) { _dart_path = s; save();  }

    const wxString& sdk_path() const { return _sdk_path;  }
    const wxString& dart_path() const { return _dart_path; }

  private:
    wxString _settings_path;    // path to the users' setting file.

    wxString _sdk_path;         // path to the active flutter sdk
    wxString _dart_path;        // dart home (/bin would be off this)


    int load();
    int save();
  };
}

#endif

