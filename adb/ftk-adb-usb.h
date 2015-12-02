#ifndef __adb_usb_h__
#define __adb_usb_h__

#include <list>
#include <wchar.h>
#include <stdint.h>
#include <openssl/rsa.h>

#include "wx/thread.h"
#include "wx/clntdata.h"

namespace ftk {

  const int kMaxFilePath = 2048;
  const int kMaxTransferSize = 512;

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

    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t serial_no;

    wxStringClientData unique_id;

    AdbDevice() {
      static uint32_t __unique_id = 1000;
      unique_id.SetData(wxString::Format(wxT("usbdevice%d"),__unique_id++));
      device_name[0] = 0;
      device_path[0] = 0;
      vendor_id = product_id = 0;
      serial_no = 0;
    }
    AdbDevice(const AdbDevice& left) {
      *this = left;
    }
    AdbDevice& operator=(const AdbDevice& left) {
      wcscpy(device_name, left.device_name);
      wcscpy(device_path, left.device_path);
      vendor_id = left.vendor_id;
      product_id = left.product_id;
      unique_id = left.unique_id;
      addr_out = left.addr_out;
      addr_in = left.addr_in;
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
    int open_device_by_id(const char* unique_id);
    int close_device();

    bool isOpen() const;

    // queue the data to be sent (async)
    int send(const uint8_t* buffer, uint32_t length);

    void poll();

    // callback when bulk data  has been received.
    virtual int on_data_received(const uint8_t* buffer, uint32_t length) = 0;
    virtual int on_devices_changed() = 0;

    void log(const char* szFormat, ...);

  private:
    UsbEndpoint& operator=(const UsbEndpoint&);
    UsbEndpoint(const UsbEndpoint&);

    struct UsbEndpointInternals* _i;

  };


  struct AdbMessage {
    AdbMessage() : payload(0), payload_len(0) {
      payload = new uint8_t[kMaxTransferSize];
    }
    ~AdbMessage() {
      delete[] payload;
    }

    void set_payload(const uint8_t* buf, uint32_t len) {
      if (len > kMaxTransferSize) {
        wxLogError("adb payload %d bytes larger than %d (kMaxTransferSize)", len, kMaxTransferSize);
        return;
      }
      memcpy(payload, buf, len);
      payload_len = len;
    }
    void reset() { cmd = arg0 = arg1 = payload_len = 0; }

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

    int connect_to_device_by_id(const wxString& id);


  protected:
    virtual ExitCode Entry();

    int send_adb_message(uint32_t cmd, uint32_t param1, uint32_t param2, const uint8_t* data=NULL, uint32_t len=0);
  private:
    virtual int on_data_received(const uint8_t* buffer, uint32_t length);
    virtual int on_devices_changed();

    virtual int on_adb_message(const AdbMessage& msg);

    bool generate_key();
    bool load_key();

    AdbMessage _message;
    RSA* _key;
    int _auth_attempt;
  };

} // namespace 

#endif
