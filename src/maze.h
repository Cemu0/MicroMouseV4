#pragma once
#include <Arduino.h>

int64_t CellSize = 180; // mm
int64_t currentPosInCell = 55; // start at the first cell facing North

bool updated_next = false;
long checkSensorPos = 0; //position where get cell data ?

int64_t oldPos = -currentPosInCell;
int64_t getAbsPos(){
    return ((encoderA.getCount() + encoderB.getCount())/2) * MM_PP_A;
    // return encoderA.getCount() * MM_PP_A; 
}

void setCellPos(long newPos){
    oldPos = getAbsPos() - newPos;
    currentPosInCell = newPos;
}

void resetCell(){
    setCellPos(55);
}

bool updateCellPos(){
    
    //case move forward only

    currentPosInCell =  (getAbsPos() - oldPos);
    
        //update when enter new cell
    if(currentPosInCell >= CellSize){
        // currentPosInCell -= CellSize;
        // oldPos = pos;
        setCellPos(0);
        return true;
    }
    return false;
}



long cellX = 1;
long cellY = 1;

enum direction {
    north = 0,
    east = 1,
    south = 2,
    west = 3
};

int direction = north;

void mazeTurnRight(){
    direction++;
    // if(direction > 3 || direction < 0)
    direction = direction % 4;
}

void mazeTurnAround(){
    direction+=2;
    direction = direction % 4;
}

void mazeTurnLeft(){ //wrong
    direction--;
    if(direction < 0)
        direction = 3;
    direction = direction % 4;
}

void startPoint(){
    cellX = 1;
    cellY = 1;
    direction = north;
}


void moveForward(){
    switch (direction)
    {
    case north:
        cellX++;
        break;
    case south:
        cellX--;
        break;
    case east:
        cellY++;
        break;
    case west:
        cellY--;
        break;
    default:
        break;
    }
}