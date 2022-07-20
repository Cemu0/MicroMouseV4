#pragma once
long turnPosition = 0;
bool turnAround = false;

float oldTargetSpeed = 200;
float accelerate = 500; //mm/s^2
unsigned long time_accelerate = 0;
float realAcc;

void hitWallStop(){
    if(frontWall == -1 && move_enable){ // NEARLY HIT THE WALL 
        if(targetSpeed != 0){
            TelnetStream.println("EMERG_STOP");
            move_enable = false;  
            targetSpeed = 0;
            fw_speed = 0;
            MotorControl.motorsStop();
        }
    }
}

void gyroMoveAngle(){
    target_angle = - offsetYaw(offset_angle) * E_ratio;

    if(abs(target_angle) < 100 && speedA + speedB > 40)
        rt_speed = 0;

    else if(abs(target_angle) > 700){
        if(target_angle>0)
            rt_speed = 700;
        else    
            rt_speed = -700;
    }
    else   
        rt_speed = target_angle;
}

//process forward and rotating 
bool movingAlgoUpdate(){
    if (move_enable) {
        if(!turning()){
            setPIDForwardValue();
            if(turnAround){
                turnAround = false;
                targetSpeed = oldTargetSpeed;
                currentPosInCell = 40;
            }else{
                gyroMoveAngle(); //use to turn around and forward
            }
        }else{
            // gyroMoveAngle(); //use gyro for normal turn
            EncoderTurnUpdate(TelnetStream);
            time_accelerate = millis();
            return false;
        }
    }


    //accelerate and calculate the speed
    if(abs(fw_speed - targetSpeed) > 1){
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
    MotorControl.motorsStop();
    delay(100);
    setPIDRotatingValue();
    gyroTurn(BACKWARD);
    turnAround = true;
    oldTargetSpeed = targetSpeed;
    targetSpeed = 0;
    fw_speed = 0; //TODO: using de_accelerate to prevent slip
}


//right wall move
void calculateAlgo(){
    if(move_enable && !turning()){
        if(currentPosInCell >= turnPosition){
            printWall(TelnetStream);
            if(!rightWall){
                    // gyroTurn(RIGHT);
                    EncoderTurn(0.5);
                    currentPosInCell = 0;
                    TelnetStream.println("Turn RIGHT");
            }else{
                if(frontWall == 1){
                    if(!leftWall){
                            // gyroTurn(LEFT);
                            EncoderTurn(-0.5);
                            currentPosInCell = 0;
                            TelnetStream.println("Turn LEFT");
                    }else{
                        //running rotate sequence
                        startRotateAround();
                        TelnetStream.println("Turn AROUND");
                    }
                }
            }
        }
    }
}

// void gyroTurn(){

// }