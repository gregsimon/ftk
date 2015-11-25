
#include "wx/wxprec.h"

#include "console.h"

#include "wx/textctrl.h"
#include "wx/txtstrm.h"
#include "wx/process.h"


namespace ftk {

  class DisplayProcess
  {
  public:
    DisplayProcess(wxTextCtrl*);
    void Execute(const wxString&);
  private:
    wxTextCtrl* tctrl;
  };

  DisplayProcess::DisplayProcess(wxTextCtrl* ctrl) : tctrl(ctrl)
  {
  }

  void DisplayProcess::Execute(const wxString& command)
  {
    if (!tctrl)
    {
      return;
    }

    wxProcess *process = new wxProcess(wxPROCESS_REDIRECT);
    //long pid =
      wxExecute(command, wxEXEC_ASYNC, process);
    process->Redirect();

    if (process)
    {
      wxString log;
      wxInputStream *msg = process->GetInputStream();

      wxTextInputStream tStream(*msg);
      while (!msg->Eof())
      {
        log = tStream.ReadLine();
        tctrl->AppendText(log + wxT("\n"));
        tctrl->ShowPosition(tctrl->GetLastPosition());
      }
      tctrl->AppendText(wxT("Finished!\n"));
    }
    else {
      tctrl->AppendText(wxT("FAIL: Command" + command + " could not be run!\n"));
    }
  }



  Console::Console(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
    : wxPanel(parent, id, pos, size)
  {
    wxBoxSizer *hbox = new wxBoxSizer(wxVERTICAL);
    _textctrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, pos, size, 
      wxTE_MULTILINE | wxTE_DONTWRAP | wxTE_READONLY);

#ifdef __WXOSX__
    wxFont font(wxFontInfo(12).FaceName("Menlo"));
#elif __WXGTK__
    wxFont font(wxFontInfo(9).FaceName("Roboto Mono"));
#else
    wxFont font(wxFontInfo(9).FaceName("Consolas"));
#endif
    _textctrl->SetFont(font);

    hbox->Add(_textctrl, 1, wxEXPAND | wxALL, 0);
    SetSizerAndFit(hbox);

    DisplayProcess* dp = new DisplayProcess(_textctrl);

    dp->Execute("git");
    delete dp;
  }

  Console::~Console()
  {
  }

  void Console::add_text(const wxString& str) {
    _textctrl->AppendText(str + wxT("\n"));
  }

  void Console::clear() {
    _textctrl->Clear();
  }

  void Console::scroll_to_line(long no) {
    _textctrl->ShowPosition(no);
  }

  void Console::scroll_to_end() {
    _textctrl->ShowPosition(_textctrl->GetLastPosition());
  }



} // ftk