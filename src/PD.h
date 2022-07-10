
#pragma once

#define P 1
#define D 1

/*
long errorP = 0;
long errorD = 0;
long oldErrorP = 0;

//http://micromouseusa.com/?p=389
long PID_SENSOR_FORWARD(long &leftSensor, long &rightSensor)       
{    
    if((leftSensor > hasLeftWall && rightSensor > hasRightWall))//has both walls
    {  //ccw direction is positive
        errorP = rightSensor - leftSensor - 63;//63 is the offset between left and right sensor when mouse in the middle of cell
        errorD = errorP - oldErrorP;
    }        
    else if((leftSensor > hasLeftWall))//only has left wall
    {
        errorP = 2 * (leftMiddleValue - leftSensor);
        errorD = errorP - oldErrorP;
    }
    else if((rightSensor > hasRightWall))//only has right wall
    {
        errorP = 2 * (rightSensor - rightMiddleValue);
        errorD = errorP - oldErrorP;
    }
    else if((leftSensor < hasLeftWall && rightSensor <hasRightWall))//no wall, use encoder or gyro
    {
        errorP = 0;//(leftEncoder - rightEncoder*1005/1000)*3;
        errorD = 0;
    }
    // totalError = P * errorP + D * errorD;
    return P * errorP + D * errorD;
    oldErrorP = errorP;
    // setLeftPwm(leftBaseSpeed - totalError);
    // setRightPwm(rightBaseSpeed + totalError);    
}
*/

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
    P_rotation = 1.25;
    D_rotation = 2.33;
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

long errorP_speed = 0;
long errorD_speed = 0;
long oldErrorP_speed = 0;
long xError = 0;

long errorP_rotation = 0;
long errorD_rotation = 0;
long oldErrorP_rotation = 0;
long wError = 0;


void pdSpeedAngular(const long &xSpeed,const long &wSpeed,const float& rotationError,const float& currentSpeed, 
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

void calculatePD(){
    if(micros() - math_timer > PD_LOOP_TIME){
        pdSpeedAngular(fw_speed, rt_speed, ((speedA - speedB) * 2.0), (speedA + speedB) / 2.0, left_pwm, right_pwm);
        math_timer = micros();
    }
}