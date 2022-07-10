#pragma once
long turnPosition = 30;
bool turnAround = false;
long oldTargetSpeed = 0;
void hitWallStop(){
    if(frontWall == -1 && move_enable){ // NEARLY HIT THE WALL 
        if(targetSpeed != 0){
            TelnetStream.println("EMERG_STOP");
            move_enable = false;  
            targetSpeed = 0;
            MotorControl.motorsStop();
        }
    }
}

void movingUpdate(){
    if (move_enable ) {
        if(!turning()){
            setPIDForwardValue();
            if(turnAround){
                turnAround = false;
                targetSpeed = oldTargetSpeed;
            }
        }
    }else{
        
    }
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

    //should replace by a accelerate calculate
    #ifdef USING_TARGET_SPEED_FILTER
        fw_speed = (fw_speed * FILTER_VALUE + targetSpeed) / (FILTER_VALUE+1);
    #else
        fw_speed = targetSpeed;
    #endif
}

void startRotateAround(){
    setPIDRotatingValue();
    turn(BACKWARD);
    turnAround = true;
    oldTargetSpeed = targetSpeed;
    targetSpeed = 0;
}

//right wall move
void calculateAlgo(){
    hitWallStop();
    if(move_enable && currentPosInCell > turnPosition){
        if(!rightWall){
            TelnetStream.println("Turn RIGHT");
            turn(RIGHT);
        }else{
            if(frontWall){
                if(!leftWall){
                    turn(LEFT);
                    TelnetStream.println("Turn LEFT");
                }else{
                    //running rotate sequence
                    startRotateAround();
                    TelnetStream.println("Turn AROUND");

                }
            }

        }
    }
    movingUpdate();
}