

#include "wx/wxprec.h"


#include "ftk-adb-usb.h"

#define PackU32(t,o,v) t[3+o]=((v>>24)&0xff); t[2+o]=((v>>16)&0xff); t[1+o]=((v>>8)&0xff); t[0+o]=((v>>0)&0xff)
#define UnpackU32(t,o) ( ((uint32_t(t[3+o])<< 24) ) | ((uint32_t(t[2+o]) << 16) ) | ((uint32_t(t[1+o]) << 8) ) | (((uint32_t(t[0+o]) << 0) )))

namespace ftk {

  AdbEndpoint* currentAdbEndpoint() {
    static AdbEndpoint* ep = NULL;
    if (!ep)
      ep = new AdbEndpoint();
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

      wxLogDebug("in thread...");
    }

    return (wxThread::ExitCode)0;
  }

  int AdbEndpoint::send(uint32_t cmd, uint32_t arg0, uint32_t arg1, uint8_t* buf, uint32_t len)
  {
    return 0;
  }

  int AdbEndpoint::on_data_received(const uint8_t* buf, uint32_t len)
  {

    return 0;
  }

} // namespace ftk