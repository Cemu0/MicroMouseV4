#ifdef MAIN
#include "pinDefine.h"
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

long speedA = 0;
long speedB = 0;

int64_t oldCountA = 0;
int64_t oldCountB = 0;

float offset_angle = 0;

#define FILTER 5

unsigned long speed_timer = 0;
bool speedCalculate(){
    auto detatime = millis() - speed_timer;
    if( detatime >= 10){ // THIS SHOULD BE FASTER IF YOU PLAN TO RUN FAST
        speedA = ((encoderA.getCount() - oldCountA) + speedA*FILTER)/(FILTER+1);
        speedB = ((encoderB.getCount() - oldCountB) + speedB*FILTER)/(FILTER+1);

        // speedA = encoderA.getCount() - oldCountA;
        // speedB = encoderB.getCount() - oldCountB;
        oldCountA = encoderA.getCount();
        oldCountB = encoderB.getCount();
        speed_timer = millis();
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
float E_ratio = 50;
float speed;

byte debug = 0;

#define RIGHT PI/2.0
#define LEFT -PI/2.0
#define BACKWARD PI

#define TURNING_TIME 500

unsigned long lastTurn = 0;

void turn(float angle){
    lastTurn = millis();
    offset_angle += angle;

    //convert rotation from 270 to -90
    if(offset_angle > PI)
        offset_angle -= (3.0/2.0) * PI;

    //convert rotation from -270 to 90
    else if(offset_angle < -PI)
        offset_angle += (3.0/2.0) * PI;

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
            case 's':
                TelnetStream.println("STOP");
                targetSpeed = 0;
                rt_speed = 0;
                move_enable = false;  
                MotorControl.motorStop(1);
                MotorControl.motorStop(0);  
                break;
            case 'C':
                offset_angle = (data.substring(1).toFloat()  * PI) / 180.0;
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

        // if(faceSensorValue3 > 4000){ // NEARLY HIT THE WALL 
        //     if(targetSpeed != 0){
        //         TelnetStream.println("STOP");
        //         targetSpeed = 0;
        //         // move_enable = false;
        //         // rt_speed = -45;
        //     }
        // }

        // if(faceSensorValue3 > 1600 && !turning()){ // turn
        //     if(targetSpeed != 0){
        //         // TelnetStream.println("STOP");
        //         // fw_speed = 0;
        //         // move_enable = false;
        //         // rt_speed = -45;
        //         if(faceSensorValue4 < 2100){ // turn right
        //             TelnetStream.println("Turn right");
        //             turn(RIGHT);
        //         }
        //         // offset_angle += (PI/2.0);
        //     }
        // }
        // else if (faceSensorValue3 < 2000) {
        //     if(move_enable){
        //         fw_speed = 30;
        //     }
        //     //forward both wall
        //     if(faceSensorValue5 > 2500 && faceSensorValue1 > 2500){
        //     //     if(fw_speed != 0)
        //             rt_speed = (faceSensorValue1 - faceSensorValue5 - 0) / 100;
        //     }
        //     // no right wall and able to turn right
        //     else if(faceSensorValue5 < 2500){
        //         if(fw_speed != 0){
        //             if(rt_speed != 15){
        //                 TelnetStream.println("rotating right...");
        //                 rt_speed = 15;
        //             }
        //         }
        //     }
        // }



        target_angle = - offsetYaw(offset_angle)*E_ratio;
        
        if(abs(target_angle) < 10 && speedA + speedB > 4)
            rt_speed = 0;

        else if(abs(target_angle) > 70){
            if(target_angle>0)
                rt_speed = 70;
            else    
                rt_speed = -70;
        }
        else   
            rt_speed = target_angle;



        fw_speed = (fw_speed*7 + targetSpeed) / 8;
        // fw_speed = targetSpeed;

        
        PD_SPEED_ANGULAR(fw_speed, rt_speed, (speedA - speedB) * 2, (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(target_angle), 0, left, right);
        // PD_SPEED_ANGULAR(fw_speed, (long)(target_angle), (speedA - speedB) * 2, (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(ypr[0] * 5), (speedA + speedB) / 2, left, right);
    //     calculate_timer = millis();
    // }
        if(move_enable){
            if(left > 0){
                // left += 40;
                MotorControl.motorForward(1, left);
            }else{
                // left -= 40;
                MotorControl.motorReverse(1, -left);
            }
            
            if(right > 0){
                // right += 40;
                MotorControl.motorForward(0, right);
            }else{
                // right -= 40;
                MotorControl.motorReverse(0, -right);
            }
        }
    }
    

    if(millis() - LOG_timer > 500){
    //     TelnetStream.print(micros()-entry); 
    //     TelnetStream.print(" ");
    //     TelnetStream.print(speedA);
    //     TelnetStream.print(" ");
    //     TelnetStream.print(speedB);
    //     TelnetStream.print(" ");
    //     TelnetStream.print(encoderA.getCount());
    //     TelnetStream.print(" ");
    //     TelnetStream.println(encoderB.getCount());
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
        // TelnetStream.print(target_angle);
        // TelnetStream.print(" ");
        // TelnetStream.print(offset_angle * 180/M_PI);
        // TelnetStream.print(" ");
        // TelnetStream.print(ypr[0] * 180/M_PI);
        // TelnetStream.print(" ");
        // TelnetStream.print(offsetYaw(offset_angle)* 180/M_PI);
        if(debug == 1)
            printIR(TelnetStream);

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
    //     LOG_timer = millis();
    }



    #ifndef USE_ASYNC_ELEGANT_OTA
        ArduinoOTA.handle();
    #endif
    digitalWrite(2,HIGH);
}
#endif