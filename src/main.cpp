#include "variable.h"
#include "tone.h"
#include "IRsensor.h"
#include "PD.h"
#include "motor.h"
#include "MPU.h" 
#include "OTA.h"
#include "maze.h"
#include "algorithm.h"
#include "baterry.h"
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
        moveForward();
        TelnetStream.print(cellX);
        TelnetStream.print(",");
        TelnetStream.println(cellY);

        if(cellX == 3 && cellY == 3)
        // if(cellX == 13 && cellY == 2)
            stopMove();
    // if(updateCellPos()){
        // readIRsensor();
        //when enter new cell 
        calculateAlgo();
        // tone(1000,3);
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
