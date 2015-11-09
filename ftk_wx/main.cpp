// ftk - main


#include "wx/wxprec.h"

#include "ftk_platform.h"
#include "project_frame.h"

#include "../adb/ftk-adb-usb.h"




// -----------------------------------------------------------------------


class FTKApp : public wxApp
{
public:
	FTKApp();
	virtual ~FTKApp();

	virtual bool OnInit();
};

wxIMPLEMENT_APP(FTKApp);


FTKApp::FTKApp()
{
}

FTKApp::~FTKApp()
{
	
}

bool FTKApp::OnInit()
{
	wxLog::SetActiveTarget(new wxLogStderr());

	FTK_Platform_Init();

	

	FTKProjectFrame *frame = new FTKProjectFrame("Flutter ToolKit", wxPoint(50, 50), wxSize(1024, 768));
	frame->Show(true);
	return true;
}

