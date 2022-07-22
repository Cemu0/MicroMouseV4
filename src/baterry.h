#pragma once 

#include "variable.h"

//https://en.ovcharov.me/2020/02/29/how-to-measure-battery-level-with-esp32-microcontroller/
#define VOLTAGE_OUT(Vin) (((Vin) * R3) / (R2 + R3))

// void setupBaterryMonitor(){
        // pinMode(BATTERY_SENSOR, INPUT)
// }

float baterryVoltage(){

    /*
        MAX: 8.4 vol when full
        LOW: 6 vol
        
        1.58 = 7vol
    */
    
    return ((analogRead(BATTERY_SENSOR) * 3.3) / 4095) * 4.43; //VOLTAGE_OUT();
}