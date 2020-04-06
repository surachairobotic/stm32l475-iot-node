#include <mbed.h>
#include <MQTTClientMbedOs.h>
#include <vector>
#include <string>

#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

#define MODE 1 // 0 : socket_wifi, 1 : MQTT

DigitalOut led(LED1);
DigitalOut led2(LED2);
DigitalOut led4(LED4); //blue
DigitalOut led3(LED3); //yellow
InterruptIn button(USER_BUTTON);
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Thread t;
Thread t2;
// Thread button_mode;
Thread t_read_sensor;


int status = 1;
int16_t pDataXYZ[3] = {0}; // Start Read sensor
float pGyroDataXYZ[3] = {0}; // Start Read sensor

// Timer t;
// t.start();
float previous_t=0.0;
int32_t rpy[3] = {0};
// t.reset();
// previous_t=t.read();
    
//kalmannnnnnnnnnnnnnnnnnnnnnnnnnnn
float est[6] =         {500,500,500,500,500,500};
float var[6] =         {255,255,255,255,255,255};
float deviation[6] =    {25,25,25,25,25,125};
float kalman_gain[6] =  {0,0,0,0,0,0};
float est_current[6] = {500,500,500,500,500,500};
float temp_val[6] = {0};

bool b_connect = false;
float freq = 0.25;

void pressed_handler();
void read_sensor();

struct Names {
  enum type { toa, bank, inn, o, por, menghorng, michael, wari, aoff, test};
};

int8_t name = Names::toa;

int main() {

    // DigitalOut led(LED1), led2(LED2);
    WiFiInterface *wifi;
    TCPSocket* socket;
    MQTTClient* mqttClient;

    std::vector<std::string> CLIENT_ID = {  "72ebe8dc-a5db-4e78-9c51-7c94c0f47e2d",
                                            "4c17795a-e0da-4573-8902-694ec60085a0",
                                            "028f671c-9d71-4dd9-b936-58b12752f366",
                                            "9471259d-d918-4fab-a56d-e3fdf5477bcd",
                                            "d8ade097-71ee-4881-a557-8556b54d2ba1",
                                            "501e7184-6bfd-4f37-81ab-bca74039b0c9",
                                            "3847fd5e-1641-4b13-ba8e-4b647b598c13",
                                            "8958661e-e9e1-4e52-9570-4a9729a3adef",
                                            "4665fab9-4827-40de-a1a6-36e538463bc4",
                                            "d3e15b73-a2dc-4940-b7a1-568a235b62e6" };

    std::vector<std::string> NETPIE_TOKEN = { "JUn91WGTt7JN9f2pS6mkxXriyJusR2eL",
                                              "xuXYkC71Ndv7XosKE8DLL5PbHaRAFwLn",
                                              "TTXbbKgY1PFwdhYCvLbtYyGM3X6wfxDY",
                                              "Q3CEkL7L1Rua3eKtCSEqUhMk9z4c5v3e",
                                              "iBukVabEyN6o5kzcrcrX8Fc7caRjcWB3",
                                              "axksa5eghhCQYp9EADGtr7edpL2Ev3G1",
                                              "Bh6gKiQ7vr8Cbxp5KVZ3VVGoSL95pXyB",
                                              "myZjvyCNdfpa41dvTX8fSk9niQeKZbVz",
                                              "CXhbMLgUwHFZWKdt77AHEVAgio42f3k7",
                                              "BJpbTPMwu8JFtrtogueNuhc2iDuKmN5U" };

    std::vector<std::string> MQTT_TOPIC = { "@msg/sensor_data/1",
                                            "@msg/sensor_data/2",
                                            "@msg/sensor_data/3",
                                            "@msg/sensor_data/4",
                                            "@msg/sensor_data/5",
                                            "@msg/sensor_data/6",
                                            "@msg/sensor_data/7",
                                            "@msg/sensor_data/8",
                                            "@msg/sensor_data/9",
                                            "@msg/sensor_data/1" };

    int8_t device_id = name;
    unsigned long seq = 1;

    t2.start(callback(&queue, &EventQueue::dispatch_forever)); // toggle switch
    t_read_sensor.start(&read_sensor);

    // WiFi connection
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }
    int16_t ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    
    // Socket connection
    socket = new TCPSocket();
    socket->open(wifi);
    SocketAddress addr;
    if( MODE ) {
      wifi->gethostbyname("mqtt.netpie.io", &addr);
      addr.set_port(1883);
    }
    else {
      wifi->gethostbyname("192.168.43.66", &addr);
      addr.set_port(1111);
    }
    socket->connect(addr);
    if (ret != 0) {
        printf("rc from TCP connect is %d\r\n", ret);
        return -1;
    }

    // MQTT
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = (char*)CLIENT_ID[name].c_str();
    data.username.cstring = (char*)NETPIE_TOKEN[name].c_str();

    if( MODE ) {
      // MQTT connection
      mqttClient = new MQTTClient(socket); 
      //MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
      //data.clientID.cstring = (char*)CLIENT_ID[name].c_str();
      //data.username.cstring = (char*)NETPIE_TOKEN[name].c_str();
      //data.password.cstring = (char*)NETPIE_SECRET;
      ret = mqttClient->connect(data);
      if (ret != 0) {
          printf("rc from MQTT connect is %d\r\n", ret);
          return -1;
      }
    }

    char buf[100];
    MQTT::Message message;
    if( MODE ) {
      printf("MQTT init : start\n");
      message.qos = MQTT::QOS0;
      message.retained = false;
      message.dup = false;
      printf("MQTT init : complete\n");
    }

    printf("Sensor init : start\n");
    BSP_GYRO_Init();
    BSP_GYRO_LowPower(0);
    BSP_ACCELERO_Init();
    BSP_ACCELERO_LowPower(0);
    printf("Sensor init : complete\n");
    
    Timer t;
    t.start();
    float previous_t=0.0;
    int32_t rpy[3] = {0};
    t.reset();
    previous_t=t.read();

    // while (1)
    // {


    // }

    while(1) {
        // thread.start(led2_thread);
        button.fall(queue.event(pressed_handler));
        read_sensor();
        // t_read_sensor.start(&read_sensor);


        // printf("%d,%d,%d,%d,%d,%d,%d\r\n", (int)temp_val[0], (int)temp_val[1], (int)temp_val[2], (int)temp_val[3], (int)temp_val[4], (int)temp_val[5], status); 
        // sprintf(buf, "%d,%d,%d,%d,%d,%d,%d", (int)temp_val[0], (int)temp_val[1], (int)temp_val[2], (int)temp_val[3], (int)temp_val[4], (int)temp_val[5], status);
  
        // printf("%lu,%lu,%lu,%d,%d,%d,%d\r\n", rpy[0], rpy[1], rpy[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], status);
        
        // sprintf(buf, "%lu,%d,%lu,%lu,%lu,%d,%d,%d,%d\r\n", seq, device_id, rpy[0], rpy[1], rpy[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], status);
        // sprintf(buf, "%lu,%lu,%lu,%d,%d,%d,%d\r\n", rpy[0], rpy[1], rpy[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], status);
        if( t.read()-previous_t > freq && MODE ) {
          //printf("%d,%d,%d,%d,%d,%d,%d,%d\r\n", device_id, (int)temp_val[0], (int)temp_val[1], (int)temp_val[2], (int)temp_val[3], (int)temp_val[4], (int)temp_val[5], status); 
          sprintf(buf, "%d,%d,%d,%d,%d,%d,%d", (int)temp_val[0], (int)temp_val[1], (int)temp_val[2], (int)temp_val[3], (int)temp_val[4], (int)temp_val[5], status);

          message.payload = (void*)buf;
          message.payloadlen = strlen(buf)+1;
       
          ret = mqttClient->publish(MQTT_TOPIC[name].c_str(), message);
          if (ret != 0)
              printf("rc from publish was %d\r\n", ret);
          previous_t = t.read();

          led2=1;
          // for(int m=0;m<6;m++)
          // {
          //   est[m] =         500;
          //   var[m] =         255;
          //   deviation[m] =    25;
          //   kalman_gain[m] =  0;
          //   est_current[m] = 500;
          // }
        }
        else if( t.read()-previous_t > freq/2.0 ) {
          led2=0;
        }
        led = !led;
    }
    t.stop();
    printf("stop\r\n");
}

