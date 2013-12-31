/*
    Copyright (c) 2011 Andy Kirkham
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
 
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
 
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#ifdef AJK_COMPILE_EXAMPLE1

#include "mbed.h"
#include "FPointer.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

uint32_t myCallback(uint32_t value) {
    // Get the value of the count in main
    // (de-reference it) so we know what it is.
    int i = *((int *)value);
    
    // Then display the bottom four bits of
    // the count value on the LEDs.
    led4 = (i & 1) ? 1 : 0;
    led3 = (i & 2) ? 1 : 0;
    led2 = (i & 4) ? 1 : 0;
    led1 = (i & 8) ? 1 : 0;
    
    // What we return doesn't matter as it's
    // not used in this example but we return
    // "something" (zero in this case) to keep
    // the compiler happy as it expects us to
    // return something.
    return 0;
}

int main() {
    FPointer myPointer;
    int count = 0;
    
    // Attach a C function pointer as the callback.
    myPointer.attach(&myCallback);
    
    while(1) {
        wait(0.5);
        
        // Make the callback passing a pointer
        // to the int count variable.
        myPointer.call((uint32_t)&count);
        
        count++;
    }
}

#endif
