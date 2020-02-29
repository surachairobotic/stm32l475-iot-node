# stm32l475-iot-node
# 24 Feb Assignment
This repository consists of three source code
1. push-to-light-baremetal
2. push-to-light-thread
3. push-to-light-eventqueue

# How to compile the source code
1. Open Git bash
2. Select your preferred folder
```console
	$ mkdir [PREFERRED_FOLDER_NAME]
```
3. Clone Repository from Github
```console
	$ git clone https://github.com/surachairobotic/stm32l475-iot-node.git [PREFERRED_FOLDER_NAME]
```
3. Setting the Configuration Path
```console
	$ mbed config -G MBED_OS_DIR <YOUR_MBED_OS_DIR>/mbed-os/
```
4. Go to the Repository
```console
	$ cd [PREFERRED_FOLDER_NAME]
```
5. Build each project using the following command
```console
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-baremetal --source <YOUR_MBED_OS_DIR>/mbed-os --build BUILD/push-to-light-baremetal
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-eventqueue --source <YOUR_MBED_OS_DIR>/mbed-os --build BUILD/push-to-light-eventqueue
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-thread --source <YOUR_MBED_OS_DIR>/mbed-os --build BUILD/push-to-light-thread
```

# Write the file to STM32L475
1. Connect the board to computer via USB port.
2. Open BUILD folder in your repository
3. Copy file ~.bin to STM32L475 folder.
4. Press the blue button on the board. The LED light should blink according to the code in main.cpp .

# 26 Feb Assignment
This repository consists of three source code
1. push-to-http
2. push-to-https
3. push-to-mqtt

# How to compile the source code and write the file to STM32L475
1. Open Git bash
2. Select your preferred folder and create new folder program
```console
	$ mbed new --program push-to-http
	$ mbed new --program push-to-https
	$ mbed new --program push-to-mqtt
```
3. Clone Repository from Github
```console
	$ mbed add https://github.com/ARMmbed/wifi-ism43362
	$ mbed add -vv https://os.mbed.com/teams/sandbox/code/mbed-http/
	$ mbed add add https://github.com/ARMmbed/mbed-mqtt
```
4. Copy main.cpp and mbed_app.json from each source folder and save in to each folder program in your computer.
5. Go to the Repository
```console
	$ cd [PREFERRED_FOLDER_NAME]
```
6. Build each project using the following command and write the file to STM32L475
```console
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-http --source <YOUR_MBED_OS_DIR>/mbed-os --source <YOUR_PREFERRED_FOLDER_NAME>\wifi-ism43362  --source <YOUR_PREFERRED_FOLDER_NAME>\mbed-http --build BUILD/push-to-light-baremetal --flash
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-https --source <YOUR_MBED_OS_DIR>/mbed-os --source <YOUR_PREFERRED_FOLDER_NAME>\wifi-ism43362 --source <YOUR_PREFERRED_FOLDER_NAME>\mbed-http --build BUILD/push-to-light-eventqueue --flash
	$ mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM --source push-to-light-mqtt --source <YOUR_MBED_OS_DIR>/mbed-os --source <YOUR_PREFERRED_FOLDER_NAME>\wifi-ism43362 --source <YOUR_PREFERRED_FOLDER_NAME>\mbed-http --source <YOUR_PREFERRED_FOLDER_NAME>\mbed-mqtt --build BUILD/push-to-light-thread --flash
``` 
7. Press the blue button on the board and see the result in YAT program terminal.