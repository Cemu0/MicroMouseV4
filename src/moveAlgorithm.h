#pragma once
long turnPosition = 20;
bool turnAround = false;

float oldTargetSpeed = 200;
float accelerate = 500; //mm/s^2
// float accelerate = 900; //mm/s^2
unsigned long time_accelerate = 0;
float realAcc;

void stopMove(){
    move_enable = false;  
    targetSpeed = 0;
    fw_speed = 0;
    MotorControl.motorsStop();
}

void hitWallStop(){
    if(frontWall == -1 && move_enable){ // NEARLY HIT THE WALL 
        if(targetSpeed != 0){
            TelnetStream.println("EMERG_STOP");
            stopMove();
        }
    }
}

void gyroMoveAngle(){
    target_angle = - offsetYaw(offset_angle) * E_ratio;

    if(abs(target_angle) < 100 && speedA + speedB > 40)
        rt_speed  = 0;

    else if(abs(target_angle) > 700){
        if(target_angle>0)
            rt_speed = 700;
        else    
            rt_speed = -700;
    }
    else   
        rt_speed = target_angle;
}

// long startPosAfterTurn = 53; //j60
long startPosAfterTurn = 125; //j125
long startPosAfterTurnAround = 75;//i75
//process forward and rotating 
bool movingAlgoUpdate(){
    if (move_enable) {
        if(!turning()){
            setPIDForwardValue();
            if(turnAround){
                turnAround = false;
                targetSpeed = oldTargetSpeed;
                setCellPos(startPosAfterTurnAround);
                TelnetStream.print("finish turn around");
                //keep up speed
                // MotorControl.motorForward(0, 150);
                // MotorControl.motorForward(1, 150);
            }else{
                gyroMoveAngle(); 
            }
        }else{
            if(turnAround){
                setPIDRotatingValue();
                gyroMoveAngle(); //use to turn around and forward
            }else{
                // gyroMoveAngle(); //use gyro for normal turn
                if(EncoderTurnUpdate(TelnetStream)){
                    setCellPos(startPosAfterTurn); //finish turn
                };
                time_accelerate = millis();
            }
            return false;
        }
    }


    //accelerate and calculate the speed
    if(abs(fw_speed - targetSpeed) > 3){
        realAcc = ((millis() - time_accelerate) / 1000.0) * accelerate;
        
        if(fw_speed < targetSpeed)
            fw_speed += realAcc;

        if(fw_speed > targetSpeed)
            fw_speed -= realAcc;

        time_accelerate = millis();
    }

    return true;
}

// long perpendicularCalibrateValue = 0;
long setPointFrontValue = 3550; //~9cm
float calibrateForwardValue = -0.5F; //Z
float calibrateRotateValue = 2.02F; // z
unsigned long calibrateTimer;

void calibrateFrontWall(){
    setPIDCalibrateValue();
    calibrateTimer = millis();
    // spee
    fw_speed = 200; //give trash value for looper
    while (millis()-calibrateTimer < 1500){
    // while (abs(fw_speed) > 3 && millis() - calibrateTimer < 5000){
    // while(1){
        readIRsensor();
        target_angle = - offsetYaw(offset_angle) * E_ratio * calibrateRotateValue;
        speedCalculate();
        if(TelnetStream.available()){
            move_enable = false;
            break;
        }   
        fw_speed = (setPointFrontValue - faceSensorValue3) * calibrateForwardValue;
        // limitAbs(fw_speed, 300.0F);

        //calibrate perpendicular but 2 and 4 sensor angle not correct so this failed
        // rt_speed = (faceSensorValue2 - faceSensorValue4 - perpendicularCalibrateValue) * calibrateRotateValue; //
        // pdSpeedAngular(0, 0, rt_speed, fw_speed, left_pwm, right_pwm); 
        

        //calibrate wall only
        pdSpeedAngular(0, 0, -target_angle, fw_speed, left_pwm, right_pwm); 

        limitAbs(left_pwm, 60L);
        limitAbs(right_pwm, 60L);


            // left_pwm = fw_speed;
            // right_pwm = fw_speed;
            // math_timer = micros();
        motorMove();
        // }
        // if(millis() % 100 == 0){
            // TelnetStream.print(fw_speed);
            // TelnetStream.print(" ");
            // TelnetStream.println(rt_speed);
        // }
        delay(3);
    }
    MotorControl.motorsStop();
    fw_speed = 0;
    rt_speed = 0;

}



void startRotateAround(){
    calibrateFrontWall();

    setPIDRotatingValue();
    gyroTurn(BACKWARD);

    //warning 2 motor have different stress
    MotorControl.motorForward(1, 45);
    MotorControl.motorReverse(0, 80);
    delay(100);
    
    // MotorControl.motorsStop();
    // delay(100);
    turnAround = true;
    oldTargetSpeed = targetSpeed;
    targetSpeed = 0;
    fw_speed = 0; //TODO: using de_accelerate to prevent slip
}


//right wall move
void calculateAlgo(){
    if(move_enable && !turning()){
        if(currentPosInCell == turnPosition){
            // printIR(TelnetStream);
            if(!rightWall){
                    // gyroTurn(RIGHT);
                    // currentPosInCell = 0;
                    setCellPos(0); //need prevent update !!!
                    mazeTurnRight();
                    encoderTurn(1);
                    // moveForward();
                    TelnetStream.println("Turn RIGHT\n");
                    
            }else{
                if(frontWall == 1){
                    if(!leftWall){
                            // gyroTurn(LEFT);
                            mazeTurnLeft();
                            encoderTurn(-1);
                            // moveForward();
                            // currentPosInCell = 0;
                            setCellPos(0); // prevent update
                            TelnetStream.println("Turn LEFT\n");
                    }else{
                        //running rotate sequence
                        mazeTurnAround();
                        startRotateAround();
                        // setCellPos(startPosAfterTurnAround);
                        // moveForward();
                        TelnetStream.println("Turn AROUND\n");
                    }
                }
            }
        }
    }
}

// void gyroTurn(){

// }