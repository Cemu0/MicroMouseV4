#pragma once

#include "system_def.h"
#include "variable.h"
#include <ESP32Encoder.h>
#include "ESP32MotorControl.h" 


ESP32MotorControl MotorControl = ESP32MotorControl();
ESP32Encoder encoderA(true);
ESP32Encoder encoderB(true);

void initMotor(){
    // MotorControl.attachMotors(BIN2_PIN, BIN1_PIN, AIN2_PIN, AIN1_PIN);
    MotorControl.attachMotors(AIN2_PIN, AIN1_PIN, BIN2_PIN, BIN1_PIN);
    MotorControl.motorsStop();
}

void initEncoder(){
    ESP32Encoder::useInternalWeakPullResistors=UP;

    #ifndef REVERT_ENCODER
        encoderA.attachFullQuad(MA_EC1, MA_EC2);
        encoderB.attachFullQuad(MB_EC2, MB_EC1);
    #else
        encoderA.attachFullQuad(MA_EC2, MA_EC1);
        encoderB.attachFullQuad(MB_EC1, MB_EC2);
    #endif
}

bool speedCalculate(){
    // auto deltaTime = millis() - speed_timer;
    unsigned long deltaTime = micros() - acc_timer;
    if(deltaTime >= SPEED_UPDATE_LOOP_TIME){ // THIS SHOULD BE FASTER IF YOU PLAN TO RUN FAST
        //convert to mm/s 
        // float tmpSpeedA = ((float)(encoderA.getCount() - oldCountA) / (deltaTimeb / 1000000.0)) * (a_ratio * PI) / (30 * (50/10) * 4);
        float tmpSpeedA = ((float)(encoderA.getCount() - oldCountA) / (deltaTime / 1000000.0)) * MM_PP_A;
        float tmpSpeedB = ((float)(encoderB.getCount() - oldCountB) / (deltaTime / 1000000.0)) * MM_PP_B;

        #ifdef USING_SPEED_FILTER
        //using a simple filter due to encoder low accuracy  
            speedA = (tmpSpeedA + speedA*FILTER_VALUE)/(FILTER_VALUE+1);
            speedB = (tmpSpeedB + speedB*FILTER_VALUE)/(FILTER_VALUE+1);
        #else
            speedA = tmpSpeedA;
            speedB = tmpSpeedB;
        #endif
        
        // speedA = encoderA.getCount() - oldCountA;
        // speedB = encoderB.getCount() - oldCountB;
        oldCountA = encoderA.getCount();
        oldCountB = encoderB.getCount();
        // speed_timer = millis();
        acc_timer = micros();
        return true;
    }
    return false;
}


void gyroTurn(float angle){
    lastTurn = millis();
    offset_angle += angle;

    //convert rotation from 270 to -90
    if(offset_angle > PI)
        offset_angle -= 2.0  * PI;

    //convert rotation from -270 to 90
    else if(offset_angle <= -PI)
        offset_angle += 2.0 * PI;
}

bool turning(){
    // return abs(offsetYaw(offset_angle) - ypr[0]) > 5;
    return millis() - lastTurn < TURNING_TIME;
}

long turningLength = 0.25 * 180 * PI + 40; //141.37166941154069573081895224758
int64_t accelerateLength = 0;
// int64_t turnLength = 0;
// int64_t turnLengthOld = 0;
float turnSide = 1;

float turnSpeed = 450; //h20
float turnAccelerate = 9000; //mm/s^2 //g100
unsigned long turnTime = 530000; //mc //g100


unsigned long accelerateTimer = 0;
unsigned long keepSpeedTimer = 0;
int turningPhase = 0;

void encoderTurn(float side){
    turnSide = side;
    lastTurn = millis();
    rt_speed = 0;
    turningPhase = 0;
    // turnLengthOld = (encoderA.getCount() + encoderB.getCount())/2;
    accelerateTimer = micros();
    if(side > 0)
        gyroTurn(RIGHT);
    else
        gyroTurn(LEFT);
    // TelnetStream.print(" start turn ");
    
}

// start ... k ee p ... end
// 0:0       0 : 1      0:2
// |---acc---|----|--dec---|

//TODO: Turn to micro for more preside
void EncoderTurnUpdate(Stream &stream){
    // rt_speed = 0;
    // auto absLength = (encoderA.getCount() + encoderB.getCount())/(int64_t)2;
    // turnLength = absLength - turnLengthOld;
    // turnLengthOld = absLength;

    // stream.print(encoderA.getCount());
    // stream.print(" ");
    // stream.print(encoderB.getCount());
    // stream.print(" ");

    //speed up
    if(abs(rt_speed) < turnSpeed && turningPhase == 0){
        rt_speed += turnSide * ((micros() - accelerateTimer) / 1000000.0) * turnAccelerate;
        accelerateTimer = micros();
        keepSpeedTimer = accelerateTimer;
        // timeTurnAccelerate = accelerateTimer - lastTurn;
        // accelerateLength = turnLength;
        // stream.print(rt_speed);
        // stream.print(" ");
        // stream.print(accelerateLength);
        // stream.println(" acc");
    //keep speed
    }else if (micros() - keepSpeedTimer < turnTime){
        // stream.print(millis() - timeTurnAccelerate < turnTime);
        // stream.println("keep");
        // timeTurnAccelerate = millis();
        accelerateTimer = micros();
        turningPhase = 1;
    //speed down
    }else if (turnSide * rt_speed > 0){
        turningPhase = 2;
        rt_speed  -= turnSide * ((micros() - accelerateTimer) / 1000000.0) * turnAccelerate;
        accelerateTimer = micros();
        // stream.print(turnSide * rt_speed);
        // stream.println("dec");
    }
    
    if (turnSide * rt_speed <= 0 && turningPhase == 2) {
        turningPhase = 3;
        lastTurn = 0; //complete turn
        rt_speed = 0;
        stream.println(" complete turn");
    }
    
}

void motorMove(){
    if(move_enable){
        if(left_pwm > 0){
            left_pwm+=30;
            MotorControl.motorForward(0, left_pwm);
        }else{
            left_pwm-=30;
            MotorControl.motorReverse(0, -left_pwm);
        }

        if(right_pwm > 0){
            right_pwm+=30;
            MotorControl.motorForward(1, right_pwm);
        }else{
            right_pwm-=30;
            // right -= E_ratio;
            MotorControl.motorReverse(1, -right_pwm);
        }
    }else{
        MotorControl.motorsStop();
    }
}

