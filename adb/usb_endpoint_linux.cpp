

#include "wx/wxprec.h"

#include <stdarg.h>
#include <libusb-1.0/libusb.h>

#include "ftk-adb-usb.h"



const int kMaxReceiveSize = 256 * 1024;

namespace ftk {

  static int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) {
    //return ((UsbEndpoint*)user_data)->hotplug_callback(ctx, dev, event);

    wxLogDebug("hotplug_callback ... ");

    return 0;
  }

  struct UsbEndpointInternals {
    UsbEndpointInternals() :hdev(0) {
      libusb_init(NULL);
    }
    ~UsbEndpointInternals() {
      libusb_exit(NULL);
    }

    libusb_hotplug_callback_handle h_hotplug_handle;
    libusb_device_handle* hdev;
  
    uint32_t incoming_received_sz;
    uint8_t incoming_msg[kMaxReceiveSize];

    AdbDeviceList devices;

  };


  UsbEndpoint::UsbEndpoint() : _i(new UsbEndpointInternals)
  {
    int rc = libusb_hotplug_register_callback(NULL, 
          libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), 
          libusb_hotplug_flag(0), 
          LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, 
          LIBUSB_HOTPLUG_MATCH_ANY, 
          hotplug_callback, 
          this, &(_i->h_hotplug_handle));
    if (LIBUSB_SUCCESS != rc) {
      wxLogError("Failed to register libusb hotplug event %d", rc);
    }
  }

  UsbEndpoint::~UsbEndpoint()
  {
    libusb_hotplug_deregister_callback(NULL, _i->h_hotplug_handle);
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
        bool might_be_device = false;

        new_device.addr_out = new_device.addr_in = 0;
        new_device.vendor_id = desc.idVendor;
        new_device.product_id = desc.idProduct;
        new_device.serial_no = desc.iSerialNumber;


        if (desc.iProduct) {
          if (libusb_get_string_descriptor_ascii(h, desc.iProduct, str, 255) > 0) {
            log("name %s  %d config(s)\n", (char*)str, (int)desc.bNumConfigurations);
            if (strstr((const char*)str, "Nexus"))
              might_be_device = true;
            mbstowcs(new_device.device_name, (const char*)str, kMaxFilePath);
          }
        }

        for (int c=0; c<desc.bNumConfigurations; c++) {
          libusb_config_descriptor *config;
          if (!libusb_get_config_descriptor(dev, c, &config)) {

            for (int i=0; i<config->bNumInterfaces; ++i) {
              const libusb_interface_descriptor* iface = config->interface[i].altsetting;

              // try and get the interface name. There may not be one, in which case
              // we should double-check the device name for "Nexus"
              if (iface->iInterface) {
                if (libusb_get_string_descriptor_ascii(h, iface->iInterface, str, 255) > 0) {
                  if (strstr((const char*)str, "ADB Interface")) {
                    log("[%s]\n", (char*)str);
                    might_be_device = true;
                  }
                }
              }

              if (might_be_device) {
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

            libusb_free_config_descriptor(config);
          }
        }

        if (new_device.addr_in || new_device.addr_out)
          devices.push_back(new_device);
        libusb_close(h);
      }

    }

    _i->devices = devices;

    return cnt;
  }

  int UsbEndpoint::open_device(const AdbDevice& d)
  {
    // locate the device based on the vid, pid, and serial #.
    // TODO use serial #
    if (!(_i->hdev = libusb_open_device_with_vid_pid(NULL, d.vendor_id, d.product_id))) {
      wxLogError("Unable to open usb device %04x %04x\n", d.vendor_id, d.product_id);
      return -1;
    }

    wxLogDebug("Opened usb device.\n");

    return 0;
  }

  int UsbEndpoint::open_device_by_id(const char* unique_id)
  {
    for (AdbDeviceList::iterator it=_i->devices.begin(); 
      it != _i->devices.end(); ++it) {
      if (!it->unique_id.GetData().Cmp(wxString(unique_id))) {
        if (!(_i->hdev = libusb_open_device_with_vid_pid(NULL, it->vendor_id, it->product_id))) {
          wxLogError("Unable to open usb device %04x %04x\n", it->vendor_id, it->product_id);
          return -2;
        }
        wxLogDebug("Opened usb device!!");
        return 0;
      }
    }

    return -1;
  }

  int UsbEndpoint::close_device()
  {
    if (_i->hdev) {
      libusb_close(_i->hdev);
      _i->hdev = NULL;
    }
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