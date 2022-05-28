#ifdef REMOTE_CONTROL
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

long speedA = 0;
long speedB = 0;

int64_t oldCountA = 0;
int64_t oldCountB = 0;

unsigned long speed_timer = 0;
void speedCalculate(){
    auto detatime = millis() - speed_timer;
    if( detatime > 10){
        speedA = ((encoderA.getCount() - oldCountA) + speedA*3)/4;
        speedB = ((encoderB.getCount() - oldCountB) + speedB*3)/4;
        oldCountA = encoderA.getCount();
        oldCountB = encoderB.getCount();
        speed_timer = millis();
    }
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
}

long fw_speed = 0;
long rt_speed = 0;
long left;
long right;
unsigned long calculate_timer = 0;
unsigned long LOG_timer = 0;
void loop(){
    digitalWrite(2,LOW);
    entry = micros();
    switch (TelnetStream.read()) {
        case 'R':
            TelnetStream.stop();
            delay(100);
            ESP.restart();
            break;
        case 'C':
            TelnetStream.println("bye bye");
            TelnetStream.stop();
            break;
    }
    // Your code here
    ws.cleanupClients();
    readGyro();
    speedCalculate();
    readIRsensor();


    if(abs(Yvalue) < 25)
        Yvalue = 0; 

    if(abs(Xvalue) < 25)
        Xvalue = 0; 

    fw_speed = Yvalue/2;
    rt_speed = Xvalue/2;

    //run every millisecond
    if(millis() != calculate_timer){

        // if(faceSensorValue3 > 3000){
        //     fw_speed = 0;
        // }

        // if(faceSensorValue5 > 3000)
        //     if(fw_speed != 0)
        //         rt_speed = -10; 
        
        // if(faceSensorValue1 > 3000)
        //     if(fw_speed != 0)
        //         rt_speed = 10;


        PD_SPEED_ANGULAR(fw_speed, rt_speed, (speedA - speedB) * 2, (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(ypr[0] * 150), (speedA + speedB) / 2, left, right);
        // PD_SPEED_ANGULAR(fw_speed, rt_speed, (long)(ypr[0] * 5), (speedA + speedB) / 2, left, right);
        calculate_timer = millis();
    }

    if(left > 0)
        MotorControl.motorForward(1, left);
    else
        MotorControl.motorReverse(1, -left);
    
    if(right > 0)
        MotorControl.motorForward(0, right);
    else
        MotorControl.motorReverse(0, -right);

    if(millis() - LOG_timer > 500){
        TelnetStream.print(micros()-entry); 
        TelnetStream.print(" ");
        TelnetStream.print(speedA);
        TelnetStream.print(" ");
        TelnetStream.print(speedB);
        TelnetStream.print(" ");
        TelnetStream.print(left);
        TelnetStream.print(" ");
        TelnetStream.print(right);
        TelnetStream.print(" ");
        TelnetStream.print(ypr[0] * 180/M_PI);
        printIR(TelnetStream);

        Serial.print(" ");
        Serial.print(speedA);
        Serial.print(" ");
        Serial.print(speedB);
        Serial.print(" ");
        Serial.print(left);
        Serial.print(" ");
        Serial.print(right);
        Serial.print(" ");
        Serial.print(ypr[0] * 180/M_PI);
        printIR(Serial);

        // printIR(Serial);
        LOG_timer = millis();
    }


    #ifndef USE_ASYNC_ELEGANT_OTA
        ArduinoOTA.handle();
    #endif
    digitalWrite(2,HIGH);
}
#endif