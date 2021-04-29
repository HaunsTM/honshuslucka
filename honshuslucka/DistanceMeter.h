#include <Arduino.h>
#include "DistanceMeterData.h"
#include <SoftwareSerial.h>
#include <TFMPlus.h> 

#ifndef DISTANCE_METER_H
#define DISTANCE_METER_H

class DistanceMeter {

public:
    DistanceMeter(SoftwareSerial& customSerialForTFMini, DistanceMeterData& currentMeterData);

    void initialize();
    bool isConnected();
    void handleDistanceMeter();

private:
    DistanceMeterData& _currentMeterData;
    DistanceMeterData _previousMeterData;
    SoftwareSerial& _customSerialForTFMini;
    TFMPlus _tfmP;

    bool _initialized;
    bool _isConnected;
    unsigned long _lastReadMs;    
};

#endif