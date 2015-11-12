// ftk - main


#include "wx/wxprec.h"

#include "ftk_platform.h"
#include "project_frame.h"

#include "../adb/ftk-adb-usb.h"


namespace ftk {



  // -----------------------------------------------------------------------


  class ToolkitApp : public wxApp
  {
  public:
    ToolkitApp();
    virtual ~ToolkitApp();

    virtual bool OnInit();
  };

  wxIMPLEMENT_APP(ftk::ToolkitApp);


  ToolkitApp::ToolkitApp()
  {
  }

  ToolkitApp::~ToolkitApp()
  {

  }

  bool ToolkitApp::OnInit()
  {
    wxLog::SetActiveTarget(new wxLogStderr());

    FTK_Platform_Init();



    ProjectFrame *frame = new ProjectFrame("Flutter ToolKit", wxPoint(50, 50), wxSize(1024, 768));
    frame->Show(true);
    return true;
  }

} // namespace ftk