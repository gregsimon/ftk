

#include "wx/wxprec.h"

#include <stdarg.h>
#include <libusb-1.0/libusb.h>

#include "ftk-adb-usb.h"



const int kMaxReceiveSize = 256 * 1024;

namespace ftk {


  struct UsbEndpointInternals {
    UsbEndpointInternals() {
      libusb_init(NULL);
    }
    ~UsbEndpointInternals() {
      libusb_exit(NULL);
    }
  
    uint32_t incoming_received_sz;
    uint8_t incoming_msg[kMaxReceiveSize];

    uint8_t addr_out;
    uint8_t addr_in;
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
    va_list arg;
    va_start(arg, szFormat);
    vprintf(szFormat, arg);
    va_end(arg);
  }



  int UsbEndpoint::list_devices(AdbDeviceList& devices)
  {
    libusb_device **devs;
    int r, j=0, k=0;
    ssize_t cnt;
    libusb_device *dev;
    uint8_t path[8];
   

    cnt = libusb_get_device_list(NULL, &devs);
    if (!cnt)
      return 0;

    while ((dev = devs[j++]) != NULL)
    {
      struct libusb_device_descriptor desc;
      int r = libusb_get_device_descriptor(dev, &desc);
      if (r < 0) {
        fprintf(stderr, "failed to get device descriptor");
        return -1;
      }

      libusb_device_handle* h;
      if (!libusb_open(dev, &h)) {
        AdbDevice new_device;
        unsigned char data[42];
        unsigned char str[256];

        new_device.addr_out = new_device.addr_in = 0;


        if (desc.iProduct) {
          if (libusb_get_string_descriptor_ascii(h, desc.iProduct, str, 255) > 0) {
            log("name %s  %d configs\n", (char*)str, (int)desc.bNumConfigurations);
            mbstowcs(new_device.device_name, (const char*)str, kMaxFilePath);
          }
        }

        for (int c=0; c<desc.bNumConfigurations; c++) {
          libusb_config_descriptor *config;
          if (!libusb_get_config_descriptor(dev, c, &config)) {

            for (int i=0; i<config->bNumInterfaces; ++i) {
              const libusb_interface_descriptor* iface = config->interface[i].altsetting;
              if (iface->iInterface) {
                if (libusb_get_string_descriptor_ascii(h, iface->iInterface, str, 255) > 0) {
                  
                  if (strstr((const char*)str, "ADB Interface")) {
                    log("[%s]\n", (char*)str);

                    // now pull the endpoints out of iface->endpoint
                    for (int e=0; e<iface->bNumEndpoints; ++e) {
                      const libusb_endpoint_descriptor& ep = iface->endpoint[e];
                      log(" endpoint %02x addr=%02x %s bmAttributes=%02x max_size=%d\n", 
                        ep.bDescriptorType,
                        ep.bEndpointAddress,
                        (ep.bEndpointAddress & 0x80) ? "IN" : "OUT",
                        ep.bmAttributes,
                        ep.wMaxPacketSize);

                      
                      if (ep.bEndpointAddress & 0x80)
                        new_device.addr_in = ep.bEndpointAddress;
                      else if (ep.bEndpointAddress & 0x80)
                        new_device.addr_out = ep.bEndpointAddress;
                      
                    }
                  }
                }
              }
            }

            libusb_free_config_descriptor(config);
          }
        }

        if (new_device.addr_in || new_device.addr_out)
          devices.push_back(new_device);
        libusb_close(h);
      }

    }

    return cnt;
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