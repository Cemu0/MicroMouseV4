#ifdef HARDWARE_TEST

#include <Arduino.h>
#include "pinDefine.h"
#include "IRsensor.h"
#include "ESP32MotorControl.h" 
#include <ESP32Encoder.h>
#include "I2Cdev.h"
// #include "MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>

ESP32MotorControl MotorControl = ESP32MotorControl();
ESP32Encoder encoderA(true);
ESP32Encoder encoderB(true);

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

#define OUTPUT_READABLE_YAWPITCHROLL

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void ICACHE_RAM_ATTR dmpDataReady() {
    mpuInterrupt = true;
}

//https://github.com/espressif/arduino-esp32/issues/1720
void tone(byte pin, int freq) {
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone
}

void setup()
{
    pinMode(12,OUTPUT);
    tone(12,1000);
    Serial.begin(115200);
    Serial.println("------- testing --------");
    pinMode(2,OUTPUT);
    delay(100);
    tone(12,0);

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
    #endif

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
    }

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    //setup all input pins 
    ESP32Encoder::useInternalWeakPullResistors=UP;
    encoderA.attachFullQuad(MA_EC1, MA_EC2);
    encoderB.attachFullQuad(MB_EC1, MB_EC2);
    MotorControl.attachMotors(BIN1_PIN, BIN2_PIN, AIN1_PIN, AIN2_PIN);
    MotorControl.motorsStop();
    initIR();

    Serial.println("------- encoder motor test --------");

    // To control the motors
    MotorControl.motorForward(0, 50);
    // MotorControl.motorReverse(0, 50);
    MotorControl.motorForward(1, 50);
    // MotorControl.motorReverse(1, 50);

    if(encoderA.getCount() != 0){
      Serial.println("encoder A OK");
    }else{
      Serial.println("encoder A FAIL !!!");
    }

    if(encoderB.getCount() != 0){
      Serial.println("encoder B OK");
    }else{
      Serial.println("encoder B FAIL !!!");
    }

    delay(500);
    MotorControl.motorForward(0, 100);
    MotorControl.motorReverse(0, 100);
    MotorControl.motorForward(1, 100);
    MotorControl.motorReverse(1, 100);
    delay(500);

    MotorControl.motorsStop();

    Serial.println("------- SENSOR test --------");
    for(int i  = 0; i < 3 ;++i){

      int thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1;
      int thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2;
      int thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
      int thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
      int thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;

      //GATE VALUE
      digitalWrite(SENSOR_ACTIVATE_GROUP_A,HIGH);
      Serial.print("NO LED");
      Serial.print(thread_value1);
      Serial.print(" ");
      Serial.print(thread_value2);
      Serial.print(" ");
      Serial.print(thread_value3);
      Serial.print(" ");
      Serial.print(thread_value4);
      Serial.print(" ");
      Serial.println(thread_value5);

      thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1;
      thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2;
      thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
      thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
      thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;
      digitalWrite(SENSOR_ACTIVATE_GROUP_A,LOW);

      digitalWrite(SENSOR_ACTIVATE_GROUP_B,HIGH);
      Serial.print("LED  A");
      Serial.print(thread_value1);
      Serial.print(" ");
      Serial.print(thread_value2);
      Serial.print(" ");
      Serial.print(thread_value3);
      Serial.print(" ");
      Serial.print(thread_value4);
      Serial.print(" ");
      Serial.println(thread_value5);

      thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1;
      thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2;
      thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
      thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
      thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;
      digitalWrite(SENSOR_ACTIVATE_GROUP_B,LOW);

      digitalWrite(SENSOR_ACTIVATE_GROUP_C,HIGH);
      Serial.print("LED  B");
      Serial.print(thread_value1);
      Serial.print(" ");
      Serial.print(thread_value2);
      Serial.print(" ");
      Serial.print(thread_value3);
      Serial.print(" ");
      Serial.print(thread_value4);
      Serial.print(" ");
      Serial.println(thread_value5);

      thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1;
      thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2;
      thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
      thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
      thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;
      digitalWrite(SENSOR_ACTIVATE_GROUP_C,LOW);

      Serial.print("LED  C");
      Serial.print(thread_value1);
      Serial.print(" ");
      Serial.print(thread_value2);
      Serial.print(" ");
      Serial.print(thread_value3);
      Serial.print(" ");
      Serial.print(thread_value4);
      Serial.print(" ");
      Serial.println(thread_value5);
      delay(1000);
    }
}
int lastea = 0;
int lasteb = 0;
void loop(){

    if(millis() % 1000 == 0){
      readIRsensor();

      printIR();
    }
    // digitalWrite(2,HIGH);
    // delay(500);
    // digitalWrite(2,LOW);
    // delay(500);
    if(lastea != encoderA.getCount() || lasteb != encoderB.getCount()){
      Serial.print("A ");
      Serial.print(encoderA.getCount());
      Serial.print(" B ");
      Serial.println(encoderB.getCount());
      lastea = encoderA.getCount();
      lasteb = encoderB.getCount();
    }

    // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize) return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      Serial.print("ypr\t");
      Serial.print(ypr[0] * 180/M_PI);
      Serial.print("\t");
      Serial.print(ypr[1] * 180/M_PI);
      Serial.print("\t");
      Serial.println(ypr[2] * 180/M_PI);
  }


}

#endif