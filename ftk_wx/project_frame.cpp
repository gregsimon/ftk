#include "wx/wxprec.h"
#include "project_frame.h"

enum
{
	MENU_DebugStart = wxID_HIGHEST + 1,
	MENU_DebugStop,
  MENU_NewProject,
  MENU_SDKManager,
  MENU_Devices,

	TEXT_Main,
	PROJECT_List,

	ID_LAST
};

wxBEGIN_EVENT_TABLE(FTKProjectFrame, wxFrame)
EVT_MENU(wxID_OPEN, FTKProjectFrame::OnOpen)
EVT_MENU(wxID_EXIT, FTKProjectFrame::OnExit)
EVT_MENU(wxID_ABOUT, FTKProjectFrame::OnAbout)
EVT_MENU(wxID_SAVE, FTKProjectFrame::OnSave)
EVT_MENU(wxID_SAVEAS, FTKProjectFrame::OnSaveAs)
EVT_MENU(MENU_DebugStart, FTKProjectFrame::OnDebugStart)
wxEND_EVENT_TABLE()

FTKProjectFrame::FTKProjectFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{

	// Build the menus
	wxMenu *menuFile = new wxMenu;
  menuFile->Append(wxID_NEW, "&New...\tCtrl+N",
    "New File");
  menuFile->Append(MENU_NewProject, "&New Project...",
    "New Project");
  menuFile->AppendSeparator();
	menuFile->Append(wxID_OPEN, "&Open...\tCtrl-O",
		"Open file");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
  

	wxMenu* menuDebug = new wxMenu;
	menuDebug->Append(MENU_DebugStart, "Start Debugging\tF5",
		"Start debugging this project");
  
  wxMenu* menuWindow = new wxMenu;
  menuWindow->Append(MENU_SDKManager, "SDK Manager",
                     "Open the SDK Manager");
  menuWindow->Append(MENU_Devices, "Devices",
                     "Open the device manager");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuDebug, "&Debug");
  menuBar->Append(menuWindow, "&Window");
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
  wxFont font(wxFontInfo(9).FaceName("Consolas"));
#endif

  _main_edit_box->StyleResetDefault();
	_main_edit_box->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
  
  _main_edit_box->SetTabWidth(2);
  _main_edit_box->SetIndent(2);

  const int MARGIN_LINE_NUMBERS = 0;

  _main_edit_box->SetMarginWidth(MARGIN_LINE_NUMBERS, 18);
  _main_edit_box->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColor(75, 75, 75));
  _main_edit_box->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(220, 220, 220));
  _main_edit_box->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
  _main_edit_box->SetWrapMode(wxSTC_WRAP_NONE);

  _main_edit_box->StyleClearAll();
  _main_edit_box->SetLexer(wxSTC_LEX_CPP);

  /*
  #define wxSTC_C_DEFAULT 0
#define wxSTC_C_COMMENT 1
#define wxSTC_C_COMMENTLINE 2
#define wxSTC_C_COMMENTDOC 3
#define wxSTC_C_NUMBER 4
#define wxSTC_C_WORD 5
#define wxSTC_C_STRING 6
#define wxSTC_C_CHARACTER 7
#define wxSTC_C_UUID 8
#define wxSTC_C_PREPROCESSOR 9
#define wxSTC_C_OPERATOR 10
#define wxSTC_C_IDENTIFIER 11
#define wxSTC_C_STRINGEOL 12
#define wxSTC_C_VERBATIM 13
#define wxSTC_C_REGEX 14
#define wxSTC_C_COMMENTLINEDOC 15
#define wxSTC_C_WORD2 16
#define wxSTC_C_COMMENTDOCKEYWORD 17
#define wxSTC_C_COMMENTDOCKEYWORDERROR 18
#define wxSTC_C_GLOBALCLASS 19
#define wxSTC_C_STRINGRAW 20
#define wxSTC_C_TRIPLEVERBATIM 21
#define wxSTC_C_HASHQUOTEDSTRING 22
#define wxSTC_C_PREPROCESSORCOMMENT 23
*/
  // CPP style colors
  _main_edit_box->StyleSetForeground(wxSTC_C_COMMENT, wxColor(150, 150, 150));
  _main_edit_box->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColor(150, 150, 150));
  _main_edit_box->StyleSetForeground(wxSTC_C_WORD2, wxColor(0, 0, 255));
  _main_edit_box->StyleSetForeground(wxSTC_C_STRING, wxColor(255, 0, 0));
  _main_edit_box->StyleSetForeground(wxSTC_C_CHARACTER, wxColor(255, 0, 0));

  _main_edit_box->SetKeyWords(0, "await abstract as base break case catch checked continue default delegate do else event explicit extern false finally fixed for foreach goto if implicit in interface internal is lock namespace new null object operator out override params private protected public readonly ref return sealed sizeof stackalloc switch this throw true try typeof unchecked unsafe using virtual while");
  _main_edit_box->SetKeyWords(1, "bool byte char class const decimal double enum float int long sbyte short static string struct uint ulong ushort void");

	hbox->Add(_main_edit_box, 4, wxEXPAND | wxALL, 1);

	
	root_panel->SetSizerAndFit(hbox);
  

}

FTKProjectFrame::~FTKProjectFrame()
{
}

void FTKProjectFrame::OnSave(wxCommandEvent& event)
{
}

void FTKProjectFrame::OnSaveAs(wxCommandEvent& event)
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
