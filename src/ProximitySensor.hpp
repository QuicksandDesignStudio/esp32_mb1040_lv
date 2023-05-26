#ifndef ProximitySensor_hpp
#define ProximitySensor_hpp

#include <Arduino.h>
#include "Globals.hpp"

class ProximitySensor {

private:

    String          m_sensorName;
    String          m_sensorType;
    int8_t          m_sensorPin;
    long            m_sensorValue;
    bool            m_stableSensorState{false};
    bool            m_previousSensorState{false};
    double          m_timeKeeper;

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

    bool getSensorState() {
        return m_stableSensorState;
    }

    long sense() {
        
        long rawSensorValue = pulseIn(m_sensorPin, HIGH);

        /*
            The scale factor is 147uS per inch
            The scale factor is 58uS per cm
        */

        m_sensorValue = static_cast<int>(rawSensorValue / 58); //in cm

        
        bool currentSensorState = false;

        if(m_sensorValue <= Sensing::SENSING_THRESHOLD_IN_CM)
            currentSensorState = true;
        
        if(currentSensorState != m_previousSensorState)
        {
            m_previousSensorState = currentSensorState;
            m_timeKeeper = Time();
        }
        else 
        {
            if(Time() - m_timeKeeper >= Sensing::SENSING_TIME_IN_SECONDS)
            {
                //state has been the same for long enough
                m_stableSensorState = currentSensorState;
                m_timeKeeper = Time();
            }
        }

        return m_sensorValue;
    }
};


#endif