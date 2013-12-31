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

#ifdef AJK_COMPILE_EXAMPLE3

#include "mbed.h"
#include "FPointer.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

// Create a data structure that matches what you need.
// In this example the structure just holds an int and
// a float. Your own data structure can be constructed
// however you need.
typedef struct {
    int     i_value;
    float   f_value;
} MYDATA;

// The callback function. 
uint32_t myCallback(uint32_t value) {

    // Get the value of the count in main
    // (de-reference it) so we know what it is.
    // Note, Mbed/LPC17xx pointers are 32bit
    // values. So we can "cast" a value back
    // into a pointer.
    MYDATA *q = (MYDATA *)value;
    
    // Then display the bottom four bits of
    // the i_value on the LEDs.
    led4 = (q->i_value & 1) ? 1 : 0;
    led3 = (q->i_value & 2) ? 1 : 0;
    led2 = (q->i_value & 4) ? 1 : 0;
    led1 = (q->i_value & 8) ? 1 : 0;
    
    // Note, f_value isn't used but we could access
    // it's value with q->f_value if we needed to.
    
    // What we return doesn't matter as it's
    // not used in this example but we return
    // "something" (zero in this case) to keep
    // the compiler happy as it expects us to
    // return something.
    return 0;
}

int main() {
    FPointer myPointer;
    MYDATA data;
    
    data.i_value = 0;
    data.f_value = 0.0;
    
    // Attach a C function pointer as the callback.
    myPointer.attach(&myCallback);
    
    while(1) {
        wait(0.5);
        
        // Make the callback passing a pointer (& means "address of")
        // to the data structure "data".
        myPointer.call((uint32_t)&data);
        
        // Increment the i_value by one.
        data.i_value++;
    }
}

#endif
