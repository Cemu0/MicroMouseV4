#pragma once

template <typename T> void limitAbs(T& value, T limit){
    if(value > limit)
        value = limit;
    if(value < -limit)
        value = -limit;

    return;
}