#ifndef __command_bar_h__
#define __command_bar_h__

#include "wx/panel.h"
#include "wx/choice.h"
#include "adb/ftk-adb-usb.h"

class wxComboBox;

namespace ftk {

  class CommandBar : public wxPanel
  {
  public:
    CommandBar(wxWindow* parent, const wxPoint& pos, const wxSize& size);
    ~CommandBar();



  private:
    AdbDeviceList _usb_devices;
    wxChoice* _device_picker;

    void refresh_device_list();
    // wxDECLARE_EVENT_TABLE();

  };

} // namespace ftk

#endif

