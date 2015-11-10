#ifndef __project_frame_h__
#define __project_frame_h__

#include "wx/wxprec.h"

#include "wx/textfile.h"
#include "wx/textctrl.h"
#include "wx/stc/stc.h"
#include "wx/listctrl.h"

class FTKProjectFrame : public wxFrame
{
public:
	FTKProjectFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~FTKProjectFrame();

	

private:
	void OnOpen(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnDebugStart(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

	wxListCtrl* _project_list_ctl;
	wxStyledTextCtrl *_main_edit_box;
};


#endif

