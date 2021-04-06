#include <Arduino.h>
#include "DistanceMeter.h"
#include <SoftwareSerial.h>
#include <TFMPlus.h> 

DistanceMeter::DistanceMeter(SoftwareSerial& customSerialForTFMini)
    :   _customSerialForTFMini(customSerialForTFMini)
{
    _initialized = false;
}

void DistanceMeter::initialize() {

    if (!_initialized) {
        _lastReadMs = 0;

        _customSerialForTFMini.begin(115200);

        _tfmP.begin(&_customSerialForTFMini);    // Initialize device library object and...
                                            // pass device serial port to the object.

        // Send some example commands to the TFMini-Plus
    // - - Perform a system reset - - - - - - - - - - -
/*        Serial.print("System reset: ");
        if (_tfmP.sendCommand(SYSTEM_RESET, 0))
        {
            Serial.print("passed.\r\n");
        }
        else _tfmP.printReply();
*/
        delay(500);  // added to allow the System Rest enough time to complete
        _tfmP.sendCommand(SET_FRAME_RATE, FRAME_20);
        delay(500);            // And wait for half a second.

        _initialized = true;
    }
}

void DistanceMeter::handleDistanceMeter() {
    
    unsigned int loopDelay = 50;// Loop delay to match the 20Hz data frame rate
    
int16_t tfDist = 0;    // Distance to object in centimeters
int16_t tfFlux = 0;    // Strength or quality of return signal
int16_t tfTemp = 0;    // Internal temperature of Lidar sensor chip

    if (millis() - _lastReadMs > loopDelay) {

if( _tfmP.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
{
    Serial.print( "Dist:  ");   // display distance,
    Serial.print( tfDist);   // display distance,
    Serial.print( "Flux:  ");   // display signal strength/quality,
    Serial.print( tfFlux);   // display signal strength/quality,
    Serial.print( "Temp:  ");   // display temperature,
    Serial.println( tfTemp);   // display temperature,
}
else                  // If the command fails...
{
    _tfmP.printFrame();  // display the error and HEX dataa
}
/**/
        //_tfmP.getData(_currentMeterData.distanceToObjectCm, _currentMeterData.strengthOrQualityOfReturnSignal, _currentMeterData.temperatureInternalOfLidarSensorChip);
        //Serial.println(_currentMeterData.distanceToObjectCm);
        _lastReadMs = millis();
    }
    else if (millis() < _lastReadMs) {

if( _tfmP.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
{
    Serial.print( "Dist:  ");   // display distance,
    Serial.print( tfDist);   // display distance,
    Serial.print( "Flux:  ");   // display signal strength/quality,
    Serial.print( tfFlux);   // display signal strength/quality,
    Serial.print( "Temp:  ");   // display temperature,
    Serial.println( tfTemp);   // display temperature,
}
else                  // If the command fails...
{
    _tfmP.printFrame();  // display the error and HEX dataa
}
        _lastReadMs = millis();
    }
    

};