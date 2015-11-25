

#include "wx/wxprec.h"

#include <stdarg.h>

#include "ftk-adb-usb.h"

namespace ftk {

  struct UsbEndpointInternals {
    UsbEndpointInternals(UsbEndpoint* owner)
      : usb(owner)
    {
    }
    ~UsbEndpointInternals()
    {
    }

    UsbEndpoint* usb;
    AdbDeviceList devices;

  };


  UsbEndpoint::UsbEndpoint() : _i(new UsbEndpointInternals(this))
  {
  }

  UsbEndpoint::~UsbEndpoint()
  {
    delete _i;
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
    return -1;
  }

  int UsbEndpoint::open_device_by_id(const char* unique_id)
  {
    return -1;
  }

  int UsbEndpoint::close_device()
  {
    return -1;
  }

  int UsbEndpoint::send(const uint8_t* buffer, uint32_t length)
  {
    return 0;
  }

  bool UsbEndpoint::isOpen() const
  {
    return false;
  }


} // namespace ftk