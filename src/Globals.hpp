#ifndef Globals_hpp
#define Globals_hpp

#include <Arduino.h>

namespace Network
{
    constexpr const char*   ON{"on"};
    constexpr const char*   OFF{"off"};
    constexpr const char*   RASPBERRY_PI_SERVER_IP{"192.168.35.50"};   //direct IP so we don't have to resolve by DNS
    constexpr std::int16_t  PORT{5000};
    constexpr std::int8_t   CLIENT_TIME_OUT_IN_SECONDS{3};
    constexpr std::int8_t   MAX_TRIES{5};
    static bool             networkSetup{false};
}

namespace Sensing
{
    constexpr const int8_t  COLLECTION_BATCHES{3};
    constexpr const int8_t  TRIGGER_PINS[COLLECTION_BATCHES]{27, 5, 19};
    constexpr const int8_t  TRIGGER_DURATION_MICROSECONDS{30};
    constexpr const long    SENSING_THRESHOLD_IN_CM{30};
    constexpr const long    SENSING_MINIMUM_IN_CM{20};
    constexpr const int8_t  NUMBER_OF_SENSORS{6};
    constexpr const int8_t  SENSORPINS[NUMBER_OF_SENSORS]{26, 25, 33, 32, 35, 34};
    constexpr const long    SENSOR_TIME_OUT_IN_MICROSECONDS{200000};
    
    String                  SENSOR_TYPE{"mb1040_lv"};
    constexpr const int8_t  NUMBER_OF_SAMPLES{5};

    void activateSensingSequence(int8_t index)
    {
        digitalWrite(TRIGGER_PINS[index], HIGH);
        delayMicroseconds(TRIGGER_DURATION_MICROSECONDS);
        digitalWrite(TRIGGER_PINS[index], LOW);
    }
}

// Black box function from Mr. Dave Plumber himself returns time of day
static double Time()
{
    timeval tv = {0};
    gettimeofday(&tv, nullptr);
    return (double)(tv.tv_usec / 1000000.0 + (double) tv.tv_sec);
}

#endif