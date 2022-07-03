#pragma once

void calculateAlgo(){
    
    if(frontWall && move_enable){ // NEARLY HIT THE WALL 
        if(targetSpeed != 0){
            TelnetStream.println("EMERG_STOP");
            move_enable = false;  
            targetSpeed = 0;
            MotorControl.motorsStop();
        }
    }

    if(frontWall && !turning() && move_enable){ // turn 1700
        if(targetSpeed != 0){
            TelnetStream.println("APPROACH EDGE");
            // fw_speed = 0;
            // move_enable = false;
            // rt_speed = -45;
            if(faceSensorValue4 < hasRightWall){ // turn right
                TelnetStream.print(TO_DEG(ypr[0]));
                TelnetStream.print(" ");
                TelnetStream.print(TO_DEG(offset_angle));
                TelnetStream.print(" ");
                turn(RIGHT);
                TelnetStream.print(TO_DEG(offset_angle));
                TelnetStream.println("Turn RIGHT");
            } else if(faceSensorValue2 < hasLeftWall){ // turn left
                TelnetStream.print(TO_DEG(ypr[0]));
                TelnetStream.print(" ");
                TelnetStream.print(TO_DEG(offset_angle));
                TelnetStream.print(" ");
                turn(LEFT);
                TelnetStream.print(TO_DEG(offset_angle));
                TelnetStream.println("Turn LEFT");
            }else{
                setRotatingValue();
                turn(BACKWARD);
                // targetSpeed = 0;
                TelnetStream.println("ROTATE");
                // MotorControl.motorsStop();
                // move_enable = false;  

            }
            // offset_angle += (PI/2.0);
        } 
    } else if (move_enable && !turning()) {
        setForwardValue();
        // targetSpeed = 200;
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

        #ifdef USING_TARGET_SPEED_FILTER
            fw_speed = (fw_speed * FILTER_VALUE + targetSpeed) / (FILTER_VALUE+1);
        #else
            fw_speed = targetSpeed;
        #endif
}