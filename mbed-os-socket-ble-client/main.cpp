#include "mbed.h"

#include "ble/BLE.h"
#include "gap/Gap.h"
#include "gap/AdvertisingDataParser.h"
#include "pretty_printer.h"

Thread thread;
EventQueue event_queue;

typedef struct {
    ble::scan_interval_t interval;
    ble::scan_window_t   window;
    ble::scan_duration_t duration;
    bool active;
} GapScanParam_t;

/** the entries in this array are used to configure our scanning
 *  parameters for each of the modes we use in our demo */
static const GapScanParam_t scanning_params[] = {
/*                      interval                  window                   duration  active */
/*                      0.625ms                  0.625ms                       10ms         */
    {   ble::scan_interval_t(4),   ble::scan_window_t(4),   ble::scan_duration_t(5), true },
    { ble::scan_interval_t(160), ble::scan_window_t(100), ble::scan_duration_t(300), false },
    { ble::scan_interval_t(160),  ble::scan_window_t(40),   ble::scan_duration_t(0), true  },
    { ble::scan_interval_t(500),  ble::scan_window_t(10),   ble::scan_duration_t(0), true },
    { ble::scan_interval_t(500),  ble::scan_window_t(100),   ble::scan_duration_t(100), true }
};

/** Demonstrate scanning
 */

class GapScanner : private mbed::NonCopyable<GapScanner>, public ble::Gap::EventHandler {
public:
    GapScanner(BLE& ble, events::EventQueue& event_queue) :
        _ble(ble),
        _gap(ble.gap()),
        _event_queue(event_queue),
        _led1(LED1, 0),
        _set_index(1),        
        _scan_count(0),
        _blink_event(0),
        b_scan(true),
        b_finish(true),
        PORT(4444),
        b_ble_init(false) {
    }

    ~GapScanner() {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
        }
        // Close the socket to return its memory and bring down the network interface
        socket.close();

        // Bring down the ethernet interface
        net->disconnect();
        printf("Done\n");
    }
    

    
/** Start BLE interface initialisation */
    void run() {
        if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
        }

        /* handle gap events */
        _gap.setEventHandler(this);

        init_socket();

        ble_error_t error = _ble.init(this, &GapScanner::on_init_complete);
        if (error) {
            print_error(error, "Error returned by BLE::init");
            return;
        }
        
        //printf("wait ble init ...");
        //while(!b_ble_init);
        //printf("ble init done ...");          

        /* to show we're running we'll blink every 500ms */
        _blink_event = _event_queue.call_every(500, this, &GapScanner::blink);

        //printf("run : before dispatch_forever\n");
        
        
        
        /* this will not return until shutdown */
        _event_queue.dispatch_forever();
        printf("run : done\n");
    }

    /** Set up and start scanning */
    void scan() {
        printf("b_scan is %d\n", b_scan);
        if( b_scan ) {
          const GapScanParam_t &scan_params = scanning_params[_set_index];

          /*
           * Scanning happens repeatedly and is defined by:
           *  - The scan interval which is the time (in 0.625us) between each scan cycle.
           *  - The scan window which is the scanning time (in 0.625us) during a cycle.
           * If the scanning process is active, the local device sends scan requests
           * to discovered peer to get additional data.
           */
          
          ble_error_t error = _gap.setScanParameters(
              ble::ScanParameters(
                  ble::phy_t::LE_1M,   // scan on the 1M PHY
                  scan_params.interval,
                  scan_params.window,
                  scan_params.active
              )
          );
          if (error) {
              print_error(error, "Error caused by Gap::setScanParameters");
              return;
          }
          
          /* start scanning and attach a callback that will handle advertisements
           * and scan requests responses */
          error = _gap.startScan(scan_params.duration);
          //error = _gap.startScan(scan_duration_t::forever(), duplicates_filter_t::DISABLE, scan_period_t(0));
          if (error) {
              print_error(error, "Error caused by Gap::startScan");
              return;
          }

          printf("Scanning started (interval: %dms, window: %dms, timeout: %dms).\r\n",
                 scan_params.interval.valueInMs(), scan_params.window.valueInMs(), scan_params.duration.valueInMs());        
        }
    }
    
    void start_scan() {
      _event_queue.call(this, &GapScanner::scan);
    }

