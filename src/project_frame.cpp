
#include "wx/wxprec.h"
#include "wx/filedlg.h"

#include "project_frame.h"
#include "command_bar.h"

#include "yaml.h"

namespace ftk {

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

  wxBEGIN_EVENT_TABLE(ProjectFrame, wxFrame)
    EVT_MENU(MENU_NewProject, ProjectFrame::OnNewProject)
    EVT_MENU(wxID_OPEN, ProjectFrame::OnOpen)
    EVT_MENU(wxID_EXIT, ProjectFrame::OnExit)
    EVT_MENU(wxID_ABOUT, ProjectFrame::OnAbout)
    EVT_MENU(wxID_SAVE, ProjectFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, ProjectFrame::OnSaveAs)
    EVT_MENU(MENU_DebugStart, ProjectFrame::OnDebugStart)
    wxEND_EVENT_TABLE()

    ProjectFrame::ProjectFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
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

    // status bar
    CreateStatusBar();
    SetStatusText("Welcome to Flutter ToolKit!");

    // Container to hold all the contents of the window
    wxPanel* root_panel = new wxPanel(this);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    // Create the subviews
    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    _command_bar = new CommandBar(root_panel, wxDefaultPosition, wxSize(320, 36));
    vbox->Add(_command_bar, 0, wxEXPAND, 0);

    // project list area
    _project_list_ctl = new wxListCtrl(root_panel, PROJECT_List, wxPoint(0, 0), wxSize(260, 320));
    hbox->Add(_project_list_ctl, 0, wxEXPAND | wxALL, 0);

    // text editor area
    _main_edit_box = new wxStyledTextCtrl(root_panel, TEXT_Main);
#ifdef __WXOSX__
    wxFont font(wxFontInfo(12).FaceName("Menlo"));
#elif __WXGTK__
    wxFont font(wxFontInfo(9).FaceName("Roboto Mono"));
#else
    wxFont font(wxFontInfo(9).FaceName("Consolas"));
#endif

    _main_edit_box->StyleResetDefault();
    _main_edit_box->StyleSetFont(wxSTC_STYLE_DEFAULT, font);

    _main_edit_box->SetTabWidth(2);
    _main_edit_box->SetIndent(2);

    const int MARGIN_LINE_NUMBERS = 0;
    const int kMarginWidth = 30;

    _main_edit_box->SetMarginWidth(MARGIN_LINE_NUMBERS, kMarginWidth);
    _main_edit_box->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColor(75, 75, 75));
    _main_edit_box->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColor(220, 220, 220));
    _main_edit_box->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);
    _main_edit_box->SetWrapMode(wxSTC_WRAP_NONE);

    _main_edit_box->StyleClearAll();
    _main_edit_box->SetLexer(wxSTC_LEX_CPP);

    // CPP style colors
    _main_edit_box->StyleSetForeground(wxSTC_C_COMMENT, wxColor(150, 150, 150));
    _main_edit_box->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColor(150, 150, 150));
    _main_edit_box->StyleSetForeground(wxSTC_C_WORD2, wxColor(0, 0, 255));
    _main_edit_box->StyleSetForeground(wxSTC_C_STRING, wxColor(255, 0, 0));
    _main_edit_box->StyleSetForeground(wxSTC_C_CHARACTER, wxColor(255, 0, 0));
    // ...

    _main_edit_box->SetKeyWords(0, "await abstract as base break case catch checked continue default delegate do else event explicit extern false finally fixed for foreach goto if implicit in interface internal is lock namespace new null object operator out override params private protected public readonly ref return sealed sizeof stackalloc switch this throw true try typeof unchecked unsafe using virtual while");
    _main_edit_box->SetKeyWords(1, "bool byte char class const decimal double enum float int long sbyte short static string struct uint ulong ushort void");

    hbox->Add(_main_edit_box, 4, wxEXPAND | wxALL, 0);

    vbox->Add(hbox, 1, wxEXPAND | wxALL, 0);


    root_panel->SetSizerAndFit(vbox);
  }

  ProjectFrame::~ProjectFrame()
  {
  }

  void ProjectFrame::OnSave(wxCommandEvent& event)
  {
  }

  void ProjectFrame::OnSaveAs(wxCommandEvent& event)
  {
  }

  void ProjectFrame::OnExit(wxCommandEvent& event)
  {
    Close(true);
  }

  void ProjectFrame::OnAbout(wxCommandEvent& event)
  {
    wxMessageBox("Flutter ToolKit 1.0.1",
      "About Flutter ToolKit", wxOK | wxICON_INFORMATION);
  }

  void ProjectFrame::OnNewProject(wxCommandEvent& event)
  {
    // TODO : show a new project wizard here

    ProjectFrame *frame = new ProjectFrame("Flutter ToolKit - Untitled", wxPoint(50, 50), wxSize(1024, 768));
    frame->Show(true);
  }

  void ProjectFrame::OnOpen(wxCommandEvent& event)
  {
    wxFileDialog
      openFileDialog(this, _("Open file"), "", "",
        "Dart files (*.dart)|*.dart|Flutter Projects (*.yaml)|*.yaml|All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
      return;


    wxString        path = openFileDialog.GetPath();
    if (path.EndsWith(".yaml"))
    {
      // -- Load a new project
      yaml_parser_t parser;
      yaml_event_t evt;
      int done = 0;
      FILE *input;

      yaml_parser_initialize(&parser);

      if ((input = fopen(path, "rb")))
      {
        yaml_parser_set_input_file(&parser, input);

        while (!done) {
          // Get the next event.
          if (!yaml_parser_parse(&parser, &evt)) {
            wxLogError("Syntax error parsing YAML file '%s'.", path);
            break;
          }

          switch (evt.type)
          {
          case YAML_MAPPING_START_EVENT:
            wxLogMessage("<mapping>");
            break;
          case YAML_MAPPING_END_EVENT:
            wxLogMessage("</mapping>");
            break;
          case YAML_SEQUENCE_START_EVENT:
            wxLogMessage("<seq>");
            break;
          case YAML_SEQUENCE_END_EVENT:
            wxLogMessage("</seq>");
            break;
          case YAML_SCALAR_EVENT:
            wxLogMessage("%d %d %s %s %s", 
              evt.data.scalar.plain_implicit,
              evt.data.scalar.style,
              (const char*)evt.data.scalar.anchor,
              (const char*)evt.data.scalar.tag,
              (const char*)evt.data.scalar.value);
            break;
          default:
            break;
          }

          // Are we finished?
          done = (evt.type == YAML_STREAM_END_EVENT);

          // The application is responsible for destroying the event object.
          yaml_event_delete(&evt);

        }


        fclose(input);
      }

      yaml_parser_delete(&parser);

    }
    else 
    {
      // -- Load a file into the editor
      wxString        complete_file_contents;
      wxString        str;
      wxTextFile      tfile;
      if (!tfile.Open(path)) {
        wxLogError("Cannot open file '%s'.", path);
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
  }

  void ProjectFrame::OnDebugStart(wxCommandEvent& event)
  {

  }

} // namespace ftk