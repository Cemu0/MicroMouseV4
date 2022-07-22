#pragma once
long turnPosition = 0;
bool turnAround = false;

float oldTargetSpeed = 200;
float accelerate = 500; //mm/s^2
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

long startPosAfterTurnAround = 170;
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
            }else{
                gyroMoveAngle(); 
            }
        }else{
            if(turnAround){
                setPIDRotatingValue();
                gyroMoveAngle(); //use to turn around and forward
            }else{
                // gyroMoveAngle(); //use gyro for normal turn
                EncoderTurnUpdate(TelnetStream);
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

void startRotateAround(){
    setPIDRotatingValue();
    gyroTurn(BACKWARD);
    MotorControl.motorsStop();
    delay(100);
    turnAround = true;
    oldTargetSpeed = targetSpeed;
    targetSpeed = 0;
    fw_speed = 0; //TODO: using de_accelerate to prevent slip
}

long startPosAfterTurn = 45;

//right wall move
void calculateAlgo(){
    if(move_enable && !turning()){
        if(currentPosInCell >= turnPosition){
            printWall(TelnetStream);
            if(!rightWall){
                    // gyroTurn(RIGHT);
                    encoderTurn(1);
                    mazeTurnRight();
                    // moveForward();
                    // currentPosInCell = 0;
                    setCellPos(startPosAfterTurn);
                    TelnetStream.println("Turn RIGHT");
                    
            }else{
                if(frontWall == 1){
                    if(!leftWall){
                            // gyroTurn(LEFT);
                            encoderTurn(-1);
                            mazeTurnLeft();
                            // moveForward();
                            // currentPosInCell = 0;
                            setCellPos(startPosAfterTurn);
                            TelnetStream.println("Turn LEFT");
                    }else{
                        //running rotate sequence
                        startRotateAround();
                        // setCellPos(startPosAfterTurnAround);
                        mazeTurnAround();
                        // moveForward();
                        TelnetStream.println("Turn AROUND");
                    }
                }
            }
        }
    }
}

// void gyroTurn(){

// }