private:
    /** This is called when BLE interface is initialised and starts the first mode */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event) {
        printf("on_init_complete : start\r\n");
        if (event->error) {
            print_error(event->error, "Error during the initialisation");
            return;
        }

        print_mac_address();

        /* setup the default phy used in connection to 2M to reduce power consumption */
        if (_gap.isFeatureSupported(ble::controller_supported_features_t::LE_2M_PHY)) {
            ble::phy_set_t phys(/* 1M */ false, /* 2M */ true, /* coded */ false);

            ble_error_t error = _gap.setPreferredPhys(/* tx */&phys, /* rx */&phys);
            if (error) {
                print_error(error, "GAP::setPreferedPhys failed");
            }
        }

        /* all calls are serialised on the user thread through the event queue */
        //_event_queue.call(this, &GapScanner::scan);
        //_event_queue.call_every(1000, this, &GapScanner::scan);
        printf("on_init_complete : done\r\n");
        b_ble_init = true;
        _event_queue.call(this, &GapScanner::socket_thread);
    }

    /** Blink LED to show we're running */
    void blink(void) {
        _led1 = !_led1;
    }
    
    int init_socket() {
      printf("init_socket : start\r\n");
      net = NetworkInterface::get_default_instance();

      if (!net) {
          printf("Error! No network inteface found.\n");
          return 0;
      }

      result = net->connect();
      if (result != 0) {
          printf("Error! net->connect() returned: %d\n", result);
          return result;
      }

      // Show the network address
      net->get_ip_address(&a);
      printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
      net->get_netmask(&a);
      printf("Netmask: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
      net->get_gateway(&a);
      printf("Gateway: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");

      // Send a simple http request
      char hostname[] = "192.168.43.66";
      char sbuffer[] = "toa_iot";
      nsapi_size_t size = strlen(sbuffer);

      result = socket.open(net);
      if (result != 0) {
          printf("Error! socket.open() returned: %d\n", result);
          //goto DISCONNECT;
      }

      // Get the host address
      printf("\nResolve hostname %s\n", hostname);
      result = net->gethostbyname(hostname, &a);
      if (result != 0) {
          printf("Error! gethostbyname(%s) returned: %d\n", hostname, result);
          net->disconnect();
          exit(-1);
      }
      printf("%s kkkkkkkkkkkkk is %s\n", hostname, (a.get_ip_address() ? a.get_ip_address() : "None") );
      a.set_port(PORT);
      result = socket.connect(a);
      if (result != 0) {
          printf("Error! socket.connect() returned: %d\n", result);
          net->disconnect();
          exit(-2);
      }
      printf("init_socket : done\r\n");
    }

    int socket_thread(){
      printf("socket_thread : start\r\n");
      //while(true) 
      {
        // Receieve an HTTP response and print out the response line
        remaining = 256;
        rcount = 0;
        p = buffer;
        if (0 < (result = socket.recv(p, remaining))) {
            p += result;
            rcount += result;
            remaining -= result;
            printf("buff: %s, remaining: %d, rcount: %d\n", buffer, remaining, rcount);
        }
        if (result < 0) {
            printf("Error! socket.recv() returned: %d\n", result);
            //goto DISCONNECT;
        }
	    // the HTTP response code
        printf("recv %d [%.*s]\n", rcount, strstr(buffer, "\r\n")-buffer, buffer);
        
        p = buffer;
        p_id = txt_id;
        p_save = txt_save;
        if( my_strcmp(p, p_id, rcount) ) {
          printf("my_strcmp --> #id$\n");
          // Loop until whole request sent
          //while (size) {
              result = socket.send(txt_name, sizeof(txt_name)-1);
              if (result < 0) {
                  printf("Error! socket.send() returned: %d\n", result);
                  //goto DISCONNECT;
              }
          //}
          _event_queue.call(this, &GapScanner::socket_thread);
        }
        else if( my_strcmp(p, p_save, rcount) ) {
          printf("my_strcmp --> save\n");
          _event_queue.call(this, &GapScanner::scan);
        }

      }
      printf("socket_thread : done\r\n");
    }

    bool my_strcmp(const char *p1, const char *p2, int _size) {
      for(int i=0; i<_size; i++) {
        if( *(p1+i) != *(p2+i) )
          return false;
      }
      return true;
    }

private:
    /* Gap::EventHandler */

    /** Look at scan payload to find a peer device and connect to it */
    virtual void onAdvertisingReport(const ble::AdvertisingReportEvent &event) {
        /* keep track of scan events for performance reporting */
        _scan_count++;

        const uint8_t* addr = event.getPeerAddress().data();

        printf("Found a device at %d, mac_address : %02X:%02X:%02X:%02X:%02X:%02X\n", event.getRssi(), addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
        

        if( (addr[0] == 252 && addr[5] == 117) ) {
          printf("%02X:%02X:%02X:%02X:%02X:%02X : ",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
          printf("%d:%d:%d:%d:%d:%d\r\n",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

            char buff[64];
             sprintf(buff, "%d", event.getRssi());
             //socket.send(buff, sizeof(buff));
        }
    }

    virtual void onScanTimeout(const ble::ScanTimeoutEvent&) {
        printf("Stopped scanning early due to timeout parameter\r\n");
        char buff[64];
        sprintf(buff, "NOT");
        socket.send(buff, 3);
        _event_queue.call_in(1000, this, &GapScanner::socket_thread);
    }    

private:
    BLE                &_ble;
    ble::Gap           &_gap;
    EventQueue         &_event_queue;
    DigitalOut          _led1;

    /* Keep track of our progress through demo modes */
    size_t              _set_index;

    /* Measure performance of our advertising/scanning */
    size_t              _scan_count;

    int                 _blink_event;
    //static GapScanner *myGapScanner;
    
    const uint8_t surachai[6] = {44, 253, 161, 71, 54, 103};
    const uint8_t iot_03[6] = {252, 153, 199, 111, 10, 117}; // FC:99:C7:6F:0A:75
    
    // Network interface
    NetworkInterface *net;
    SocketAddress a;
    // Open a socket on the network interface, and create a TCP connection to ifconfig.io
    TCPSocket socket;
    
public:
  bool b_scan, b_finish, b_ble_init;
  int PORT;

  int remaining;
  int rcount;
  char *p;
  char buffer[256];
  nsapi_size_or_error_t result;

  char *p_id, *p_save;
  char txt_id[4] = {'#', 'i', 'd', '$'};
  char txt_save[4] = {'s', 'a', 'v', 'e'};
  char txt_name[7] = {'t', 'o', 'a', '_', 'i', 'o', 't'};

};

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

bool my_strcmp(const char *p1, const char *p2, int _size);
void socket_thread();

// Socket demo
int main() {
    // Bring up the ethernet interface
    printf("Mbed OS Socket example\n");

#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

    BLE &ble = BLE::Instance();
    /* this will inform us off all events so we can schedule their handling
     * using our event queue */
    ble.onEventsToProcess(schedule_ble_events);
    GapScanner scanner(ble, event_queue);
    //thread.start(callback(&scanner, &GapScanner::run));
    scanner.run();

    //thread.start(socket_thread);
}


