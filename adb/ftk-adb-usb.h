#ifndef __adb_usb_h__
#define __adb_usb_h__

#include <list>
#include <wchar.h>
#include <stdint.h>

#include "wx/thread.h"

namespace ftk {

  const int kMaxFilePath = 2048;

  // ADB constants
  const uint32_t A_SYNC = 0x434e5953;
  const uint32_t A_CNXN = 0x4e584e43;
  const uint32_t A_OPEN = 0x4e45504f;
  const uint32_t A_OKAY = 0x59414b4f;
  const uint32_t A_CLSE = 0x45534c43;
  const uint32_t A_WRTE = 0x45545257;
  const uint32_t A_AUTH = 0x48545541;

  const int kAdbHeaderSize = 24;

  class AdbDevice
  {
  public:
    wchar_t device_name[kMaxFilePath];
    wchar_t device_path[kMaxFilePath];

    uint8_t addr_out;
    uint8_t addr_in;

    AdbDevice() {
      device_name[0] = 0;
      device_path[0] = 0;
    }
    AdbDevice(const AdbDevice& left) {
      *this = left;
    }
    AdbDevice& operator=(const AdbDevice& left) {
      wcscpy(device_name, left.device_name);
      wcscpy(device_path, left.device_path);
      return *this;
    }
  };
  typedef std::list<AdbDevice> AdbDeviceList;



  class UsbEndpoint
  {
  public:
    UsbEndpoint();
    virtual ~UsbEndpoint();

    int list_devices(AdbDeviceList&);
    int open_device(const AdbDevice&);
    int close_device(const AdbDevice&);

    bool isOpen() const;

    virtual int on_data_received(const uint8_t* buffer, uint32_t length) = 0;

    int send_data(const uint8_t* buffer, uint32_t length);

    void log(const char* szFormat, ...);

  private:
    UsbEndpoint& operator=(const UsbEndpoint&);
    UsbEndpoint(const UsbEndpoint&);

    struct UsbEndpointInternals* _i;

  };


  struct AdbMessage {
    AdbMessage() : payload(0), payload_len(0) {}
    ~AdbMessage() {
      delete[] payload;
    }

    uint32_t cmd;
    uint32_t arg0;
    uint32_t arg1;
    uint8_t* payload;
    uint32_t payload_len;
  };


  // Subclass AdbEndpoint and implement on_message to receive messages
  // from the device. 
  class AdbEndpoint : public UsbEndpoint, public wxThread
  {
  public:
    AdbEndpoint();
    ~AdbEndpoint();

    int send(uint32_t cmd, uint32_t arg0, uint32_t arg1, uint8_t* buf = 0, uint32_t len = 0);

    

  protected:
    virtual ExitCode Entry();
  private:
    int on_data_received(const uint8_t* buffer, uint32_t length);
  };

} // namespace 

#endif
