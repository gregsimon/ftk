// ftk - main


#include "wx/wxprec.h"

#include <wx/textfile.h>
#include <wx/textctrl.h>
#include <wx/stc/stc.h>
#include <wx/font.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/log.h>



#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// -----------------------------------------------------------------------


class FTKApp : public wxApp
{
public:
	virtual bool OnInit();
};

// -----------------------------------------------------------------------

class FTKFrame : public wxFrame
{
public:
	FTKFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	wxStyledTextCtrl *main_edit_box_;

private:
	void OnOpen(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnDebugStart(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

enum
{
	MENU_DebugStart= wxID_HIGHEST + 1,
	MENU_DebugStop,
	TEXT_Main,
	ID_LAST
};

wxBEGIN_EVENT_TABLE(FTKFrame, wxFrame)
EVT_MENU(wxID_OPEN, FTKFrame::OnOpen)
EVT_MENU(wxID_EXIT, FTKFrame::OnExit)
EVT_MENU(wxID_ABOUT, FTKFrame::OnAbout)
EVT_MENU(MENU_DebugStart, FTKFrame::OnDebugStart)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(FTKApp);

bool FTKApp::OnInit()
{
	wxLog::SetActiveTarget(new wxLogStderr());

	FTKFrame *frame = new FTKFrame("Flutter ToolKit", wxPoint(50, 50), wxSize(1024, 768));
	frame->Show(true);
	return true;
}

FTKFrame::FTKFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN, "&Open...\tCtrl-O",
		"Open file");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuDebug = new wxMenu;
	menuDebug->Append(MENU_DebugStart, "Start Debugging\tF5",
		"Start debugging this project");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuDebug, "&Debug");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to Flutter ToolKit!");

	main_edit_box_ = new wxStyledTextCtrl(this, TEXT_Main); 
	/*
	wxFont* font = new wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
	main_edit_box_->SetFont(*font);
	main_edit_box_->SetBackgroundColour(wxColour(30, 30, 30));
	main_edit_box_->SetForegroundColour(wxColour(200, 200, 200));
	*/
}

void FTKFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void FTKFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a wxWidgets' Hello world sample",
		"About Hello World", wxOK | wxICON_INFORMATION);
}

void FTKFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog
		openFileDialog(this, _("Open file"), "", "",
			"Dart files (*.dart)|*.dart|CoffeeScript files (*.coffee)|*.coffee|JavaScript files (*.js)|*.js", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...


	wxString        complete_file_contents;
	wxString        str;
	wxTextFile      tfile;
	if (!tfile.Open(openFileDialog.GetPath())) {
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	// read the first line
	str = tfile.GetFirstLine();
	complete_file_contents += str;
	while (!tfile.Eof())
	{
		str = tfile.GetNextLine();
		complete_file_contents += "\n";
		complete_file_contents += str;
	}

	wxLogMessage("File loaded is\n%s\n", complete_file_contents);
	main_edit_box_->SetValue(complete_file_contents);
}

void FTKFrame::OnDebugStart(wxCommandEvent& event)
{

}