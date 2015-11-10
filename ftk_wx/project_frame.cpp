#include "wx/wxprec.h"
#include "project_frame.h"

enum
{
	MENU_DebugStart = wxID_HIGHEST + 1,
	MENU_DebugStop,
	TEXT_Main,
	PROJECT_List,

	ID_LAST
};

wxBEGIN_EVENT_TABLE(FTKProjectFrame, wxFrame)
EVT_MENU(wxID_OPEN, FTKProjectFrame::OnOpen)
EVT_MENU(wxID_EXIT, FTKProjectFrame::OnExit)
EVT_MENU(wxID_ABOUT, FTKProjectFrame::OnAbout)
EVT_MENU(MENU_DebugStart, FTKProjectFrame::OnDebugStart)
wxEND_EVENT_TABLE()

FTKProjectFrame::FTKProjectFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{

	// Build the menus
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

  // Container to hold all the contents of the window
  wxPanel* root_panel = new wxPanel(this);

	// Create the subviews
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

	// project list area
	_project_list_ctl = new wxListCtrl(root_panel, PROJECT_List, wxPoint(0,0), wxSize(260,320));
	hbox->Add(_project_list_ctl, 0, wxEXPAND | wxALL, 1);

	// text editor area
	_main_edit_box = new wxStyledTextCtrl(root_panel, TEXT_Main);
#ifdef __WXOSX__
	wxFont font(wxFontInfo(12).FaceName("Menlo"));
#else
  wxFont font(wxFontInfo(10).FaceName("Consolas"));
#endif
	_main_edit_box->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
  _main_edit_box->SetTabWidth(2);
	hbox->Add(_main_edit_box, 4, wxEXPAND | wxALL, 1);

	
	root_panel->SetSizerAndFit(hbox);
  

}

FTKProjectFrame::~FTKProjectFrame()
{
}

void FTKProjectFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void FTKProjectFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("Flutter ToolKit 1.0.1",
		"About Flutter ToolKit", wxOK | wxICON_INFORMATION);
}

void FTKProjectFrame::OnOpen(wxCommandEvent& event)
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

	_main_edit_box->SetValue(complete_file_contents);
}

void FTKProjectFrame::OnDebugStart(wxCommandEvent& event)
{

}
