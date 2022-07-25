#pragma once
#include <Arduino.h>
#include "system_def.h"
#include "driver/adc.h"

long LED_WARMUP_TIME = 50;
// #define LED_WARMUP_TIME 100
#define LED_COOLDOWN_TIME 5

#define SIMPLE_FILTER(n, old) ((n + (old * 4.0F))) / (5.0F)
// #define SIMPLE_FILTER(n, old) (n + old * 0)

//value to determent haves wall or not
// #define HAVE_WALL_COUNTER 5


#define PREVENT_ZERO(val) val>=0?val:0

//for 1 and 5
float collapseFrontWall = 4090;
float hasFrontWall = 180;
float hasLeftWall = 500;
// long hasRightWall = 800; //hmm true but not in case mouse not alight center
float hasRightWall = 400;
// #define leftMiddleValue 3500
// #define rightMiddleValue  3400

float faceSensorValue1 = 0;
float faceSensorValue2 = 0;
float faceSensorValue3 = 0;
float faceSensorValue4 = 0;
float faceSensorValue5 = 0;

// the value that determent when the mouse approach edge
int frontWallCounter,leftWallCounter,rightWallCounter;
int frontWall,leftWall,rightWall;

// #define analogReads1 adc1_get_raw(ADC1_CHANNEL_0)
// #define analogReads2 adc1_get_raw(ADC1_CHANNEL_3)
void initIR(){
    // gpio_config_t config;
	// config.pin_bit_mask = ((1ULL<<SENSOR_PIN_1) | (1ULL<<SENSOR_PIN_2)) ;
	// config.mode = GPIO_MODE_DISABLE;
	// config.pull_up_en = GPIO_PULLUP_DISABLE;
	// config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	// config.intr_type = GPIO_INTR_DISABLE;
	// gpio_config(&config);
    adcAttachPin(SENSOR_PIN_1);
    adcAttachPin(SENSOR_PIN_2);
    adcAttachPin(SENSOR_PIN_3);
    adcAttachPin(SENSOR_PIN_4);
    adcAttachPin(SENSOR_PIN_5);

    analogReadResolution(12); //12 bits
    // analogSetAttenuation(ADC_11db);
    analogSetClockDiv(16);
    // analogSetWidth(10);


    //not need using pin mode

    // pinMode(SENSOR_PIN_1,INPUT_PULLDOWN);
    // pinMode(SENSOR_PIN_2,INPUT_PULLDOWN);
    // pinMode(SENSOR_PIN_3,INPUT_PULLDOWN);
    // pinMode(SENSOR_PIN_4,INPUT_PULLDOWN);
    // pinMode(SENSOR_PIN_5,INPUT_PULLDOWN);

    pinMode(SENSOR_ACTIVATE_GROUP_A,OUTPUT);
    pinMode(SENSOR_ACTIVATE_GROUP_B,OUTPUT);
    pinMode(SENSOR_ACTIVATE_GROUP_C,OUTPUT);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A, LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_B, LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_C, LOW);
}

