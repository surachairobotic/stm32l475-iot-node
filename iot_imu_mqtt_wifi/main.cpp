#include <mbed.h>
#include <MQTTClientMbedOs.h>
#include <vector>
#include <string>

#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

#include "imu.h"

#define MODE 1 // 0 : socket_wifi, 1 : MQTT
//#define TRANING_MODE

DigitalOut led(LED1);
DigitalOut led2(LED2);
DigitalOut led4(LED4); //blue
DigitalOut led3(LED3); //yellow
InterruptIn button(USER_BUTTON);
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Thread t;
Thread t_button;
Thread t_sensor;
IMU* imu;
int status = 1;

//kalmannnnnnnnnnnnnnnnnnnnnnnnnnnn
float est[6] = {500, 500, 500, 500, 500, 500};
float var[6] = {255, 255, 255, 255, 255, 255};
float deviation[6] = {25, 25, 25, 25, 25, 125};
float kalman_gain[6] = {0, 0, 0, 0, 0, 0};
float est_current[6] = {500, 500, 500, 500, 500, 500};
float data_val[12] = {0};

struct Names
{
  enum type
  {
    toa,
    bank,
    inn,
    o,
    por,
    menghorng,
    michael,
    wari,
    aoff,
    test
  };
};

int8_t name = Names::toa;

void pressed_handler();
void read_sensor();


void thread_button()
{
  while (1)
  {
    button.fall(queue.event(pressed_handler));
    ThisThread::sleep_for(10);
  }
}
void thread_sensor()
{
  while (1)
  {
    read_sensor();
    ThisThread::sleep_for(10);
  }
}

