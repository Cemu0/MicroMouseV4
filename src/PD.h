
#pragma once

// #define WallP 1
// #define WallD 1

// long errorP = 0;
// long errorD = 0;
// long oldErrorP = 0;

// long centerMoveVal = 340;

// if(leftWall == 1 && rightWall == 1){
        //     if(forwardWall && fw_speed > 0){
        //         rotateErrorP = faceSensorValue1 - faceSensorValue5 - centerMoveVal;
        //         //combine both gyro and side wall 
        //         errorD_rotate = rotateErrorP - oldRotateErrorP;
        //         rt_speed = rotateErrorP * forwardWallRatioP + errorD_rotate * forwardWallRatioD;
        //     }
        // }

//http://micromouseusa.com/?p=389
// long PID_SENSOR_FORWARD(long &leftSensor, long &rightSensor)       
// {    
//     if((leftSensor > hasLeftWall && rightSensor > hasRightWall))//has both walls
//     {  //ccw direction is positive
//         errorP = rightSensor - leftSensor - centerMoveVal;
//         errorD = errorP - oldErrorP;
//     }        
//     else if((leftSensor > hasLeftWall))//only has left wall
//     {
//         errorP = 2 * (leftMiddleValue - leftSensor);
//         errorD = errorP - oldErrorP;
//     }
//     else if((rightSensor > hasRightWall))//only has right wall
//     {
//         errorP = 2 * (rightSensor - rightMiddleValue);
//         errorD = errorP - oldErrorP;
//     }


//     // else if((leftSensor < hasLeftWall && rightSensor <hasRightWall))//no wall, use encoder or gyro
//     // {
//     //     errorP = 0;//(leftEncoder - rightEncoder*1005/1000)*3;
//     //     errorD = 0;
//     // }
//     totalError = WallP * errorP + WallD * errorD;
//     // return P * errorP + D * errorD;
//     // oldErrorP = errorP;
//     // setLeftPwm(leftBaseSpeed - totalError);
//     // setRightPwm(rightBaseSpeed + totalError);    
// }

float P_speed = 0.7;
float D_speed = 0.4;

// float P_speed = 0;
// float D_speed = 0;

//FOR ROTATE USING ENCODER
// #define P_rotation 2.5
// #define D_rotation 7.0

//FOR corner ROTATE USING GYRO
float P_rotation = 0.25;
float D_rotation = 0.73;

// float P_rotation = 0;
// float D_rotation = 0;

//for rotating at position
void setPIDRotatingValue(){
    P_rotation = 1.12;
    D_rotation = 2.33;
    // P_rotation = 0;
    // D_rotation = 0;
}

void setPIDWallMoveValue(){
    P_rotation = 0.10;
    D_rotation = 0.25;
    // P_rotation = 0;
    // D_rotation = 0;
}

void setPIDForwardValue(){
    P_rotation = 0.25;
    D_rotation = 0.73;
    // P_rotation = 0;
    // D_rotation = 0;
}
// float P_rotation = 0.5;
// float D_rotation = 0;

float errorP_speed = 0;
float errorD_speed = 0;
float oldErrorP_speed = 0;
float xError = 0;

float errorP_rotation = 0;
float errorD_rotation = 0;
float oldErrorP_rotation = 0;
float wError = 0;


void pdSpeedAngular(const float &xSpeed,const float &wSpeed,const float& rotationError,const float& currentSpeed, 
                            long& rightPWM,  long& leftPWM){
    // PD for the Movement
    errorP_speed = xSpeed - currentSpeed;
    errorD_speed = errorP_speed - oldErrorP_speed;
    xError = P_speed * errorP_speed + D_speed * errorD_speed;
    oldErrorP_speed = errorP_speed;

    // PD for the 
    errorP_rotation = wSpeed - rotationError;
    errorD_rotation = errorP_rotation - oldErrorP_rotation;
    wError = P_rotation * errorP_rotation + D_rotation * errorD_rotation;
    oldErrorP_rotation = errorP_rotation;

    //result
    rightPWM = xError + wError;
    leftPWM = xError - wError;
}

long rotateErrorP = 0;
// long oldRotateErrorP = 0;
float forwardWallRatioP = 0.2;
// float forwardWallRatioD = 0.05;

// float errorD_rotate = 0;

long centerMoveVal = 340;

// long centerMoveVal = 850;

void calculatePD(bool forwardWall = true){
    if(micros() - math_timer > PD_LOOP_TIME){
        if(leftWall == 1 && rightWall == 1){
            if(forwardWall && fw_speed > 0){
                rotateErrorP = faceSensorValue1 - faceSensorValue5 - centerMoveVal;
                //combine both gyro and side wall 
                // errorD_rotate = rotateErrorP - oldRotateErrorP;
                rt_speed = rotateErrorP * forwardWallRatioP;// + errorD_rotate * forwardWallRatioD;
                setPIDWallMoveValue();
            }
        }

        pdSpeedAngular(fw_speed, rt_speed, ((speedA - speedB) * 2.0), (speedA + speedB) / 2.0, left_pwm, right_pwm);
        math_timer = micros();
    }
}