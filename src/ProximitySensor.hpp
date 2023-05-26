#ifndef ProximitySensor_hpp
#define ProximitySensor_hpp

#include <Arduino.h>
#include "Globals.hpp"

class ProximitySensor {

private:

    String          m_sensorName;
    String          m_sensorType;
    int8_t          m_sensorPin;

public:
    ProximitySensor() = default;  // Default constructor

    ProximitySensor(String sensorName, String sensorType, int8_t sensorPin)
        :m_sensorName{sensorName}, m_sensorType{sensorType}, m_sensorPin{sensorPin}
    {
        pinMode(m_sensorPin, INPUT);
    }

    String getSensorName() {
        return m_sensorName;
    }


    Sensing::SensorResponse sense() {
        long rawSensorValue = pulseIn(m_sensorPin, HIGH);
        /*
            The scale factor is 147uS per inch
            The scale factor is 58uS per cm
        */
        bool currentSensorState = (static_cast<int>(rawSensorValue / 58) <= Sensing::SENSING_THRESHOLD_IN_CM);
        return Sensing::SensorResponse{rawSensorValue, currentSensorState};
    }
};


#endif