#include <Arduino.h>
#include "DistanceMeter.h"
#include <SoftwareSerial.h>
#include <TFMPlus.h> 

DistanceMeter::DistanceMeter(SoftwareSerial& customSerialForTFMini, DistanceMeterData& currentMeterData)
    :   _customSerialForTFMini(customSerialForTFMini),
        _currentMeterData(currentMeterData)
{
    _initialized = false;
}

void DistanceMeter::initialize() {

    if (!_initialized) {
        _lastReadMs = 0;

        _customSerialForTFMini.begin(115200);

        _tfmP.begin(&_customSerialForTFMini);    // Initialize device library object and...
                                            // pass device serial port to the object.

        delay(500);  // added to allow the System Rest enough time to complete
        _tfmP.sendCommand(SET_FRAME_RATE, FRAME_20);
        

        _initialized = true;
    }
}

void DistanceMeter::handleDistanceMeter() {
    
    unsigned int loopDelay = 50;// Loop delay to match the 20Hz data frame rate

    if (millis() - _lastReadMs > loopDelay) {
        _tfmP.getData(_currentMeterData.distanceToObjectCm, _currentMeterData.strengthOrQualityOfReturnSignal, _currentMeterData.temperatureInternalOfLidarSensorChip);

        _lastReadMs = millis();
    }
    else if (millis() < _lastReadMs) {

        _tfmP.getData(_currentMeterData.distanceToObjectCm, _currentMeterData.strengthOrQualityOfReturnSignal, _currentMeterData.temperatureInternalOfLidarSensorChip);

        _lastReadMs = millis();
    }
};