int main()
{

  // DigitalOut led(LED1), led2(LED2);
  WiFiInterface *wifi;
  TCPSocket *socket;
  MQTTClient *mqttClient;

  std::vector<std::string> CLIENT_ID = {"72ebe8dc-a5db-4e78-9c51-7c94c0f47e2d",
                                        "4c17795a-e0da-4573-8902-694ec60085a0",
                                        "028f671c-9d71-4dd9-b936-58b12752f366",
                                        "9471259d-d918-4fab-a56d-e3fdf5477bcd",
                                        "d8ade097-71ee-4881-a557-8556b54d2ba1",
                                        "501e7184-6bfd-4f37-81ab-bca74039b0c9",
                                        "3847fd5e-1641-4b13-ba8e-4b647b598c13",
                                        "8958661e-e9e1-4e52-9570-4a9729a3adef",
                                        "4665fab9-4827-40de-a1a6-36e538463bc4",
                                        "d3e15b73-a2dc-4940-b7a1-568a235b62e6"};

  std::vector<std::string> NETPIE_TOKEN = {"JUn91WGTt7JN9f2pS6mkxXriyJusR2eL",
                                           "xuXYkC71Ndv7XosKE8DLL5PbHaRAFwLn",
                                           "TTXbbKgY1PFwdhYCvLbtYyGM3X6wfxDY",
                                           "Q3CEkL7L1Rua3eKtCSEqUhMk9z4c5v3e",
                                           "iBukVabEyN6o5kzcrcrX8Fc7caRjcWB3",
                                           "axksa5eghhCQYp9EADGtr7edpL2Ev3G1",
                                           "Bh6gKiQ7vr8Cbxp5KVZ3VVGoSL95pXyB",
                                           "myZjvyCNdfpa41dvTX8fSk9niQeKZbVz",
                                           "CXhbMLgUwHFZWKdt77AHEVAgio42f3k7",
                                           "BJpbTPMwu8JFtrtogueNuhc2iDuKmN5U"};

#ifdef TRANING_MODE
  std::vector<std::string> MQTT_TOPIC = {"@msg/sensor_data/1",
                                         "@msg/sensor_data/2",
                                         "@msg/sensor_data/3",
                                         "@msg/sensor_data/4",
                                         "@msg/sensor_data/5",
                                         "@msg/sensor_data/6",
                                         "@msg/sensor_data/7",
                                         "@msg/sensor_data/8",
                                         "@msg/sensor_data/9",
                                         "@msg/sensor_data/1"};
#else
  std::vector<std::string> MQTT_TOPIC = {"@msg/predict_data/1",
                                         "@msg/predict_data/2",
                                         "@msg/predict_data/3",
                                         "@msg/predict_data/4",
                                         "@msg/predict_data/5",
                                         "@msg/predict_data/6",
                                         "@msg/predict_data/7",
                                         "@msg/predict_data/8",
                                         "@msg/predict_data/9",
                                         "@msg/predict_data/1"};
#endif

  int8_t device_id = name;
  unsigned long seq = 1;

  // WiFi connection
  wifi = WiFiInterface::get_default_instance();
  if (!wifi)
  {
    printf("ERROR: No WiFiInterface found.\n");
    return -1;
  }
  int16_t ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
  if (ret != 0)
  {
    printf("\nConnection error: %d\n", ret);
    return -1;
  }

  // Socket connection
  socket = new TCPSocket();
  socket->open(wifi);
  SocketAddress addr;
  if (MODE)
  {
    wifi->gethostbyname("mqtt.netpie.io", &addr);
    addr.set_port(1883);
  }
  else
  {
    wifi->gethostbyname("192.168.43.66", &addr);
    addr.set_port(1111);
  }
  socket->connect(addr);
  if (ret != 0)
  {
    printf("rc from TCP connect is %d\r\n", ret);
    return -1;
  }

  if (MODE)
  {
    // MQTT connection
    mqttClient = new MQTTClient(socket);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = (char*)CLIENT_ID[name].c_str();
    data.username.cstring = (char*)NETPIE_TOKEN[name].c_str();
    //data.password.cstring = (char*)NETPIE_SECRET;
    ret = mqttClient->connect(data);
    if (ret != 0)
    {
      printf("rc from MQTT connect is %d\r\n", ret);
      return -1;
    }
  }

  printf("Sensor init : start\n");
  imu = new IMU();
  printf("Sensor init : complete\n");

  t.start(callback(&queue, &EventQueue::dispatch_forever));
  t_button.start(&thread_button);
  t_sensor.start(&thread_sensor);

  char buf[100];
  MQTT::Message message;
  if (MODE)
  {
    printf("MQTT init : start\n");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    printf("MQTT init : complete\n");
  }

  Timer t;
  t.start();
  float previous_t = 0.0;
  t.reset();
  previous_t = t.read();

  bool b_connect = false;
  float freq = 0.25;

  while(1) {
    printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n", data_val[0], data_val[1], data_val[2], data_val[3], data_val[4], data_val[5], data_val[6], data_val[7], data_val[8], data_val[9], data_val[10], data_val[11]);

    if (t.read() - previous_t > freq && MODE) {
#ifdef TRANING_MODE
      sprintf(buf, "%d,%d,%d,%d,%d,%d,%d", (int)data_val[0], (int)data_val[1], (int)data_val[2], (int)data_val[3], (int)data_val[4], (int)data_val[5], status);
#else
      sprintf(buf, "%d,%d,%d,%d,%d,%d", (int)data_val[0], (int)data_val[1], (int)data_val[2], (int)data_val[3], (int)data_val[4], (int)data_val[5]);
#endif

      message.payload = (void *)buf;
      message.payloadlen = strlen(buf) + 1;

      ret = mqttClient->publish(MQTT_TOPIC[name].c_str(), message);
      if (ret != 0)
        printf("rc from publish was %d\r\n", ret);
      previous_t = t.read();

      imu->rebase_kalman();
    }
    else if (t.read() - previous_t > freq / 2.0) {
      led2 = 0;
    }
    led = !led;
  }
  t.stop();
  printf("stop\r\n");
}

void pressed_handler()
{
  if (status == 1)
  {
    printf("Status : idle");
    status = 0;
    led4 = 1; //blue
    led3 = 0; //yellow
  }
  else if (status == 0)
  {
    printf("Status : walk");
    status = 1;
    led4 = 0; //blue
    led3 = 1; //yellow
  }
}

void read_sensor()
{
  imu->get_sensor_kalman(data_val);
}
