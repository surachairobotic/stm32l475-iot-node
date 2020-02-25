#include "mbed.h"

DigitalOut led(LED1);
//DigitalOut led2(PB_14);
InterruptIn button(USER_BUTTON);

Thread thread_led;
 
int main() {
    while(1)
    {
        while(button==1)
        {
            led = 0; 
        }
        led=1;
    }
    
}
