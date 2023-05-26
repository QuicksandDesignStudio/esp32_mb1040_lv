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
    constexpr const int8_t  TRIGGET_PIN{5};
    constexpr const int8_t  TRIGGER_DURATION_MICROSECONDS{30};
    constexpr const int8_t  SENSING_CYCLE_IN_MILLISECONDS{50};
    constexpr const long    SENSING_THRESHOLD_IN_CM{20};
    constexpr const double  SENSING_TIME_IN_SECONDS{1};
    constexpr const int8_t  NUMBER_OF_SENSORS{6};
    constexpr const int8_t  SENSORPINS[NUMBER_OF_SENSORS]{12,13,14,25,26,27};
    constexpr const int16_t SENSE_DELAY_IN_MILLISECONDS{500};
    String                  SENSOR_TYPE{"mb1040_lv"};
    struct SensorResponse
    {
        long sensorValue;
        bool isTriggered;
    };
}

// Black box function from Mr. Dave Plumber himself returns time of day
static double Time()
{
    timeval tv = {0};
    gettimeofday(&tv, nullptr);
    return (double)(tv.tv_usec / 1000000.0 + (double) tv.tv_sec);
}

#endif