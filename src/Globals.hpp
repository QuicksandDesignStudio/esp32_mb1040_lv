#ifndef Globals_hpp
#define Globals_hpp

#include <Arduino.h>

namespace Network
{
    constexpr const char*   DEVICE_NAME{"SB_0"};
    constexpr const char*   SOURCE_IDEN{"0"};
    constexpr const char*   ON{"on"};
    constexpr const char*   OFF{"off"};
    constexpr const char*   RASPBERRY_PI_SERVER_IP{"192.168.35.50"};   //direct IP so we don't have to resolve by DNS
    constexpr std::int16_t  PORT{5000};
    constexpr std::int8_t   CLIENT_TIME_OUT_IN_SECONDS{3};
    constexpr std::int8_t   MAX_TRIES{5};
    constexpr const int16_t SENSOR_SEND_MESSAGE_TRIES_DELAY_MILISECONDS{500};
    static bool             networkSetup{false};
}

namespace Demo 
{
    constexpr const int8_t  DEMO_TIME_IN_SECONDS{10};
}

namespace Sensing
{
    constexpr const long    SENSING_THRESHOLD_IN_CM{30};
    constexpr const double  SENSING_TIME_IN_SECONDS{0.5};
    constexpr const int8_t  NUMBER_OF_SENSORS{1};
    constexpr const int8_t  SENSORPINS[NUMBER_OF_SENSORS]{27};
    constexpr const int16_t SENSE_DELAY_IN_MILLISECONDS{500};
    String                  SENSOR_TYPE{"mb1040_lv"};
}

#endif