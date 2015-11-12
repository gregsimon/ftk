
#include "wx/wxprec.h"

#include "settings.h"
#include "ftk_platform.h"

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>


namespace ftk 
{

  //
  // Currently we store settings in $HOME/.ftk/settings
  //

Settings::Settings(const wxString& path)
{
  wxStandardPaths& paths = wxStandardPaths::Get();
  
  wxFileName settings_path;
  settings_path.AssignDir(paths.GetUserConfigDir());
  settings_path.AppendDir(".ftk");

  if (!wxDir::Exists(settings_path.GetFullPath())) {
    if (!wxDir::Make(settings_path.GetFullPath(),wxS_DIR_DEFAULT)) {
      wxLogError("Unable to create settings folder %s", settings_path.GetFullPath());
    }
  }

  settings_path.SetName("settings");

  wxLogMessage("settings path -> %s", settings_path.GetFullPath());

}

Settings::~Settings()
{
}

} // namespace ftk



