

#include "wx/wxprec.h"

#include <stdarg.h>
#include <libusb-1.0/libusb.h>

#include "ftk-adb-usb.h"



const int kMaxReceiveSize = 256 * 1024;

namespace ftk {


  struct UsbEndpointInternals {
  
    uint32_t incoming_received_sz;
    uint8_t incoming_msg[kMaxReceiveSize];

  };

  UsbEndpoint::UsbEndpoint() : i(new UsbEndpointInternals)
  {
  }

  UsbEndpoint::~UsbEndpoint()
  {
    delete i;
  }

  void UsbEndpoint::log(const char* szFormat, ...)
  {
    va_list arg;
    va_start(arg, szFormat);
    vprintf(szFormat, arg);
    va_end(arg);
  }



  int UsbEndpoint::list_devices(AdbDeviceList& devices)
  {
    
    return 0;
  }

  int UsbEndpoint::open_device(const AdbDevice& d)
  {

    return 0;
  }

  int UsbEndpoint::close_device(const AdbDevice& d)
  {
    return 0;
  }

  int UsbEndpoint::send_data(const uint8_t* buffer, uint32_t length)
  {
    return 0;
  }

  bool UsbEndpoint::isOpen() const
  {
    return false;
  }

} // namespace ftk