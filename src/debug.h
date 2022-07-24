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
                time_accelerate = millis();
                move_enable = true;
                break;
            case 'S':
            case 's':
                TelnetStream.println("STOP");
                stopMove();
                break;
            case 'C':
                offset_angle = TO_RADIAN(data.substring(1).toFloat());
                TelnetStream.print(offset_angle);
                TelnetStream.println("self rotating mode...");
                break;

            case 'B':
                TelnetStream.println("Test Algorithm");
                resetCell();
                startPoint();
                offset_angle = ypr[0];
                targetSpeed = data.substring(1).toInt();
                time_accelerate = millis();
                move_enable = true;
                break;

            case 'T':
                TelnetStream.println("Test Encoder Turn");
                encoderTurn(data.substring(1).toInt());
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
                P_temp = 0;
                TelnetStream.println("P_temp");
                break;

            case 'd':
                D_rotation = data.substring(1).toFloat();
                D_temp = 0;
                TelnetStream.println("D_temp");
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

            case 'g':
                turnTime = data.substring(1).toFloat();
                TelnetStream.println("turnTime");
                break;

            case 'h':
                turnSpeed = data.substring(1).toFloat();
                TelnetStream.println("turnSpeed");
                break;

            case 'k':
                turnAccelerate = data.substring(1).toFloat();
                TelnetStream.println("turnAccelerate");
                break;

            case 'j':
                startPosAfterTurn = data.substring(1).toFloat();
                TelnetStream.println("startPosAfterTurn");
                break;

            case 'v':
                whellDiameter = data.substring(1).toFloat();
                TelnetStream.println("whellDiameter");
                break;

            case 'o':
                forwardWallRatioP = data.substring(1).toFloat();
                TelnetStream.println("forwardWallRatioP");
                break;

            case 'O':
                turnPosition = data.substring(1).toFloat();
                TelnetStream.println("turnPosition");
                break;

            case 'i':
                startPosAfterTurnAround = data.substring(1).toInt();
                TelnetStream.println("startPosAfterTurnAround");
                break;

            case 'L':
                leftMiddleValue = data.substring(1).toInt();
                TelnetStream.println("leftMiddleValue");
                break;
                
            case 'V':
                rightMiddleValue = data.substring(1).toInt();
                TelnetStream.println("rightMiddleValue");
                break;

            case 'y':
                TelnetStream.print(baterryVoltage());
                TelnetStream.println("vol");
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
        if(debug == 4){
            TelnetStream.print(" ");
            TelnetStream.print(rotateErrorP);
            TelnetStream.print(" ");
            TelnetStream.print(fw_speed);
            TelnetStream.print(" ");
            TelnetStream.println(rt_speed);
            
        }

        if(debug == 5){
            TelnetStream.print(micros()-entry); 
            printIR(TelnetStream);
        }

        if(debug == 6){
            TelnetStream.print("cPos: ");
            TelnetStream.println(currentPosInCell); 
        }

        if(debug == 7){
            TelnetStream.println(rotateErrorP);
        }

        if(debug == 10){
            yield();
            TelnetStream.print("FROZEN SYSTEM for upload!!! ");
            TelnetStream.stop();
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
        // gyroTurn(Xvalue/100);
        move_enable = true;
        TelnetStream.print(" ");
        TelnetStream.print(fw_speed);
        TelnetStream.print(" ");
        TelnetStream.println(rt_speed);
    }
}