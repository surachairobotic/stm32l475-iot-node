#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
InterruptIn sw(PC_13);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void rise_handler(void) {
    // Toggle LED
    led1 = !led1;
}

void fall_handler(void) {
    // Toggle LED
    led1 = !led1;
}

int main() {
    // Start the event queue
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    // The 'rise' handler will execute in IRQ context
    sw.rise(rise_handler);
    // The 'fall' handler will execute in the context of thread 't'
    sw.fall(queue.event(fall_handler));
}
