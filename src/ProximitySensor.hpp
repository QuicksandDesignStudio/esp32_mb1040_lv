/*
Author          : Romit Raj
Last Modified   : 05/06/2023 (5th June)
Description     : ProximitySensor class
Repo            : https://github.com/QuicksandDesignStudio/esp32_mb1040_lv
*/

#ifndef ProximitySensor_hpp
#define ProximitySensor_hpp

#include <algorithm>
#include <Arduino.h>
#include "Globals.hpp"


class ProximitySensor {

private:

    String          m_sensorName;
    String          m_sensorType;
    int8_t          m_sensorPin;
    int8_t          m_sensor_scale_factor{58};

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


    long sense() {
        long rawSensorValue = pulseIn(m_sensorPin, HIGH, Sensing::SENSOR_TIME_OUT_IN_MICROSECONDS);
        /*
            The scale factor is 147uS per inch
            The scale factor is 58uS per cm
        */
        return max(Sensing::SENSING_MINIMUM_IN_CM, static_cast<long>(rawSensorValue / m_sensor_scale_factor));
        //return static_cast<long>(rawSensorValue / m_sensor_scale_factor);
    }
};


#endif