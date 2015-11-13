#ifndef __command_bar_h__
#define __command_bar_h__

#include "wx/panel.h"
#include "wx/combobox.h"

namespace ftk {

  class CommandBar : public wxPanel
  {
  public:
    CommandBar(wxWindow* parent, const wxPoint& pos, const wxSize& size);
    ~CommandBar();



  private:
    // wxDECLARE_EVENT_TABLE();

  };

} // namespace ftk

#endif

