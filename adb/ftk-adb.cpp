

#include "wx/wxprec.h"


#include "ftk-adb-usb.h"

#define PackU32(t,o,v) t[3+o]=((v>>24)&0xff); t[2+o]=((v>>16)&0xff); t[1+o]=((v>>8)&0xff); t[0+o]=((v>>0)&0xff)
#define UnpackU32(t,o) ( ((uint32_t(t[3+o])<< 24) ) | ((uint32_t(t[2+o]) << 16) ) | ((uint32_t(t[1+o]) << 8) ) | (((uint32_t(t[0+o]) << 0) )))

const int kMaxReceiveSizeAdb = 256 * 1024;

namespace ftk {

  AdbEndpoint* currentAdbEndpoint() {
    static AdbEndpoint* ep = NULL;
    if (!ep) {
      ep = new AdbEndpoint();
      ep->Run();
    }
    return ep;
  }

  AdbEndpoint::AdbEndpoint()
  {
  }

  AdbEndpoint::~AdbEndpoint()
  {
  }

  wxThread::ExitCode AdbEndpoint::Entry()
  {
    while (!TestDestroy())
    {
      poll(); // kick the usb stack
    }

    return (wxThread::ExitCode)0;
  }

  int AdbEndpoint::connect_to_device_by_id(const wxString& unique_id)
  {
    int rc;

    if ((rc = open_device_by_id(unique_id)))
      return rc;

    // kick off the hello sequence.
    const char* kHostName = "host::Flutter ToolKit";
    send_adb_message(A_CNXN, 0x01000000L, kMaxReceiveSizeAdb, (const uint8_t *)kHostName, (uint32_t)strlen(kHostName));

    return 0;
  }

  int AdbEndpoint::send_adb_message(uint32_t cmd, uint32_t param1, uint32_t param2, const uint8_t* data, uint32_t len)
  {
    // two bulk transfers: header, payload.

    uint8_t header[kAdbHeaderSize];
    uint32_t crc = 0;

    for (int i=0; i<len; i++)
      crc += data[i];

    PackU32(header, 0, cmd);
    PackU32(header, 4, param1);
    PackU32(header, 8, param2);
    PackU32(header, 12 ,len);
    PackU32(header, 16 ,crc);
    PackU32(header, 20, (cmd ^ 0xffffffff));

    send(header, 24);

    send(data, len);
  }

  int AdbEndpoint::on_devices_changed()
  {
    wxLogDebug(" ... device list changed ... ");
    return 0;
  }

  int AdbEndpoint::on_data_received(const uint8_t* buf, uint32_t len)
  {

    return 0;
  }

} // namespace ftk