#ifndef __console_h__
#define __console_h__

#include "wx/panel.h"

class wxTextCtrl;

namespace ftk {

  class Console : public wxPanel
  {
  public:
    Console(wxWindow* parent, wxWindowID id = wxID_ANY, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
    ~Console();


    void add_text(const wxString&);
    void clear();
    void scroll_to_line(long);
    void scroll_to_end();

  private:
    ::wxTextCtrl* _textctrl;
  };


} // namespace ftk

#endif

