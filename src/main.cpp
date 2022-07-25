#include "helperFunctions.h"
#include "variable.h"
#include "tone.h"
#include "IRsensor.h"
#include "motor.h"
#include "maze.h"
#include "PD.h"
#include "MPU.h" 
#include "OTA.h"
#include "algorithm.h"
#include "baterry.h"

//our target
int stopX = 1;
int stopY = 6;

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

        // if(cellX == 4 && cellY == 4)
        if(cellX == stopX && cellY == stopY){
            stopMove();
            successTone();
        }
    // if(updateCellPos()){
        //when enter new cell 
        tone(1000,3);
        // readIRsensor();
    };
    calculateAlgo();

    calculatePD(movingAlgoUpdate());

    motorMove();
}

void loop(){
    if(!move_enable){
        if(faceSensorValue4 > 4000 && faceSensorValue2 > 4000){
            startTone();
            startRun();
        }
    }
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
