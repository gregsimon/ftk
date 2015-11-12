#ifndef __project_frame_h__
#define __project_frame_h__

#include "wx/frame.h"
#include "wx/textfile.h"
#include "wx/textctrl.h"
#include "wx/stc/stc.h"
#include "wx/listctrl.h"

namespace ftk {

  class CommandBar;

  class ProjectFrame : public wxFrame
  {
  public:
    ProjectFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~ProjectFrame();


  private:
    void OnNewProject(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnDebugStart(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();

    wxListCtrl* _project_list_ctl;
    wxStyledTextCtrl *_main_edit_box;
    CommandBar* _command_bar;
  };

} // namespace ftk

#endif

