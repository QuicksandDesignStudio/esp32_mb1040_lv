#ifndef Globals_hpp
#define Globals_hpp

#include <Arduino.h>

namespace Network
{
    constexpr const char*   ON{"on"};                                   //on and off to communicate the proximity
    constexpr const char*   OFF{"off"};                                 //on and off to communicate the proximity
    constexpr const char*   RASPBERRY_PI_SERVER_IP{"192.168.35.50"};    //direct IP so we don't have to resolve by DNS
    constexpr std::int16_t  PORT{5000};                                 //the socket server port
    constexpr std::int8_t   CLIENT_TIME_OUT_IN_SECONDS{3};              //the socket client timeout in seconds
    constexpr std::int8_t   MAX_TRIES{5};                               //the maximum number of tries to send a message
    static bool             networkSetup{false};                        //flag to indicate if the network has been setup
}

namespace Sensing
{
    constexpr const int8_t  COLLECTION_BATCHES{3};                                  //number of batches of sensors
    constexpr const int8_t  TRIGGER_PINS[COLLECTION_BATCHES]{27, 5, 19};            //trigger pins for the sensor sets
    constexpr const int8_t  TRIGGER_DURATION_MICROSECONDS{30};                      //duration of the trigger pulse
    constexpr const long    SENSING_THRESHOLD_IN_CM{30};                            //threshold for the sensor to be considered on
    constexpr const long    SENSING_MINIMUM_IN_CM{20};                              //minimum distance for the sensor to be considered. Anything below this is clamped to this value
    constexpr const int8_t  NUMBER_OF_SENSORS{6};                                   //number of sensors
    constexpr const int8_t  SENSORPINS[NUMBER_OF_SENSORS]{26, 25, 33, 32, 35, 34};  //sensor pins that recieve PW data
    constexpr const long    SENSOR_TIME_OUT_IN_MICROSECONDS{200000};                //timeout for the sensor reading. Beyond this a 0 values is returned
    
    String                  SENSOR_TYPE{"mb1040_lv"};                               //sensor type
    constexpr const int8_t  NUMBER_OF_SAMPLES{5};                                   //number of samples to take for each sensor

    // Trigger the sensor set at the given index
    // Each set has 2 sensors in this case with a total of 3 sets
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