long CellSize = 180; // mm
long currentPosInCell = 55; // start at the first cell facing North

bool updated_next = false;
long checkSensorPos = 0; //position where get cell data ?

long oldPos = -currentPosInCell;

void resetCell(){
    long pos = ((encoderA.getCount() + encoderB.getCount())/2) * MM_PP_A;

    oldPos = pos - 55;
}

bool updateCellPos(){
    
    //case move forward
    long pos = ((encoderA.getCount() + encoderB.getCount())/2) * MM_PP_A;

    currentPosInCell =  pos - oldPos;
    
        //update when enter new cell
    if(currentPosInCell > CellSize){
        currentPosInCell -= CellSize;
        oldPos = pos;
        return true;
    }
    return false;
}

void turnAble(){

}