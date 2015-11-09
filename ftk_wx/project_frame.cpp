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


	// Create the subviews
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

	

	//wxPanel* panel = new wxPanel(this, -1);

	// project list
	_project_list_ctl = new wxListCtrl(this, PROJECT_List, wxPoint(0,0), wxSize(320, 640));
	hbox->Add(_project_list_ctl, 1, wxEXPAND | wxALL, 2);

	// text editor
	_main_edit_box = new wxStyledTextCtrl(this, TEXT_Main);
	wxFont font(wxFontInfo(9).FaceName("Inconsolata"));
	_main_edit_box->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
	hbox->Add(_main_edit_box, 1, wxEXPAND | wxALL, 2);

	
	SetSizerAndFit(hbox);

	//panel->SetSizer(vbox);

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
