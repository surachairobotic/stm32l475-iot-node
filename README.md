# stm32l475-iot-node
This repository consists of three source code
1. push-to-light-baremetal
2. push-to-light-thread
3. push-to-light-eventqueue

# How to compile the source code
1. Open Git bash
1. Select your preferred folder
```console
	$ mkdir [PREFERRED_FOLDER_NAME]
```
2. Clone Repository from Github
```console
	$ git clone https://github.com/surachairobotic/stm32l475-iot-node.git [PREFERRED_FOLDER_NAME]
```
2. Setting the Configuration Path
```console
	$ mbed config -G MBED_OS_DIR <YOUR_MBED_OS_DIR>/mbed-os/
```
3. Go to the Repository
```console
	$ cd [PREFERRED_FOLDER_NAME]
```
4. Build each project using the following command
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


