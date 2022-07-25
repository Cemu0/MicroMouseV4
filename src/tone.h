#pragma once
#include "variable.h"

void initTone(){
  pinMode(BUZZ_PIN,OUTPUT);
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(BUZZ_PIN, 0); // attach beeper

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);
}

void buildInLedOn(){
    digitalWrite(LED_PIN,LOW);
}

void buildInLedOff(){
    digitalWrite(LED_PIN,HIGH);
}

void tone(int freq = 2000, unsigned long duration = 100) {
  ledcWriteTone(0, freq); // play tone
  if (duration != 0){
    delay(duration);
    ledcWriteTone(0, 0); // play tone
  }
}

void readyTone(){
    tone(1000,100);
    delay(200);
    tone(2000,100);
    delay(200);
    tone(3000,100);
}

void stopTone(){
    tone(1000,100);
    delay(1000);
    tone(1000,100);
    delay(1000);
    tone(1000,100);
}

void startTone(){
    tone(1000,100);
    delay(1000);
    tone(2000,100);
    delay(1000);
    tone(3000,100);
    delay(1000);
    tone(5000,100);

}

void successTone(){
    tone(3000,100);
    delay(200);
    tone(2000,100);
    delay(200);
    tone(3000,100);
    delay(500);
    tone(3000,100);
    delay(200);
    tone(2000,100);
    delay(200);
    tone(3000,100);
}