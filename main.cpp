#include "mbed.h"
#include "rtos.h"
#include "FreeIMU.h"

Serial pc(USBTX, USBRX); // tx, rx

typedef struct {
  float ypr[3];
  float alt;
  float temp;
  int time;
} imu_t;

FreeIMU imu;
Mail<imu_t, 4> imu_queue;
int i = 0;
Timer t,t2;

void getIMUdata(void const *n){
    i++;
    if (i%10==0){
        float *q = (float *)imu_queue.alloc();
        t.reset();
        t.start();
        imu.getYawPitchRoll(q);
        t.stop();
        ((imu_t*)q)->time = t.read_us();
        ((imu_t*)q)->alt = imu.getBaroAlt();
        ((imu_t*)q)->temp = imu.baro->getTemperature();
        imu_queue.put((imu_t*)q);
    }else{
        imu.getQ(NULL);
    }
}

int main() {
    imu.init(true);
    pc.baud(115200);
    RtosTimer IMUTimer(getIMUdata, osTimerPeriodic, (void *)0);
    IMUTimer.start(2);
    t2.start();
    
    while (true) {
        t2.reset();
        osEvent evt = imu_queue.get();
        if (evt.status == osEventMail) {
            imu_t *obj = (imu_t*)evt.value.p;
            pc.printf("Y: %f, P: %f, R: %f, Alt: %f, Temp: %fC\r\n", obj->ypr[0], obj->ypr[1], obj->ypr[2], obj->alt, obj->temp);
            imu_queue.free(obj);
            pc.printf("\r\n");
        }
    }
}
