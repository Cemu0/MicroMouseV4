#include "variable.h"
#include "tone.h"
#include "IRsensor.h"
#include "PD.h"
#include "motor.h"
#include "MPU.h" 
#include "OTA.h"
#include "maze.h"
#include "algorithm.h"
#include "debug.h"


void setup(){
    initMotor();
    initTone();
    tone();
    initEncoder();
    initIR();
    // initMPU();
    initDebug();
    tone(3000,100);
    if(!initMPU())
        while(1){
            tone(1000,200);
            delay(100);
        }
    tone(1000,100);
    readyTone();
}

void updateMovement(){
    readIRsensor();
    speedCalculate();

    hitWallStop();
    if(updateCellPos()){
        TelnetStream.println(++cellX);
    // if(updateCellPos()){
        // readIRsensor();
        //when enter new cell 
        tone(1000,10);
    calculateAlgo();
    };
    calculatePD(movingAlgoUpdate());
    motorMove();
}

void loop(){
    buildInLedOn();
    entry = micros();
    ws.cleanupClients(1);
    updateMovement();
    #ifndef USE_ASYNC_ELEGANT_OTA
        ArduinoOTA.handle();
    #endif
    buildInLedOff();
    telnetStreamHandle();
    debugHandle();
}
