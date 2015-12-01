// ftk - main


#include "wx/wxprec.h"

#include "ftk_platform.h"
#include "project_frame.h"
#include "settings.h"

#include "wx/persist.h"
#include "wx/config.h"


namespace ftk {

  // Overwide this so we can control what the settings file is named.
  class PersistenceManager : public wxPersistenceManager {
  public:
    PersistenceManager() : _config(new wxConfig("ftk-settings")) { }
    ~PersistenceManager() { delete _config; }

    virtual wxConfigBase* GetConfig() const { return _config; }

    private:
      wxConfig* _config;
  };

  Settings* globalSettings() {
    static Settings sdk("");
    return &sdk;
  }

}

// -----------------------------------------------------------------------

class ToolkitApp : public wxApp
{
public:
  ToolkitApp();
  virtual ~ToolkitApp();

  virtual bool OnInit();

private:
};

wxIMPLEMENT_APP(ToolkitApp);


ToolkitApp::ToolkitApp()
{
  ftk::globalSettings();
}

ToolkitApp::~ToolkitApp()
{
}

bool ToolkitApp::OnInit()
{
  static ftk::PersistenceManager sPersistenceManager;
  wxLog::SetActiveTarget(new wxLogStderr());

  wxPersistenceManager::Set(sPersistenceManager);

  FTK_Platform_Init();

  ftk::ProjectFrame *frame = new ftk::ProjectFrame("Flutter ToolKit", 
        wxPoint(50, 50), wxSize(1024, 768));
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}
