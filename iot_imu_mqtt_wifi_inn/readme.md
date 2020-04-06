# iot_imu_mqtt_wifi_version2  
This code have some improvement of iot_imu_mqtt_wifi main code.  
This code can run correctly.  
By the way this code is not verified, please use caution when use this code.  

## How to use  
1. Clone all respitories to your computer folder.  
2. Use this command to compile in command prompt
```
mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source . --source ..\mbed-mqtt --source ..\mbed-os --source ..\wifi-ism43362 --build BUILD
```
3. Type this command in command prompt to see result from sensor
```
python mqtt_dump.py
```
