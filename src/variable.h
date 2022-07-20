
#pragma once
#include <Arduino.h>
#include "system_def.h"

const char* ssid = "mouse_4";
const char* password = "micromouse";

unsigned long entry;


bool move_enable = false;

long targetSpeed = 0;

float speedA = 0;
float speedB = 0;

int64_t oldCountA = 0;
int64_t oldCountB = 0;

float offset_angle = 0;
float E_ratio = 700;

// unsigned long speed_timer = 0;
unsigned long acc_timer = 0;
unsigned long math_timer = 0;

float fw_speed = 0;
float rt_speed = 0;
long left_pwm;
long right_pwm;
float target_angle;
float speed;

byte debug = 0;

#define RIGHT PI/2.0
#define LEFT -PI/2.0
#define BACKWARD PI

#define TO_RADIAN(n) (n  * PI) / 180.0
#define TO_DEG(n) n * (180/PI)

#define TURNING_TIME 9000

unsigned long lastTurn = 0;

unsigned long calculate_timer = 0;
unsigned long LOG_timer = 0;

// MPU control/status vars

float ypr[3];   
float delta;
