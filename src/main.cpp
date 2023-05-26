#include <Arduino.h>
#include <FreeRTOSConfig.h>

#include "Globals.hpp"
#include "WebClient.hpp"
#include "ProximitySensor.hpp"



bool          networkTrackerActive{true};
WebClient     client{};
TaskHandle_t  networkTaskHandle;

// Array of sensors
ProximitySensor sensors[Sensing::NUMBER_OF_SENSORS];
bool sensorStates[Sensing::NUMBER_OF_SENSORS];



void sensingTask(void *param) 
{
  // Setup the array of sensors
  for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
  {
    String name = String(i);
    sensors[i] = ProximitySensor(name, Sensing::SENSOR_TYPE, Sensing::SENSORPINS[i]);
    sensorStates[i] = false;
  }

  while (true) {

    /* BEGIN SENSOR CYCLE  */
    //trigger the first sensor
    digitalWrite(Sensing::TRIGGET_PIN, HIGH);
    delayMicroseconds(Sensing::TRIGGER_DURATION_MICROSECONDS);
    digitalWrite(Sensing::TRIGGET_PIN, LOW);

    bool currentSensorStates[Sensing::NUMBER_OF_SENSORS];

    for (std::size_t i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
    {
      sensors[i].sense();
      currentSensorStates[i] = sensors[i].getSensorState();
      
      //allow the sensor to finish the sensing cycle
      delay(Sensing::SENSE_DELAY_IN_MILLISECONDS);
    }
    /* END OF SENSOR CYCLE */

    // Send message
    for (std::size_t i=0; i< Sensing::NUMBER_OF_SENSORS; i++)
    {
      if (currentSensorStates[i] != sensorStates[i]) {
        // Send message
        String message = sensors[i].getSensorName() + ":" + (sensors[i].getSensorState() ? Network::ON : Network::OFF);
        Serial.println("Sending message : " + message);

        // Send socket message
        bool success = client.sendMessage(message, Network::RASPBERRY_PI_SERVER_IP, Network::PORT, Network::MAX_TRIES);

        if (success) {
          sensorStates[i] = sensors[i].getSensorState();
        }
        else {
          Serial.println("Failed to send message");
        }
      }
    }
    delay(Sensing::SENSE_DELAY_IN_MILLISECONDS);
  }
}

void networkTask(void *param)
{
  while (true) {
    if (Network::networkSetup) {
      //start sensing
      xTaskCreatePinnedToCore(
        sensingTask,        //Task Function
        "sendingTask",      // Task Name
        10000,              // Stack Size
        NULL,               // Task Parameters
        1,                  // Priority 
        NULL,               // Task Handle
        1                   // Core
      );
      

      // Stop and delete this task
      vTaskSuspend(networkTaskHandle);  
      vTaskDelete(networkTaskHandle); // Delete this task
    }
    delay(10);
  }
}

void setup() 
{
  pinMode(Sensing::TRIGGET_PIN, OUTPUT);
  Serial.begin(115200);
  client.startClient(&Serial);
  
  xTaskCreatePinnedToCore(
    networkTask,       //Task Function
    "setupNetwork",     // Task Name
    8192,               // Stack Size
    NULL,               // Task Parameters
    1,                  // Priority 
    &networkTaskHandle, // Task Handle
    0                   // Core
  );

}

void loop(){}