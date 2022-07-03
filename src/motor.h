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


void turn(float angle){
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