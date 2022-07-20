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
    oldPos = getAbsPos() + newPos;
    currentPosInCell = 0;
}

void resetCell(){
    setCellPos(-55);
}

bool updateCellPos(){
    
    //case move forward only

    currentPosInCell =  getAbsPos() - oldPos;
    
        //update when enter new cell
    if(currentPosInCell > CellSize){
        // currentPosInCell -= CellSize;
        // oldPos = pos;
        setCellPos(0);
        return true;
    }
    return false;
}



long cellX = 1;
long cellY = 1;

