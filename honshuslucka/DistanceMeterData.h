#ifndef DISTANCE_METER_DATA_H
#define DISTANCE_METER_DATA_H

struct DistanceMeterData {
    int16_t distanceToObjectCm = 0;
    int16_t strengthOrQualityOfReturnSignal = 0;
    int16_t temperatureInternalOfLidarSensorChip = 0;
    String errorAndHexData = "";
};

#endif