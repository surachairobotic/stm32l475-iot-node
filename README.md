#Feb 24 assignment
This repository consists of three source code
1. push-to-light-baremetal
2. push-to-light-thread
3. push-to-light-eventqueue

#How to compile the source code
1. Open Git bash
2. Select your preferred folder
```console
	$cd "YOUR_PREFERRED_FOLDER"
```
3. Import repository to your folder
```console
	$mbed import <MBED_OS_DIR>
```
4. Download main.cpp 
5. Open Repository Folder and open main.cpp with your favorite editer.
6. Replace text in main.cpp with main.cpp that you download. 
7. Open Git bash
8. Select REPOSITORY folder
```console
	$cd "REPOSITORY_FOLDER"
```
9. Compile the source code
```console
	$mbed compile -m DISCO_L475VG_IOT01A -t GCC_ARM
```

#Write the file to STM32L475
1. Connect the board to computer via USB port.
2. Open BUILD folder in your repository
3. Copy file ~.bin to STM32L475 folder.
4. Press the blue button on the board. The LED light should blink according to the code in main.cpp .


