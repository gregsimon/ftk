

#include "wx/wxprec.h"

#include <stdarg.h>
#include <libusb-1.0/libusb.h>

#include "ftk-adb-usb.h"



const int kMaxReceiveSize = 256 * 1024;

namespace ftk {

  struct UsbEndpointInternals {
    UsbEndpointInternals(UsbEndpoint* owner) : usb(owner),
          hdev(0), read_buffer_sz(512) {
      read_buffer = new uint8_t[read_buffer_sz];
      libusb_init(NULL);
    }
    ~UsbEndpointInternals() {      
      libusb_exit(NULL);
      delete[] read_buffer;
    }

    static void BulkReadComplete(struct libusb_transfer *xfr);
    static void BulkWriteComplete(struct libusb_transfer *xfr);

    UsbEndpoint* usb;

    uint8_t open_addr_in;   // current device in/out address.
    uint8_t open_addr_out;
    libusb_hotplug_callback_handle h_hotplug_handle;
    libusb_device_handle* hdev;
    libusb_transfer *read_xfr;
    libusb_transfer *write_xfr;
    uint8_t* read_buffer;
    uint16_t read_buffer_sz;

  
    uint32_t incoming_received_sz;
    uint8_t incoming_msg[kMaxReceiveSize];

    int refresh_devices();
    AdbDeviceList devices;

  };