void pressed_handler() {
    if (status == 1){
        printf("Status : idle");
        status = 0;
        led4 = 1; //blue
        led3 = 0; //yellow
    }
    else if (status == 0){
        printf("Status : walk");
        status = 1;
        led4 = 0; //blue
        led3 = 1; //yellow
    }

}


float update_kalman_gain(float var,float deviation)
{
  float kalman_gain=var/(var+deviation);
    return kalman_gain;
}

float update_est_current(float kalman_gain,float measure,float est_last)
{
  float est_current=est_last+kalman_gain*(measure-est_last);
  return est_current;
}

float update_var(float kalman_gain , float var)
{
  var=(1-kalman_gain)*var;
  return var;
}

void read_sensor()
{
    BSP_GYRO_GetXYZ(pGyroDataXYZ);
    for(int i=0; i<3; i++) 
      rpy[i] = pGyroDataXYZ[i];
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);

    temp_val[0]=rpy[0];
    temp_val[1]=rpy[1];
    temp_val[2]=rpy[2];
    temp_val[3]=pDataXYZ[0];
    temp_val[4]=pDataXYZ[1];
    temp_val[5]=pDataXYZ[2];

    for(int j=0;j<6;j++)
    {
      kalman_gain[j]=update_kalman_gain(var[j],deviation[j]);
      est_current[j]=update_est_current(kalman_gain[j],temp_val[j],est_current[j]);
      var[j]=update_var(kalman_gain[j],var[j]);
    }  

    for(int m=0;m<6;m++)
          {
            est[m] =         500;
            var[m] =         255;
            deviation[m] =    25;
            kalman_gain[m] =  0;
            est_current[m] = 500;
          } 
}
