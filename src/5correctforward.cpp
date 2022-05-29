#ifdef MAIN
#include "system_def.h"
#include "OTA.h"
#include "MPU.h"
#include "PD.h"
#include "tone.h"
#include <ESP32Encoder.h>
#include "IRsensor.h"
#include "ESP32MotorControl.h" 

unsigned long entry;

const char* ssid = "CI3_R1";
const char* password = "154dsgf654ref";

ESP32MotorControl MotorControl = ESP32MotorControl();
ESP32Encoder encoderA(true);
ESP32Encoder encoderB(true);

bool move_enable = false;

long targetSpeed = 0;

float speedA = 0;
float speedB = 0;

float a_ratio = 30.5;

int64_t oldCountA = 0;
int64_t oldCountB = 0;

float offset_angle = 0;
float E_ratio = 700;

#define FILTER 7

unsigned long speed_timer = 0;
unsigned long acc_timer = 0;
bool speedCalculate(){
    auto deltaTime = millis() - speed_timer;
    auto deltaTimeb = micros() - acc_timer;
    if( deltaTime >= 5){ // THIS SHOULD BE FASTER IF YOU PLAN TO RUN FAST
        //convert to mm/s 
        // float tmpSpeedA = ((float)(encoderA.getCount() - oldCountA) / (deltaTimeb / 1000000.0)) * (a_ratio * PI) / (30 * (50/10) * 4);
        float tmpSpeedA = ((float)(encoderA.getCount() - oldCountA) / (deltaTimeb / 1000000.0)) * MM_PP_A;
        float tmpSpeedB = ((float)(encoderB.getCount() - oldCountB) / (deltaTimeb / 1000000.0)) * MM_PP_B;

        //using a simple filter due to encoder low accuracy  
        speedA = (tmpSpeedA + speedA*FILTER)/(FILTER+1);
        speedB = (tmpSpeedB + speedB*FILTER)/(FILTER+1);

        // speedA = encoderA.getCount() - oldCountA;
        // speedB = encoderB.getCount() - oldCountB;
        oldCountA = encoderA.getCount();
        oldCountB = encoderB.getCount();
        speed_timer = millis();
        acc_timer = micros();
        return true;
    }
    return false;
}

void setup(){
    MotorControl.attachMotors(BIN2_PIN, BIN1_PIN, AIN2_PIN, AIN1_PIN);
    MotorControl.motorsStop();
    pinMode(12,OUTPUT);
    tone(12,1000);

    ESP32Encoder::useInternalWeakPullResistors=UP;
    encoderA.attachFullQuad(MA_EC2, MA_EC1);
    encoderB.attachFullQuad(MB_EC1, MB_EC2);

    pinMode(2,OUTPUT);
    digitalWrite(2,LOW);
    initIR();
    delay(100);
    tone(12,0);
    delay(100);
    tone(12,1000);
    delay(100);
    tone(12,0);
    tone(12,1000);
    delay(100);
    tone(12,0);
    Serial.begin(115200);
    if(!setupMPU()){
        tone(12,500);
        delay(1000);
        tone(12,0);
    }    
    tone(12,1000);
    delay(100);
    tone(12,0);
    Serial.println("Connecting ... ");
    connectWifi(ssid,password);
    setupServer();
    Serial.println("HTTP server started");
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    tone(12,1000);
    delay(100);
    tone(12,0);
}

long fw_speed = 0;
long rt_speed = 0;
long left;
long right;
float target_angle;
float speed;

byte debug = 0;

#define RIGHT PI/2.0
#define LEFT -PI/2.0
#define BACKWARD PI

#define TO_RADIAN(n) (n  * PI) / 180.0
#define TO_DEG(n) n * (180/PI)

#define TURNING_TIME 500

unsigned long lastTurn = 0;

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

