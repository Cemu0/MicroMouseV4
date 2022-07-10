#pragma once
#include <Arduino.h>
#include "system_def.h"
#include "driver/adc.h"

// long LED_WARMUP_TIME = 5;
#define LED_WARMUP_TIME 80
#define LED_COOLDOWN_TIME 5

#define SIMPLE_FILTER(n, old) ((n + (old * 2))) / (3)
// #define SIMPLE_FILTER(n, old) (n + old * 0)

#define PREVENT_ZERO(val) val>=0?val:0

//for 1 and 5
long collapseFrontWall = 3500;
long hasFrontWall = 1000;
long hasLeftWall = 1000;
long hasRightWall = 1000;
#define leftMiddleValue 3500
#define rightMiddleValue  3400

long faceSensorValue1 = 0;
long faceSensorValue2 = 0;
long faceSensorValue3 = 0;
long faceSensorValue4 = 0;
long faceSensorValue5 = 0;

// the value that determent when the mouse approach edge
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

    // analogSetAttenuation(ADC_11db);
    // analogSetWidth(10);

    //not need using pin mode

    // pinMode(SENSOR_PIN_1,INPUT);
    // pinMode(SENSOR_PIN_2,INPUT);
    // pinMode(SENSOR_PIN_3,INPUT);
    // pinMode(SENSOR_PIN_4,INPUT);
    // pinMode(SENSOR_PIN_5,INPUT);

    pinMode(SENSOR_ACTIVATE_GROUP_A,OUTPUT);
    pinMode(SENSOR_ACTIVATE_GROUP_B,OUTPUT);
    pinMode(SENSOR_ACTIVATE_GROUP_C,OUTPUT);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A, LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_B, LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_C, LOW);
}

void readIRsensor(bool captureValue = false){
    long thread_value1 = analogRead(SENSOR_PIN_1);// - ThdVal1; adc1_get_raw(ADC1_CHANNEL_0);
    long thread_value2 = analogRead(SENSOR_PIN_2);// - ThdVal2; adc1_get_raw(ADC1_CHANNEL_3);
    long thread_value3 = analogRead(SENSOR_PIN_3);// - ThdVal3;
    long thread_value4 = analogRead(SENSOR_PIN_4);// - ThdVal4;
    long thread_value5 = analogRead(SENSOR_PIN_5);// - ThdVal5;

    // LED_WARMUP_TIME = (millis()%10000)/100;

    // digitalWrite(SENSOR_ACTIVATE_GROUP_B,HIGH);
    // delayMicroseconds(LED_WARMUP_TIME);
    // faceSensorValue2 = thread_value2-analogRead(SENSOR_PIN_2);
    // faceSensorValue4 = thread_value4-analogRead(SENSOR_PIN_4);
    // digitalWrite(SENSOR_ACTIVATE_GROUP_B,LOW);

    digitalWrite(SENSOR_ACTIVATE_GROUP_C,HIGH);
    delayMicroseconds(LED_WARMUP_TIME); // 100uS
    faceSensorValue3 = SIMPLE_FILTER((thread_value3-analogRead(SENSOR_PIN_3)),faceSensorValue3);
    // faceSensorValue3 = ((thread_value3-analogRead(SENSOR_PIN_3)) + faceSensorValue3)/2;

    digitalWrite(SENSOR_ACTIVATE_GROUP_C,LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A,HIGH);
    delayMicroseconds(LED_WARMUP_TIME); // 100uS
    // faceSensorValue5 = SIMPLE_FILTER(PREVENT_ZERO(thread_value5-analogRead(SENSOR_PIN_5)),faceSensorValue5);
    faceSensorValue5 = SIMPLE_FILTER((thread_value5-analogRead(SENSOR_PIN_5)),faceSensorValue5);
    // faceSensorValue1 = SIMPLE_FILTER(PREVENT_ZERO(thread_value1-analogRead(SENSOR_PIN_1)),faceSensorValue1);
    faceSensorValue1 = SIMPLE_FILTER((thread_value1-analogRead(SENSOR_PIN_1)),faceSensorValue1);
    digitalWrite(SENSOR_ACTIVATE_GROUP_A,LOW);

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


    // digitalWrite(SENSOR_ACTIVATE_GROUP_A,LOW);
    digitalWrite(SENSOR_ACTIVATE_GROUP_B,LOW);
    delayMicroseconds(LED_WARMUP_TIME); // too much time !

    // digitalWrite(SENSOR_ACTIVATE_GROUP_C,LOW);

    if(faceSensorValue3 < collapseFrontWall)
        frontWall = faceSensorValue3 > hasFrontWall;
    else 
        frontWall = -1;

    if(captureValue){
        leftWall = faceSensorValue2 > hasLeftWall;
        rightWall = faceSensorValue4 > hasRightWall;
    }



    
}

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