// ftk - main


#include "wx/wxprec.h"

#include "ftk_platform.h"
#include "project_frame.h"
#include "settings.h"

// -----------------------------------------------------------------------

class ToolkitApp : public wxApp
{
public:
  ToolkitApp();
  virtual ~ToolkitApp();

  virtual bool OnInit();

private:
  ftk::Settings*  _settings;
};

wxIMPLEMENT_APP(ToolkitApp);


ToolkitApp::ToolkitApp()
{
  _settings = new ftk::Settings("");
}

ToolkitApp::~ToolkitApp()
{
  delete _settings;
}

bool ToolkitApp::OnInit()
{
  wxLog::SetActiveTarget(new wxLogStderr());

  FTK_Platform_Init();



  ftk::ProjectFrame *frame = new ftk::ProjectFrame("Flutter ToolKit", wxPoint(50, 50), wxSize(1024, 768));
  frame->Show(true);
  return true;
}
