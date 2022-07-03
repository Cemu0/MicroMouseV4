#pragma once

void initDebug(){
    Serial.begin(115200);
    Serial.println("Connecting ... ");
    connectWifi(ssid,password);
    setupServer();
    Serial.println("HTTP server started");
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void checkRunningOnCore(){
    Serial.print("setup running on core");
    Serial.println(xPortGetCoreID());
}

void telnetStreamHandle(){
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
                TelnetStream.print(offset_angle);
                TelnetStream.println("offset_angle...");
                break;

            case 'P':
                P_speed = data.substring(1).toFloat();
                errorP_speed = 0;
                TelnetStream.println("P_speed");
                break;

            case 'D':
                D_speed = data.substring(1).toFloat();
                errorD_speed = 0;
                TelnetStream.println("D_speed");
                break;

            case 'p':
                P_rotation = data.substring(1).toFloat();
                errorP_rotation = 0;
                TelnetStream.println("P_rotation");
                break;

            case 'd':
                D_rotation = data.substring(1).toFloat();
                errorD_rotation = 0;
                TelnetStream.println("D_rotation");
                break;

            case 'E':
                E_ratio = data.substring(1).toFloat();
                oldErrorP_rotation = 0;
                TelnetStream.println("E_ratio");
                break;

            case 'a':
                a_ratio = data.substring(1).toFloat();
                TelnetStream.println("a_ratio");
                break;

            case 'o':
                PD_LOOP_TIME = data.substring(1).toInt();
                TelnetStream.println("PD_LOOP_TIME");
                break;

            case 'i':
                SPEED_UPDATE_LOOP_TIME = data.substring(1).toInt();
                TelnetStream.println("SPEED_UPDATE_LOOP_TIME");
                break;

            case 'F':
                FILTER_VALUE = data.substring(1).toInt();
                TelnetStream.println("FILTER_VALUE");
                break;

            case 'm':
                speed = data.substring(1).toFloat();
                move_enable = true;  
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
}
/*
WARNING: USING THIS WILL COST LOT OF CPU !!! be aware 
*/

void debugHandle(){
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

        if(debug == 2){
            TelnetStream.print(" ");
            TelnetStream.print(left_pwm);
            TelnetStream.print(" ");
            TelnetStream.println(right_pwm);
        }
        if(debug == 3){
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
        if(debug == 10){
            yield();
            TelnetStream.print("FROZEN SYSTEM for upload!!! ");
            vTaskDelete(mpuTask);
            delay(100000);
        }
        LOG_timer = millis();
    }
}

void remoteControl(){
        //remote contorl
    if(Yvalue == 0 && Xvalue == 0){
        fw_speed = 0;
        rt_speed = 0;
        move_enable = false;
    }
    if(abs(Yvalue) > 25 || abs(Xvalue) > 25){
        fw_speed = Yvalue * 5;
        rt_speed = Xvalue * 5;
        // turn(Xvalue/100);
        move_enable = true;
        TelnetStream.print(" ");
        TelnetStream.print(fw_speed);
        TelnetStream.print(" ");
        TelnetStream.println(rt_speed);
    }
}