#include "Arduino.h"
#include "I2Cdev.h"
#include "pinDefine.h"

#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>


// MPU control/status vars
MPU6050 mpu;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];   
float delta;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void ICACHE_RAM_ATTR dmpDataReady() {
    mpuInterrupt = true;
}

uint8_t setupMPU(){
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    mpu.initialize();
    if (!mpu.testConnection())
        return false;
    devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();

        //using the calibrated value

        // mpu.setXAccelOffset(-318); 
        // mpu.setYAccelOffset(1365); 
        // mpu.setZAccelOffset(1822); 
        // mpu.setXGyroOffset(46);
        // mpu.setYGyroOffset(-15);
        // mpu.setZGyroOffset(12);

        // enable this to calibrate
        mpu.CalibrateAccel(10);
        mpu.CalibrateGyro(12);
        // enable this to print the calibrated value
        mpu.PrintActiveOffsets();

        return true;
    }
}

float offsetYaw(float offset){
    delta = ypr[0] - offset;
    if(offset >= 0){
        // if(delta <= -180.0)
        //     return 360.0 + delta;
        if(delta <= -PI)
            return PI*2.0 + delta;
        else    
            return delta;
    }
    else{
        // if(delta >= 180.0)
        if(delta >= PI)
            return  ypr[0] + offset;
        else
            return delta;
    }
}

void readGyro(){
    if (!dmpReady) return;
    if (!mpuInterrupt && fifoCount < packetSize) return;
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();
    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        TelnetStream.println("over flow !!!");

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        // Serial.print("ypr\t");
        // Serial.print(ypr[0] * 180/M_PI);
        // Serial.print("\t");
        // Serial.print(ypr[1] * 180/M_PI);
        // Serial.print("\t");
        // Serial.println(ypr[2] * 180/M_PI);
    }
}