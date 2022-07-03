#include "variable.h"
#include "PD.h"
#include "tone.h"
#include "IRsensor.h"
#include "motor.h"
#include "MPU.h" 
#include "OTA.h"
#include "debug.h"
#include "algorithm.h"

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

void loop(){
    buildInLedOn();
    entry = micros();
    ws.cleanupClients(1);
    readIRsensor();
    speedCalculate();

    calculateAlgo();

    calculatePD();
    motorMove();
    #ifndef USE_ASYNC_ELEGANT_OTA
        ArduinoOTA.handle();
    #endif
    buildInLedOff();
    telnetStreamHandle();
    debugHandle();
}
