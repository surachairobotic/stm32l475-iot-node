#include <mbed.h>
#include <MQTTClientMbedOs.h>

#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
WiFiInterface *wifi;
TCPSocket* socket;
MQTTClient* mqttClient;
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);

const char CLIENT_ID[] = "22b356db-9099-4922-8e10-60bae9346d47";
const char NETPIE_TOKEN[] = "frDwbf2zZHZtj9Pq9KkiQek1w841zD8X"; 
const char NETPIE_SECRET[] = "e.bf2$(tjjMNja@$HKXr+#imN3&+TxA3";
const char MQTT_TOPIC[] = "@msg/taist2020_inn/imu/1";

int device_id = 1;

// void pressed_handler() {
//     static int count = 0;
//     int ret;
//     MQTT::Message message;
 
//     // QoS 0
//     char buf[100];
//     sprintf(buf, "{\"count\":%d}", ++count);
//     message.qos = MQTT::QOS0;
//     message.retained = false;
//     message.dup = false;
//     message.payload = (void*)buf;
//     message.payloadlen = strlen(buf)+1;
//     printf("Sending MQTT message\n");
//     ret = mqttClient->publish(MQTT_TOPIC, message);
//     if (ret != 0) {
//         printf("rc from publish was %d\r\n", ret);
//         return;
//     }    
// }

// void imu_read_mqtt() {
//     // static int count = 0;

//     BSP_GYRO_GetXYZ(pGyroDataXYZ);
//     ThisThread::sleep_for(50);
//     BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    
//     printf("GYRO[%f, %f, %f], ACC[%d, %d, %d]\r\n", pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);


//     int ret;
//     MQTT::Message message;
 
//     // QoS 0
//     char buf[100];
//     // sprintf(buf, "{\"count\":%d}", ++count);
//     sprintf(buf, "GYRO[%f, %f, %f], ACC[%d, %d, %d]\r\n", pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
//     message.qos = MQTT::QOS0;
//     message.retained = false;
//     message.dup = false;
//     message.payload = (void*)buf;
//     message.payloadlen = strlen(buf)+1;
//     printf("Sending MQTT message\n");
//     ret = mqttClient->publish(MQTT_TOPIC, message);
//     if (ret != 0) {
//         printf("rc from publish was %d\r\n", ret);
//         return;
//     }    
// }

int main() { 
    // WiFi connection
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
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
    //data.MQTTVersion = 3;
    data.clientID.cstring = (char*)CLIENT_ID;
    data.username.cstring = (char*)NETPIE_TOKEN;
    //data.password.cstring = (char*)NETPIE_SECRET;
    ret = mqttClient->connect(data);
    if (ret != 0) {
        printf("rc from MQTT connect is %d\r\n", ret);
        return -1;
    }
 
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    // button.fall(queue.event(pressed_handler));

    printf("Starting\n");

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

    while(1) {

        // imu_read_mqtt()

        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        ThisThread::sleep_for(50);
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        
        printf("DEVICE[%d],GYRO[%f, %f, %f], ACC[%d, %d, %d]\r\n", device_id, pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);


        int ret;
        MQTT::Message message;
    
        // QoS 0
        char buf[100];
        // sprintf(buf, "{\"count\":%d}", ++count);
        sprintf(buf, "DEVICE[%d],GYRO[%f, %f, %f], ACC[%d, %d, %d]\r\n", device_id, pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2], pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;
        printf("Sending MQTT message\r\n");
        ret = mqttClient->publish(MQTT_TOPIC, message);
        if (ret != 0) //{
            printf("rc from publish was %d\r\n", ret);
        //     return;
        // } 

        led = !led;
        ThisThread::sleep_for(500);
    }
}

