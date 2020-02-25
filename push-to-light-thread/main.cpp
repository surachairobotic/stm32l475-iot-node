/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
 

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalIn button(PC_13);
Thread thread;

void led2_thread() {
    while (true) {
        if (button == 1){
//			thread.start(led2_thread);
            led1 = 0;
            }
        else{
            led1 = 1;
            }
//        led2 = !led2;
//        wait(1);
    }
}

int main() {
    
    while (true) {
		thread.start(led2_thread);
        led2 = !led2;
        wait(1);
    }		
/*        if (button == 1){
			thread.start(led2_thread);
            led1 = 0;
            }
        else{
            led = 1;
            }*/
        
}

