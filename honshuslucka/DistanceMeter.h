#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TFMPlus.h> 

#ifndef DISTANCE_METER_H
#define DISTANCE_METER_H

class DistanceMeter {

public:
    DistanceMeter(SoftwareSerial& customSerialForTFMini);

    void initialize();
    void handleDistanceMeter();

private:
    SoftwareSerial& _customSerialForTFMini;
    TFMPlus _tfmP;

    bool _initialized;
    unsigned long _lastReadMs;    
};

#endif