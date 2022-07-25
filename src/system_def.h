#pragma once

#define     BOOT_BUTTON     0
#define     LED_PIN         2
#define     BUZZ_PIN         12

//Sensor 
#define     SENSOR_PIN_1      36
#define     SENSOR_PIN_2      39
#define     SENSOR_PIN_3      34
#define     SENSOR_PIN_4      35
#define     SENSOR_PIN_5      32

//BAterry
#define R2 100
#define R3 33
#define     BATTERY_SENSOR      33


//LED 1 5
#define     SENSOR_ACTIVATE_GROUP_A       5
// LED 2 4
#define     SENSOR_ACTIVATE_GROUP_B       4
// LED 3
#define     SENSOR_ACTIVATE_GROUP_C       15

//GyroScope MPU6050
#define     INTERRUPT_PIN       23
#define     SDA_PIN             21
#define     SCL_PIN             22

#define REVERT_MOTION 

//motor control pin DRV8833
#define     ERR_DETECT     13 

#define     BIN1_PIN     25 
#define     BIN2_PIN     26 

#define     AIN1_PIN     27
#define     AIN2_PIN     14 


//Encode
#define     MA_EC1     16
#define     MA_EC2     17  

#define     MB_EC1     18
#define     MB_EC2     19

// #define REVERT_ENCODER 

//radius depend on mouse !
// radius / pulse per one rotation, encoder , main gear / second gear
// mm per pulse
// #define MM_PP_A  ((32.15 * PI) / (30 * (50/10) * 4))
// #define MM_PP_B  ((31.5 * PI) / (30 * (50/10) * 4))
float a_ratio = 1.055;

// float whellDiameter = 31.5;
float whellDiameter = 30.9;

#define MM_PP_A  ((whellDiameter * a_ratio * PI) / (334 * (50/10) * 4))
#define MM_PP_B  ((whellDiameter * PI) / (334 * (50/10) * 4))


// algorithm control
// #define PD_LOOP_TIME 1000 //micro second
// #define SPEED_UPDATE_LOOP_TIME 1000 //micro second

unsigned long PD_LOOP_TIME = 1000;
unsigned long SPEED_UPDATE_LOOP_TIME = 1000;


#define USING_SPEED_FILTER 
#define USING_TARGET_SPEED_FILTER 
// #define FILTER_VALUE 20
int FILTER_VALUE = 5;