  static int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) {

    UsbEndpointInternals* i = (UsbEndpointInternals*)user_data;
    i->refresh_devices();
    i->usb->on_devices_changed();
    return 0;
  }

  UsbEndpoint::UsbEndpoint() : _i(new UsbEndpointInternals(this))
  {
    int rc = libusb_hotplug_register_callback(NULL, 
          libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED|LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT), 
          libusb_hotplug_flag(0), 
          LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, 
          LIBUSB_HOTPLUG_MATCH_ANY, 
          hotplug_callback, 
          _i, &(_i->h_hotplug_handle));
    if (LIBUSB_SUCCESS != rc) {
      wxLogError("Failed to register libusb hotplug event %d", rc);
    }

    _i->refresh_devices();
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

  void UsbEndpoint::poll()
  {
    struct timeval tvs = {0};
    tvs.tv_sec = 1;

    libusb_handle_events_timeout_completed(NULL, &tvs, NULL);
  }

  int UsbEndpoint::list_devices(AdbDeviceList& devices) {
    devices = _i->devices;
    return devices.size();
  }

  int UsbEndpointInternals::refresh_devices()
  {
    libusb_device **devs;
    int r, j=0, k=0;
    ssize_t cnt;
    libusb_device *dev;
    uint8_t path[8];
    
    devices.clear();

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
            usb->log("name %s  %d config(s)\n", (char*)str, (int)desc.bNumConfigurations);
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
                    usb->log("[%s]\n", (char*)str);
                    might_be_device = true;
                  }
                }
              }

              if (might_be_device) {
                // now pull the endpoints out of iface->endpoint
                for (int e=0; e<iface->bNumEndpoints; ++e) {
                  const libusb_endpoint_descriptor& ep = iface->endpoint[e];
                  usb->log(" endpoint %02x addr=%02x %s bmAttributes=%02x max_size=%d\n",
                    ep.bDescriptorType,
                    ep.bEndpointAddress,
                    (ep.bEndpointAddress & 0x80) ? "IN" : "OUT",
                    ep.bmAttributes,
                    ep.wMaxPacketSize);

                  if (ep.bEndpointAddress & 0x80)
                    new_device.addr_in = ep.bEndpointAddress;
                  else //if (ep.bEndpointAddress & 0x80)
                    new_device.addr_out = ep.bEndpointAddress;
                }
              }
            }

            libusb_free_config_descriptor(config);
          }
        }

        if (new_device.addr_in || new_device.addr_out)
          this->devices.push_back(new_device);
        libusb_close(h);
      }

    }

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
    int rc;

    for (AdbDeviceList::iterator it=_i->devices.begin(); 
      it != _i->devices.end(); ++it) {
      if (!it->unique_id.GetData().Cmp(wxString(unique_id))) {
        if (!(_i->hdev = libusb_open_device_with_vid_pid(NULL, it->vendor_id, it->product_id))) {
          wxLogError("Unable to open usb device %04x %04x\n", it->vendor_id, it->product_id);
          return -2;
        }
        wxLogDebug("Opened usb device %04x %04x in=%02x out=%02x", 
            it->vendor_id, it->product_id, it->addr_in, it->addr_out);

        _i->open_addr_in = it->addr_in;
        _i->open_addr_out = it->addr_out;

        // start listening for bulk transfers

        _i->read_xfr = libusb_alloc_transfer(0);
        libusb_fill_bulk_transfer(_i->read_xfr,
            _i->hdev,
            it->addr_in, // endpoint ID
            _i->read_buffer,
            _i->read_buffer_sz,
            UsbEndpointInternals::BulkReadComplete,
            _i,
            5000);

        if (libusb_submit_transfer(_i->read_xfr) < 0) {
          wxLogError("Failed to submit bulk read transfer ep=%d", it->addr_in);
        }


        return 0;
      }
    }

    return -1;
  }

  void UsbEndpointInternals::BulkReadComplete(struct libusb_transfer *xfr)
  {
    UsbEndpointInternals* i = (UsbEndpointInternals*)xfr->user_data;

    switch(xfr->status)
    {
      case LIBUSB_TRANSFER_COMPLETED:
        //LogDebug("BulkReadComplete %d bytes", xfr->actual_length);
        i->usb->on_data_received((const uint8_t*)xfr->buffer, (uint32_t)xfr->actual_length);
        break;
      case LIBUSB_TRANSFER_CANCELLED:
      case LIBUSB_TRANSFER_NO_DEVICE:
      case LIBUSB_TRANSFER_TIMED_OUT:
      case LIBUSB_TRANSFER_ERROR:
      case LIBUSB_TRANSFER_STALL:
      case LIBUSB_TRANSFER_OVERFLOW:
        // Various type of errors here
        wxLogError("BulkReadComplete error, status = %d", xfr->status);
        break;
    }

    // re-prime the read transfer
    libusb_free_transfer(i->read_xfr);

    i->read_xfr = libusb_alloc_transfer(0);
        libusb_fill_bulk_transfer(i->read_xfr,
            i->hdev,
            i->open_addr_in, // endpoint ID
            i->read_buffer,
            i->read_buffer_sz,
            UsbEndpointInternals::BulkReadComplete,
            i,
            5000);

        if (libusb_submit_transfer(i->read_xfr) < 0) {
          wxLogError("Failed to submit bulk read transfer ep=%d", i->open_addr_in);
        }
  }

  int UsbEndpoint::close_device()
  {
    if (_i->hdev) {
      libusb_close(_i->hdev);
      _i->hdev = NULL;
    }
    return 0;
  }

  int UsbEndpoint::send(const uint8_t* buffer, uint32_t length)
  {
    int rc;

    _i->write_xfr = libusb_alloc_transfer(0);
    libusb_fill_bulk_transfer(_i->write_xfr,
            _i->hdev,
            _i->open_addr_out, // endpoint ID
            (unsigned char*)buffer, // Grr
            length,
            UsbEndpointInternals::BulkWriteComplete,
            _i,
            5000);

    if ((rc = libusb_submit_transfer(_i->write_xfr)) < 0) {
      wxLogError("Failed to submit bulk write transfer err=%d ep=%d", rc, _i->open_addr_out);
    }

    return 0;
  }

  void UsbEndpointInternals::BulkWriteComplete(struct libusb_transfer *xfr)
  {
    UsbEndpointInternals* i = (UsbEndpointInternals*)xfr->user_data;

    switch(xfr->status)
    {
      case LIBUSB_TRANSFER_COMPLETED:
        //wxLogDebug("BulkWriteComplete %d bytes", xfr->actual_length);
        break;
      case LIBUSB_TRANSFER_CANCELLED:
      case LIBUSB_TRANSFER_NO_DEVICE:
      case LIBUSB_TRANSFER_TIMED_OUT:
      case LIBUSB_TRANSFER_ERROR:
      case LIBUSB_TRANSFER_STALL:
      case LIBUSB_TRANSFER_OVERFLOW:
        // Various type of errors here
        wxLogError("BulkWriteComplete error, status = %d", xfr->status);
        break;
    }
  }

  bool UsbEndpoint::isOpen() const
  {
    return (_i->hdev != NULL);
  }


} // namespace ftk