

#include "wx/wxprec.h"

#include <winusb.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <cfgmgr32.h>

#include "ftk-adb-usb.h"

#include <initguid.h>


DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
	0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

const int kMaxReceiveSize = 256 * 1024;

namespace ftk {


  struct UsbEndpointInternals
  {
    WINUSB_INTERFACE_HANDLE hInterface;
    HANDLE hFile;

    OVERLAPPED overlapped;
    HANDLE receive_event;

    ULONG incoming_received_sz;
    uint8_t incoming_msg[kMaxReceiveSize];

    UCHAR read_endpoint, write_endpoint;
    UCHAR read_endpoint_id, write_endpoint_id;
  };

  UsbEndpoint::UsbEndpoint() : _i(new UsbEndpointInternals)
  {
  }

  UsbEndpoint::~UsbEndpoint()
  {
    delete _i;
  }

  void UsbEndpoint::log(const char* szFormat, ...)
  {
    char szBuff[1024];

    va_list arg;
    va_start(arg, szFormat);
    _vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
    va_end(arg);

    OutputDebugStringA(szBuff);
  }



  int UsbEndpoint::list_devices(AdbDeviceList& devices)
  {
    HDEVINFO dev_info = NULL;
    SP_DEVINFO_DATA DeviceInfoData = { sizeof(DeviceInfoData) };
    const int bufSize = 4096;
    LPSTR prop_buffer = (LPSTR)LocalAlloc(LPTR, bufSize);
    DWORD required_size;
    BOOL bRes;

    dev_info = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(dev_info, i, &DeviceInfoData); i++)
    {
      SP_DEVICE_INTERFACE_DATA did = { sizeof(did) };
      PSP_DEVICE_INTERFACE_DETAIL_DATA pdd = NULL;
      DWORD idx = 0;
      while (1)
      {
        DWORD err;

        // Are there any interfaces left? -- get the next interface.
        bRes = SetupDiEnumDeviceInterfaces(dev_info, &DeviceInfoData, &GUID_DEVINTERFACE_USB_DEVICE, idx++, &did);
        if (!bRes)
        {
          err = GetLastError();
          if (ERROR_NO_MORE_DEVICES == err ||
            ERROR_NO_MORE_ITEMS == err)
            break;
        }

        // Figure out how much memory to allocate for this specific interface
        bRes = SetupDiGetDeviceInterfaceDetail(dev_info, &did, NULL, 0, &required_size, NULL);
        if (!bRes)
        {
          err = GetLastError();
          if (ERROR_INSUFFICIENT_BUFFER == err)
          {
            pdd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, required_size);
            pdd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
          }
          else
            break;
        }

        // Get the interface detail
        bRes = SetupDiGetDeviceInterfaceDetail(dev_info, &did, pdd, required_size, NULL, NULL);
        if (bRes)
        {
          TCHAR last_device_name[bufSize + 1];
          DEVINST child = 0;

          if (CR_SUCCESS == CM_Get_Child(&child, DeviceInfoData.DevInst, 0))
          {
            ZeroMemory(last_device_name, sizeof(last_device_name));

            // Iterate through the child interfaces looking for something called "ADB ..."
            while (1)
            {
              TCHAR interface_path[MAX_PATH + 1];
              ULONG len = bufSize;

              ZeroMemory(prop_buffer, bufSize);
              CM_Get_Device_ID(child, (LPTSTR)interface_path, MAX_PATH, 0);
              //log("  child = %S\n", interface_path);

              if (CR_SUCCESS == CM_Get_DevNode_Registry_Property_Ex(child, CM_DRP_DEVICEDESC, 0,
                prop_buffer, &len, 0, 0))
              {
                if (_tcsstr((LPCTSTR)prop_buffer, _T("ADB Interface"))) {
                  // parent : \\?\usb#vid_18d1&pid_4ee2#r32d100yyrd#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
                  // 3f966bd9-fa04-4ec5-991c-d326973b5128
                  //   this : USB\VID_18D1&PID_4EE2&MI_01\8&14E07994&0&0001
                  //
                  // --> _T("\\\\?\\usb#vid_18d1&pid_4ee2&mi_01#8&14e07994&0&0001#{F72FE0D4-CBCB-407d-8814-9ED673D0DD6B}")
                  TCHAR fullpath[MAX_PATH + 1];
                  TCHAR regpath[MAX_PATH + 1];
                  DWORD regpath_size = MAX_PATH;
                  LONG lres;
                  HKEY hkey;

                  // Read the DeviceInterfaceGUIDs out of the registry
                  _stprintf_s(regpath, _T("SYSTEM\\CurrentControlSet\\Enum\\%s\\Device Parameters"), interface_path);

                  if (ERROR_SUCCESS == (lres = RegOpenKeyEx(HKEY_LOCAL_MACHINE, regpath, 0, KEY_READ, &hkey)))
                  {
                    if (ERROR_SUCCESS == (lres = RegQueryValueEx(hkey, _T("DeviceInterfaceGUIDs"), 0, NULL, (LPBYTE)regpath, &regpath_size)))
                    {
                      for (int i = 0; i < _tcslen(interface_path); ++i)
                        if (interface_path[i] == '\\')
                          interface_path[i] = '#';

                      _stprintf_s(fullpath, _T("\\\\?\\%s#%s"), interface_path, regpath);
                      //log("Found \"%S\" at %S\n", last_device_name, fullpath);

                      AdbDevice dev;
                      _tcscpy_s(dev.device_name, last_device_name);
                      _tcscpy_s(dev.device_path, fullpath);
                      devices.push_back(dev);
                    }
                    else {
                      log("ERROR : coulnd't find registry value 'DeviceInterfaceGUIDs'\n");
                    }

                    RegCloseKey(hkey);
                  }
                  else {
                    log("ERROR : coulnd't find registry value 'Device Parameters'\n");
                  }

                } // Name is "ADB"

                  // Usually the first interface is the device Name so we'll save that in case
                  // the second is in fact ADB ... 
                _tcscpy_s(last_device_name, (LPCTSTR)prop_buffer);
              }

              DWORD current = child;
              if (CR_SUCCESS != CM_Get_Sibling(&child, current, 0))
                break;
            }
          }
        }
        else
        {
          err = GetLastError();
          log("Last Err: %d\n", err);
        }
        LocalFree(pdd);
        pdd = NULL;
      }
    }

    SetupDiDestroyDeviceInfoList(dev_info);
    return 0;
  }

  int UsbEndpoint::open_device(const AdbDevice& d)
  {

    _i->hFile = CreateFile(d.device_path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
      0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (INVALID_HANDLE_VALUE == _i->hFile)
      return -1;

    if (!WinUsb_Initialize(_i->hFile, &_i->hInterface)) {
      CloseHandle(_i->hFile);
      return -2;
    }

    // we need to get the endpoint address
    USB_INTERFACE_DESCRIPTOR iface_desc;
    if (!WinUsb_QueryInterfaceSettings(_i->hInterface, 0, &iface_desc)) {
      WinUsb_Free(_i->hInterface);
      CloseHandle(_i->hFile);
      return -3;
    }



    for (int ep = 0; ep < iface_desc.bNumEndpoints; ++ep)
    {
      WINUSB_PIPE_INFORMATION pipe_info;
      if (WinUsb_QueryPipe(_i->hInterface, 0, ep, &pipe_info)) {
        if (pipe_info.PipeType == UsbdPipeTypeBulk) {
          if (0 != (pipe_info.PipeId & USB_ENDPOINT_DIRECTION_MASK)) {
            _i->read_endpoint = ep;
            _i->read_endpoint_id = pipe_info.PipeId;
          }
          else {
            _i->write_endpoint = ep;
            _i->write_endpoint_id = pipe_info.PipeId;
          }
        }
      }

    }

    WINUSB_PIPE_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    WinUsb_QueryPipe(_i->hInterface, 0, _i->write_endpoint, &pi);

    // listen for the next msg from the device.
    _i->receive_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    _i->overlapped.hEvent = _i->receive_event;
    //(i->incoming_msg, kAdb);

    WinUsb_ReadPipe(_i->hInterface, _i->read_endpoint_id, _i->incoming_msg, kAdbHeaderSize, &_i->incoming_received_sz, &_i->overlapped);

    // Say hello to the device!
    //adb_queue_outgoing_msg(hInterface, write_endpoint_id, A_CNXN, 0x01000000L, kMaxReceiveSize, "host::Flutter ToolKit");


    //ZeroMemory(&pi, sizeof(pi));
    //WinUsb_QueryPipe(hInterface, 0, write_endpoint, &pi);

    //WinUsb_Free(hInterface);
    //CloseHandle(hFile);
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

  //virtual int on_data_received(const uint8_t* buffer, uint32_t length) = 0;

} // namespace ftk