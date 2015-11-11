#ifndef __command_bar_h__
#define __command_bar_h__

#include "wx/panel.h"
#include "wx/combobox.h"

class FTKCommandBar : public wxPanel
{
public:
  FTKCommandBar(wxWindow* parent, const wxPoint& pos, const wxSize& size);
  ~FTKCommandBar();



private:
 // wxDECLARE_EVENT_TABLE();

};


#endif