void readIRsensor(){
    float thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1; adc1_get_raw(ADC1_CHANNEL_0);
    float thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2; adc1_get_raw(ADC1_CHANNEL_3);
    float thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
    float thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
    float thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;

    // LED_WARMUP_TIME = (millis()%10000)/100;

    // digitalWrite(SENSOR_ACTIVATE_GROUP_B,HIGH);
    // delayMicroseconds(LED_WARMUP_TIME);
    // faceSensorValue2 = thread_value2-analogRead(SENSOR_PIN_2);
    // faceSensorValue4 = thread_value4-analogRead(SENSOR_PIN_4);
    // digitalWrite(SENSOR_ACTIVATE_GROUP_B,LOW);




    //damn the internal capacitor between sensor 1 and sensor 2 require at least 3 millisecond to cooldown => using 1 or 2 only 
    // delay(3);
    // delayMicroseconds(LED_WARMUP_TIME);
    // faceSensorValue1 = (thread_value1-analogRead(SENSOR_PIN_1));

    // digitalWrite(SENSOR_ACTIVATE_GROUP_A,HIGH);
    digitalWrite(SENSOR_ACTIVATE_GROUP_B,HIGH);
    // digitalWrite(SENSOR_ACTIVATE_GROUP_C,HIGH);
    delayMicroseconds(LED_WARMUP_TIME); // too much time !

    // faceSensorValue1 = thread_value1-analogRead(SENSOR_PIN_1);
    // faceSensorValue5 = thread_value5-analogRead(SENSOR_PIN_5);
    // faceSensorValue4 = SIMPLE_FILTER(PREVENT_ZERO(thread_value4-analogRead(SENSOR_PIN_4)),faceSensorValue4);
    faceSensorValue4 = SIMPLE_FILTER((thread_value4-analogRead(SENSOR_PIN_4)), faceSensorValue4);
    // faceSensorValue3 = thread_value3-analogRead(SENSOR_PIN_3);
    // faceSensorValue2 = SIMPLE_FILTER(PREVENT_ZERO(thread_value2-analogRead(SENSOR_PIN_2)),faceSensorValue2);
    faceSensorValue2 = SIMPLE_FILTER((thread_value2-analogRead(SENSOR_PIN_2)),faceSensorValue2);

    digitalWrite(SENSOR_ACTIVATE_GROUP_B,LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A,HIGH);
    delayMicroseconds(LED_WARMUP_TIME); // 100uS
    // faceSensorValue5 = SIMPLE_FILTER(PREVENT_ZERO(thread_value5-analogRead(SENSOR_PIN_5)),faceSensorValue5);
    faceSensorValue5 = SIMPLE_FILTER((thread_value5-analogRead(SENSOR_PIN_5)),faceSensorValue5);
    // faceSensorValue1 = SIMPLE_FILTER(PREVENT_ZERO(thread_value1-analogRead(SENSOR_PIN_1)),faceSensorValue1);
    faceSensorValue1 = SIMPLE_FILTER((thread_value1-analogRead(SENSOR_PIN_1)),faceSensorValue1);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A,LOW);

    // digitalWrite(SENSOR_ACTIVATE_GROUP_A,LOW);
    // delayMicroseconds(LED_WARMUP_TIME); // too much time !
    digitalWrite(SENSOR_ACTIVATE_GROUP_C,HIGH);
    delayMicroseconds(LED_WARMUP_TIME); // 100uS
    faceSensorValue3 = SIMPLE_FILTER((thread_value3-analogRead(SENSOR_PIN_3)),faceSensorValue3);
    // faceSensorValue3 = ((thread_value3-analogRead(SENSOR_PIN_3)) + faceSensorValue3)/2;

    digitalWrite(SENSOR_ACTIVATE_GROUP_C,LOW);
    // digitalWrite(SENSOR_ACTIVATE_GROUP_C,LOW);

    if(faceSensorValue3 < collapseFrontWall)
        frontWall = faceSensorValue3 > hasFrontWall;
    else 
        frontWall = -1;
        

    // leftWall = faceSensorValue2 > hasLeftWall;
    // rightWall = faceSensorValue4 > hasRightWall;

    // leftWall = faceSensorValue1 > 1800;
    // rightWall = faceSensorValue5 > 1900;

    leftWall = faceSensorValue1 > hasLeftWall;
    rightWall = faceSensorValue5 > hasRightWall;

}
/*
  930: 173
 1450: 130
 
*/

// float frontWallDistance(){

// }

void printIR(Stream &stream){
    // stream.print(" ");
    // stream.print(LED_WARMUP_TIME);
    stream.print(" ");
    stream.print(leftWall);
    stream.print("-");
    stream.print(frontWall);
    stream.print("-");
    stream.print(rightWall);
    stream.print(" ");
    stream.print(faceSensorValue2 > hasLeftWall);
    stream.print("-");
    stream.print(faceSensorValue4 > hasRightWall);
    stream.print(" ");
    stream.print(faceSensorValue1);
    stream.print(" ");
    stream.print(faceSensorValue2);
    stream.print(" ");
    stream.print(faceSensorValue3);
    stream.print(" ");
    stream.print(faceSensorValue4);
    stream.print(" ");
    stream.println(faceSensorValue5);
}

void printWall(Stream &stream){
    // stream.print(" ");
    // stream.print(LED_WARMUP_TIME);
    stream.print(" ");
    stream.print(leftWall);
    stream.print("-");
    stream.print(frontWall);
    stream.print("-");
    stream.println(rightWall);
}