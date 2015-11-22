
#include "wx/wxprec.h"

#include "ftk.h"

#include "wx/combobox.h"


#include "command_bar.h"

namespace ftk {

  CommandBar::CommandBar(wxWindow* parent, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, wxID_ANY, pos, size)
  {
    // get the list of USB devices.
    currentAdbEndpoint()->list_devices(_usb_devices);


    _device_picker = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(128, 32),
      wxArrayString(), wxCB_DROPDOWN | wxCB_READONLY);

    refresh_device_list();
  }

  CommandBar::~CommandBar()
  {

  }

  void CommandBar::refresh_device_list()
  {
    _device_picker->Clear();
    _device_picker->AppendString("Disconnected");
    _device_picker->SetCanFocus(false);

    for (AdbDeviceList::iterator it = _usb_devices.begin();
    it != _usb_devices.end(); ++it) {
      _device_picker->AppendString(it->device_name);
    }

    _device_picker->SetSelection(0);

  }

} // namespace ftk