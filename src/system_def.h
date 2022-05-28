#pragma once

#define     LED_PIN         2
#define     BUZZ_PIN         12

//Sensor 
#define     SENSOR_PIN_1      36
#define     SENSOR_PIN_2      39
#define     SENSOR_PIN_3      34
#define     SENSOR_PIN_4      35
#define     SENSOR_PIN_5      32


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

//radius depend on mouse !
// radius / pulse per one rotation, encoder , main gear / second gear
// mm per pulse
#define MM_PP_A  ((32.1 * PI) / (30 * (50/10) * 4))
#define MM_PP_B  ((31.5 * PI) / (30 * (50/10) * 4))
