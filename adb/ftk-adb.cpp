

#include "wx/wxprec.h"

#include <openssl/pem.h>

#include "../src/ftk.h"
#include "../src/settings.h"

#include "wx/filename.h"
#include "ftk-adb-usb.h"

#define PackU32(t,o,v) t[3+o]=((v>>24)&0xff); t[2+o]=((v>>16)&0xff); t[1+o]=((v>>8)&0xff); t[0+o]=((v>>0)&0xff)
#define UnpackU32(t,o) ( ((uint32_t(t[3+o])<< 24) ) | ((uint32_t(t[2+o]) << 16) ) | ((uint32_t(t[1+o]) << 8) ) | (((uint32_t(t[0+o]) << 0) )))

const int kMaxReceiveSizeAdb = 256 * 1024;

namespace ftk {

  static const char* token_name(uint32_t t) {
    switch (t) {
    case A_SYNC: return "A_SYNC";
    case A_CNXN: return "A_CNXN";
    case A_OPEN: return "A_OPEN";
    case A_OKAY: return "A_OKAY";
    case A_CLSE: return "A_CLSE";
    case A_WRTE: return "A_WRTE";
    case A_AUTH: return "A_AUTH";
    }
    return "UNKNOWN";
  }

  AdbEndpoint* currentAdbEndpoint() {
    static AdbEndpoint* ep = NULL;
    if (!ep) {
      ep = new AdbEndpoint();
      ep->Run();
    }
    return ep;
  }

  AdbEndpoint::AdbEndpoint() : _key(0), _auth_attempt(0)
  {
    if (!load_key()) {      
    }
  }

  AdbEndpoint::~AdbEndpoint()
  {
    if (_key)
      RSA_free(_key);
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

    _auth_attempt = 0;

    if ((rc = open_device_by_id(unique_id)))
      return rc;

    // kick off the hello sequence.
    const char* kHostName = "host:0000:Flutter ToolKit";
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

  int AdbEndpoint::on_adb_message(const AdbMessage& msg) 
  {
    wxLogDebug("IN : %s %04x %04x %d bytes",
      token_name(msg.cmd),
      msg.arg0, msg.arg1,
      msg.payload_len);

    switch (msg.cmd) {
      case A_AUTH:
        if (msg.arg0 == 1) {  // token
          // msg.payload_len is a random token that the recipient can sign 
          // with a private key
          if (!_key) {
            if (!generate_key()) {
              wxLogError("GenerateKey failed.");
            }
          }

          // this is a key unknown to the device, let's
          // send the (new) public key over. token type=3

          if (0 == _auth_attempt) {
            // sign with the key we have.
            unsigned char* buf1;
            unsigned char* buf2;
            int len = i2d_RSAPublicKey(_key, 0);
            buf1 = buf2 = (unsigned char *)malloc(len + 1);
            memset(buf1, 0, len+1);
            i2d_RSAPublicKey(_key, (unsigned char **)&buf2);

            send_adb_message(A_AUTH, 3, 0, buf1, len);
            
            free (buf1);

            _auth_attempt ++ ;
          }



        }
        break;
    }
    return 0;
  }

  

  int AdbEndpoint::on_data_received(const uint8_t* buf, uint32_t len)
  {
    bool is_header = false;
    if (kAdbHeaderSize == len) {
      // this may be a header. Or not. Let's check to make sure.
      uint32_t cmd = UnpackU32(buf,0);
      uint32_t cmd2 = UnpackU32(buf, 20);
      if ((cmd ^ 0xffffffff) == cmd2 ) {
        is_header = true;

        _message.cmd = cmd;
        _message.arg0 = UnpackU32(buf,4);
        _message.arg1 = UnpackU32(buf,8);
        _message.payload_len = UnpackU32(buf,12);
      }
    }

    if (!is_header) {
      // This should be the payload as part of the last bulk transfer.
      // match it up and send the adb message for processing.
      if (_message.payload_len != len) {
        // error! we either had a buffer under/overrun in the driver
        // or we're out of sync with the message processing. 
        // Drop this buffer on the floor and continue.
        wxLogError("on_data_received: payload length != header size");
        _message.reset();
        return -1;
      }

      on_adb_message(_message);
      _message.reset();
    }

    return 0;
  }

  // Attempt to load private key.
  bool AdbEndpoint::load_key()
  {
    BIO* bp_public;
    BIO* bp_private;

    wxFileName key_file_name(globalSettings()->settings_folder());

    key_file_name.SetName("adb_public.pem");
    bp_public = BIO_new_file(key_file_name.GetFullPath(), "r");
    if (!bp_public) {
      wxLogError("Unable to open public key file %s", key_file_name.GetFullPath());
      return false;
    }
    

    key_file_name.SetName("adb_private.pem");
    bp_private = BIO_new_file(key_file_name.GetFullPath(), "r");
    if (!bp_private) {
      wxLogError("Unable to open private key file %s", key_file_name.GetFullPath());
      BIO_free_all(bp_public);
      return false;
    }

    _key = RSA_new();
    PEM_read_bio_RSAPublicKey(bp_public, &_key, NULL, NULL);
    //PEM_read_bio_RSAPrivateKey(bp_private, &_key, NULL, NULL);
    
    BIO_free_all(bp_private);
    BIO_free_all(bp_public);

    return true;
  }

  bool AdbEndpoint::generate_key()
  {
    bool success = false;
    BIGNUM* bne = NULL;
    //BIO* bp_public = NULL;
    //BIO* bp_private = NULL;
    int bits = 2048;
    unsigned long e = RSA_F4;

    // Generate RSA key
    bne = BN_new();
    if (1 == BN_set_word(bne, e))
    {
      _key = RSA_new();
      if (1 == RSA_generate_key_ex(_key, bits, bne, NULL))
      {
        success = true;
        int ret;
        BIO* bp;

        wxLogDebug("generated RSA key");

        // save it.
        wxFileName key_file_name(globalSettings()->settings_folder());

        key_file_name.SetName("adb_public.pem");
        bp = BIO_new_file(key_file_name.GetFullPath(), "w+");
        ret = PEM_write_bio_RSAPublicKey(bp, _key);
        BIO_free_all(bp);

        key_file_name.SetName("adb_private.pem");
        bp = BIO_new_file(key_file_name.GetFullPath(), "w+");
        ret = PEM_write_bio_RSAPrivateKey(bp, _key, NULL, NULL, 0, NULL, NULL);
        BIO_free_all(bp);
      }
      BN_free(bne);
    }

    return success;
  }

} // namespace ftk