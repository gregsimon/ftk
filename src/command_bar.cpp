
#include "wx/wxprec.h"

#include "ftk.h"

#include "wx/combobox.h"


#include "command_bar.h"

namespace ftk {

  enum
  {
    ID_DevicePicker = wxID_HIGHEST + 100,
  };

  wxBEGIN_EVENT_TABLE(CommandBar, wxPanel)
    EVT_CHOICE(ID_DevicePicker, CommandBar::OnChangeDevice)
  wxEND_EVENT_TABLE()

  CommandBar::CommandBar(wxWindow* parent, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, wxID_ANY, pos, size)
  {
    // get the list of USB devices.
    currentAdbEndpoint()->list_devices(_usb_devices);


    _device_picker = new wxChoice(this, ID_DevicePicker, wxDefaultPosition, 
      wxSize(128, 32));

    refresh_device_list();
  }

  CommandBar::~CommandBar()
  {

  }

  void CommandBar::OnChangeDevice(wxCommandEvent& event)
  {
    wxStringClientData* cd = (wxStringClientData*)(event.GetClientObject());
    wxString unique_id = cd->GetData();

    if (!unique_id.Cmp("none")) {
      currentAdbEndpoint()->close_device();
    } else {
      currentAdbEndpoint()->connect_to_device_by_id(unique_id);
    }
  }

  void CommandBar::refresh_device_list()
  {
    _device_picker->Clear();
    _device_picker->Append("Disconnected", new wxStringClientData("none"));
    _device_picker->SetCanFocus(false);

    for (AdbDeviceList::iterator it = _usb_devices.begin();
    it != _usb_devices.end(); ++it) {
      _device_picker->Append(it->device_name, &(it->unique_id));
    }

    _device_picker->SetSelection(0);
  }

} // namespace ftk