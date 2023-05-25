#include <array>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#include "Globals.hpp"
#include "WebClient.hpp"
#include "ProximitySensor.hpp"

#define sensorPin 27

bool          networkTrackerActive{true};
bool          demoMode{false};    // Send socket message on and off every 5 seconds
bool          demoState{true};   // On or Off

WebClient     client{};

TaskHandle_t  networkTaskHandle;
TaskHandle_t  demoTaskHandle;
TaskHandle_t  sensingTaskHandle;

//array of sensors
std::array<ProximitySensor, Sensing::NUMBER_OF_SENSORS> sensors;
std::array<bool, Sensing::NUMBER_OF_SENSORS> sensorStates;


void sensingTask(void *param) 
{
  //setup the array of sensors
  for(std::size_t i=0; i<Sensing::NUMBER_OF_SENSORS; i++)
  {
    String name = String(i);
    sensors[i] = ProximitySensor(name, Sensing::SENSOR_TYPE, Sensing::SENSORPINS[i]);
    sensorStates[i] = false;
  }

  while (true) {

    for(std::size_t i=0; i<Sensing::NUMBER_OF_SENSORS; i++)
    {
      sensors[i].sense();
      
      if(sensors[i].getSensorState() != sensorStates[i]) {
        //send message
        String message = sensors[i].getSensorName() + ":" + (sensors[i].getSensorState() ? Network::ON : Network::OFF);
        Serial.println("Sending message : " + message);

        //send socket message
        bool success = client.sendMessage(message, Network::RASPBERRY_PI_SERVER_IP, Network::PORT, Network::MAX_TRIES);

        if (success) {
          demoState = !demoState;
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

void demoTask(void *param)
{
  while (true) {
    String message = demoState ? (String(Network::SOURCE_IDEN) + ":" + String(Network::ON)) : (String(Network::SOURCE_IDEN) + ":" + String(Network::OFF));
    Serial.println("Sending message : " + message);
    
    //send socket message
    bool success = client.sendMessage(message, Network::RASPBERRY_PI_SERVER_IP, Network::PORT, Network::MAX_TRIES);

    if (success) {
      demoState = !demoState;
    }
    else {
      Serial.println("Failed to send message");
    }

    delay(Demo::DEMO_TIME_IN_SECONDS * 1000);
  }
}

void networkTask(void *param)
{
  while (true) {
    if (Network::networkSetup) {
      //is it in demo mode?
      if (demoMode) {
        //start demo
        xTaskCreatePinnedToCore(
          demoTask,         //Task Function
          "demoTask",       // Task Name
          8192,             // Stack Size
          NULL,             // Task Parameters
          1,                // Priority 
          &demoTaskHandle,  // Task Handle
          0                 // Core
        );
      }
      else {
        //start sensing
        xTaskCreatePinnedToCore(
          sensingTask,        //Task Function
          "sendingTask",      // Task Name
          8192,               // Stack Size
          NULL,               // Task Parameters
          1,                  // Priority 
          &sensingTaskHandle, // Task Handle
          0                   // Core
        );
      }

      // Stop and delete this task
      vTaskSuspend(networkTaskHandle);  
      vTaskDelete(networkTaskHandle); // Delete this task
    }
    delay(10);
  }
}

void setup() 
{
  //pinMode(sensorPin, INPUT);
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