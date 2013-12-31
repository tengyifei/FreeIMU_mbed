/*
MS5611-01BA.cpp - Interfaces a Measurement Specialities MS5611-01BA with Arduino
See http://www.meas-spec.com/downloads/MS5611-01BA01.pdf for the device datasheet

Copyright (C) 2011 Fabio Varesano <fvaresano@yahoo.it>

Development of this code has been supported by the Department of Computer Science,
Universita' degli Studi di Torino, Italy within the Piemonte Project
http://www.piemonte.di.unito.it/


This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mbed.h"
#include "rtos.h"
#include "MODI2C.h"
#include "MS561101BA.h"

#ifndef I2C_SDA
    #define I2C_SDA p28
    #define I2C_SCL p27
#endif

MS561101BA::MS561101BA():i2c(I2C_SDA,I2C_SCL),_thread(&MS561101BA::samplingthread_stub, this),sem(0){
    zero = 0;
    MS561101BA_RESET = 0x1E;
    _OSR = NULL;
}

void MS561101BA::init(uint8_t address) {  
    lastPresConv=0;
    lastTempConv=0;
  t.start();
  _addr =  address << 1;
 
  reset(); // reset the device to populate its internal PROM registers
  Thread::wait(500); // some safety time
  readPROM(); // reads the PROM into object variables for later use
}

void MS561101BA::samplingthread_stub(void const *p) {
  MS561101BA *instance = (MS561101BA*)p;
  instance->samplingthread();
}

float MS561101BA::getPressure() {
  // see datasheet page 7 for formulas
  
  if(pressCache == NULL) {
    return NULL;
  }
  
  int32_t dT = getDeltaTemp();
  if(dT == NULL) {
    return NULL;
  }
  
  int64_t off  = ((uint32_t)_Cal[1] <<16) + (((int64_t)dT * _Cal[3]) >> 7);
  int64_t sens = ((uint32_t)_Cal[0] <<15) + (((int64_t)dT * _Cal[2]) >> 8);
  return ((( (pressCache * sens ) >> 21) - off) >> 15) / 100.0;
}

float MS561101BA::getTemperature() {
  // see datasheet page 7 for formulas
  int64_t dT = getDeltaTemp();
  
  if(dT != NULL) {
    return (2000 + ((dT * _Cal[5]) >> 23)) / 100.0;
  }
  else {
    return NULL;
  }
}

int32_t MS561101BA::getDeltaTemp() {
  if(tempCache != NULL) {
    return (int32_t)(tempCache - ((uint32_t)_Cal[4] << 8));
  }
  else {
    return NULL;
  }
}

void MS561101BA::samplingthread(){
    Thread::signal_wait(0x1);
    for (;;){
        char command = MS561101BA_D1 + _OSR;
        startConversion(&command);
        Thread::wait(13);
        getConversion();
        sem.wait();
        pressCache = conversion;
        command = MS561101BA_D2 + _OSR;
        startConversion(&command);
        Thread::wait(13);
        getConversion();
        sem.wait();
        tempCache = conversion;
        Thread::yield();
    }
}

void MS561101BA::start_sampling(uint8_t OSR){
    _OSR = OSR;
    _thread.signal_set(0x1);
}

int MS561101BA::rawTemperature(){
    return tempCache;
}

int MS561101BA::rawPressure(){
    return pressCache;
}

// see page 11 of the datasheet
void MS561101BA::startConversion(char *command) {
  // initialize pressure conversion
  i2c.write(_addr, (char*)command, 1);
}

uint32_t getConversion_fin(uint32_t param){
    MS561101BA* ins = (MS561101BA*)param;
    ins->conversion = (ins->cobuf[0] << 16) + (ins->cobuf[1] << 8) + ins->cobuf[2];
    ins->sem.release();
    return 0;
}

void MS561101BA::getConversion() {
  i2c.write(_addr, (char*)&zero, 1);
  i2c.read_nb(_addr, (char*)cobuf, MS561101BA_D1D2_SIZE, &getConversion_fin, this);
}

/**
 * Reads factory calibration and store it into object variables.
*/
int MS561101BA::readPROM() {
  for (int i=0;i<MS561101BA_PROM_REG_COUNT;i++) {
    char a = MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE);
    i2c.write(_addr, &a, 1);
    
    char tmp[2];
    if (i2c.read(_addr, tmp, MS561101BA_PROM_REG_SIZE)!=0) return -1;
    _Cal[i] = tmp[0] <<8 | tmp[1];
    Thread::wait(200);
  }
  return 0;
}


/**
 * Send a reset command to the device. With the reset command the device
 * populates its internal registers with the values read from the PROM.
*/
void MS561101BA::reset() {
  i2c.write(_addr, (char*)&MS561101BA_RESET, 1);
}