unsigned long calculate_timer = 0;
unsigned long LOG_timer = 0;
void loop(){
    digitalWrite(2,LOW);
    entry = micros();
    while(TelnetStream.available()){
        String data = TelnetStream.readStringUntil('\n');
        switch (data[0]) {
            case 'R':
                TelnetStream.stop();
                delay(100);
                ESP.restart();
                break;
            // case 'C':
            //     TelnetStream.println("bye bye");
            //     TelnetStream.stop();
            //     break;
            case 'w':
                TelnetStream.println("Test Forward");
                offset_angle = ypr[0];
                targetSpeed = data.substring(1).toInt();
                move_enable = true;
                break;
            case 'S':
            case 's':
                TelnetStream.println("STOP");
                targetSpeed = 0;
                rt_speed = 0;
                move_enable = false;  
                MotorControl.motorsStop();
                break;
            case 'C':
                offset_angle = TO_RADIAN(data.substring(1).toFloat());
                TelnetStream.println("TURNING...");
                break;

            case 'P':
                P_speed = data.substring(1).toFloat();
                errorP_speed = 0;
                TelnetStream.println("CHANGED");
                break;

            case 'D':
                D_speed = data.substring(1).toFloat();
                errorD_speed = 0;
                TelnetStream.println("CHANGED");
                break;

            case 'p':
                P_rotation = data.substring(1).toFloat();
                errorP_rotation = 0;
                TelnetStream.println("CHANGED");
                break;

            case 'd':
                D_rotation = data.substring(1).toFloat();
                errorD_rotation = 0;
                TelnetStream.println("CHANGED");
                break;

            case 'E':
                E_ratio = data.substring(1).toFloat();
                oldErrorP_rotation = 0;
                TelnetStream.println("CHANGED");
                break;

            case 'a':
                a_ratio = data.substring(1).toFloat();
                TelnetStream.println("CHANGED");
                break;

            case 'm':
                speed = data.substring(1).toFloat();
                if(speed > 0){
                    MotorControl.motorForward(0, speed);
                    MotorControl.motorForward(1, speed);
                }else{
                    MotorControl.motorReverse(0, -speed);
                    MotorControl.motorReverse(1, -speed);
                }
                TelnetStream.println("moving");
                break;

            case 'f':
                hasFrontWall = data.substring(1).toInt();
                TelnetStream.println("CHANGED");
                break;

            case 'l':
                hasLeftWall = data.substring(1).toInt();
                TelnetStream.println("CHANGED");
                break;

            case 'r':
                hasRightWall = data.substring(1).toInt();
                TelnetStream.println("CHANGED");
                break;

            case 'b':
                debug = data.substring(1).toInt();
                break;

            default:
                break;
        }
    }

    // Your code here
    ws.cleanupClients();
    readGyro();
    readIRsensor();
    if(speedCalculate()){



    if(abs(Yvalue) < 25)
        Yvalue = 0; 

    if(abs(Xvalue) < 25)
        Xvalue = 0; 

    if(Yvalue !=0 || Xvalue!=0){
        targetSpeed = Yvalue/2;
        rt_speed = Xvalue/2;
        // offset_angle += rt_speed/10;
    }

    //run every millisecond
    // if(millis() != calculate_timer){

        if(faceSensorValue3 > 3500 && move_enable){ // NEARLY HIT THE WALL 
            if(targetSpeed != 0){
                TelnetStream.println("EMERG_STOP");
                move_enable = false;  
                targetSpeed = 0;
                MotorControl.motorsStop();
                // move_enable = false;
                // rt_speed = -45;
            }
        }

        if(faceSensorValue3 > hasFrontWall && !turning() && move_enable){ // turn 1700
        // if(!turning()){ // turn 1700
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
                } else if(faceSensorValue2 < hasLeftWall){ // turn right
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
                    targetSpeed = 0;
                    TelnetStream.println("ROTATE");
                    // MotorControl.motorsStop();
                    // move_enable = false;  

                }
                // offset_angle += (PI/2.0);
            } 
        } else if (move_enable && !turning()) {
            setForwardValue();
            targetSpeed = 200;
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


    //should turn off for more accuracy -_-
        // if(move_enable && !turning()){
        //         if(faceSensorValue5 > hasRightWall && faceSensorValue1 > hasLeftWall){
        //             if(fw_speed != 0){
        //                 // float offset = (faceSensorValue1 - faceSensorValue5) / 10000000.0;
                        
        //                 // turn(0);
        //                 // float offset = (faceSensorValue1 - faceSensorValue5) / 1000.0;
        //                 // if(abs(offset) > 0.00093599){
        //                 //     TelnetStream.print("osw");
        //                 //     // TelnetStream.println(TO_DEG(offset));
        //                 //     // offset_angle += offset;
        //                 //     rt_speed += offset;
        //                 // }
        //                 // else
        //                 //     TelnetStream.println(TO_DEG(offset));
        //             }
        //         }
        // }
        //         // else if((faceSensorValue1 > leftMiddleValue))// nearly touch the wall
        //         // {
        //         //     float offset = (leftMiddleValue - faceSensorValue1) / 100000.0;
        //         //     TelnetStream.print("slight right");
        //         //     TelnetStream.println(TO_DEG(offset));
        //         //     offset_angle -= offset;
        //         // }else if((faceSensorValue5 > rightMiddleValue))// nearly touch the wall
        //         // {
        //         //     float offset = (rightMiddleValue - faceSensorValue5) / 100000.0;
        //         //     TelnetStream.print("slight left");
        //         //     TelnetStream.println(TO_DEG(offset));
        //         //     offset_angle += offset;
        //         // }
        // }

        fw_speed = (fw_speed*7 + targetSpeed) / 8;
        // fw_speed = targetSpeed;

        
        PD_SPEED_ANGULAR(fw_speed, rt_speed, ((speedA - speedB) * 2), (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(target_angle), 0, left, right);
        // PD_SPEED_ANGULAR(fw_speed, (long)(target_angle), (speedA - speedB) * 2, (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(ypr[0] * 5), (speedA + speedB) / 2, left, right);
    //     calculate_timer = millis();
    // }
        if(move_enable){
            if(left > 0){
                // left += E_ratio; //-2 ok but lag
                MotorControl.motorForward(1, left);
            }else{
                // left -= E_ratio;
                MotorControl.motorReverse(1, -left);
            }
            
            if(right > 0){
                // right += E_ratio;
                MotorControl.motorForward(0, right);
            }else{
                // right -= E_ratio;
                MotorControl.motorReverse(0, -right);
            }
        }
    }
    

    if(millis() - LOG_timer > 100){
        if(debug == 1){
            TelnetStream.print(" ");
            TelnetStream.print(speedA);
            TelnetStream.print(" ");
            TelnetStream.print(speedB);
            TelnetStream.print(" ");
            TelnetStream.print(encoderA.getCount());
            TelnetStream.print(" ");
            TelnetStream.println(encoderB.getCount());
        }
        // TelnetStream.print(" ");
        // TelnetStream.print(left);
        // TelnetStream.print(" ");
        // TelnetStream.print(right);
        // TelnetStream.print(" ");
        // TelnetStream.print(ypr[0] * 180/M_PI);
        // TelnetStream.print(" ");
        // TelnetStream.print(P_speed);
        // TelnetStream.print(" ");
        // TelnetStream.print(D_speed);
        // TelnetStream.print(" ");
        if(debug == 2){
            TelnetStream.print(target_angle);
            TelnetStream.print(" ");
            TelnetStream.print(offset_angle * 180/M_PI);
            TelnetStream.print(" ");
            TelnetStream.print(ypr[0] * 180/M_PI);
            TelnetStream.print(" ");
            TelnetStream.println(offsetYaw(offset_angle)* 180/M_PI);
        }

        if(debug == 5){
            TelnetStream.print(micros()-entry); 
            printIR(TelnetStream);
        }

        // Serial.print(" ");
        // Serial.print(speedA);
        // Serial.print(" ");
        // Serial.print(speedB);
        // Serial.print(" ");
        // Serial.print(left);
        // Serial.print(" ");
        // Serial.print(right);
        // Serial.print(" ");
        // Serial.print(ypr[0] * 180/M_PI);
        // printIR(Serial);

        // printIR(Serial);
        LOG_timer = millis();
    }



    #ifndef USE_ASYNC_ELEGANT_OTA
        ArduinoOTA.handle();
    #endif
    digitalWrite(2,HIGH);
}
#endif