#include <mbed.h>
#include <MQTTClientMbedOs.h>
#include <vector>
#include <string>

#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

struct Names {
  enum type { toa, bank, inn, o, por, menghorng, michael, wari, aoff, test};
};

int8_t name = Names::toa;

int main() {

    DigitalOut led(LED1), led2(LED2);
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

    std::vector<std::string> MQTT_TOPIC = { "@msg/sensor_data/toa",
                                            "@msg/sensor_data/bank",
                                            "@msg/sensor_data/inn",
                                            "@msg/sensor_data/o",
                                            "@msg/sensor_data/por",
                                            "@msg/sensor_data/menghorng",
                                            "@msg/sensor_data/michael",
                                            "@msg/sensor_data/wari",
                                            "@msg/sensor_data/aoff",
                                            "@msg/sensor_data/toa" };

    int8_t device_id = name;
    unsigned long seq = 1;

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
    wifi->gethostbyname("mqtt.netpie.io", &addr);
    addr.set_port(1883);
    socket->connect(addr);
    if (ret != 0) {
        printf("rc from TCP connect is %d\r\n", ret);
        return -1;
    }

    // MQTT connection
    mqttClient = new MQTTClient(socket); 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = (char*)CLIENT_ID[name].c_str();
    data.username.cstring = (char*)NETPIE_TOKEN[name].c_str();
    //data.password.cstring = (char*)NETPIE_SECRET;
    ret = mqttClient->connect(data);
    if (ret != 0) {
        printf("rc from MQTT connect is %d\r\n", ret);
        return -1;
    }
 
    // Start Read sensor
    float sensor_value = 0;
    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};
	
    printf("Sensor init : start\n");
    BSP_GYRO_Init();
    BSP_GYRO_LowPower(0);
    BSP_ACCELERO_Init();
    BSP_ACCELERO_LowPower(0);
    printf("Sensor init : complete\n");
    
    printf("MQTT init : start\n");
    MQTT::Message message;
    char buf[100];
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    printf("MQTT init : complete\n");

    Timer t;
    t.start();
    float l_time[9], previous_t=0.0;
    int32_t rpy[3] = {0};
    t.reset();
    previous_t=t.read();
    while(1) {
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        for(int i=0; i<3; i++)
          rpy[i] = pGyroDataXYZ[i];
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        
        sprintf(buf, "%lu,%d,%d,%d,%d,%d,%d,%d\r\n", seq, device_id, rpy[0], rpy[1], rpy[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;
        if( t.read()-previous_t > 0.5 ) {
          ret = mqttClient->publish(MQTT_TOPIC[name].c_str(), message);
          if (ret != 0)
              printf("rc from publish was %d\r\n", ret);
          seq=seq+1;
          led2=1;
          previous_t = t.read();
        }
        else if( t.read()-previous_t > 0.025 )
          led2=0;
        led = !led;
    }
    t.stop();
}

