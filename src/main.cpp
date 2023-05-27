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
bool            sensorStates[Sensing::NUMBER_OF_SENSORS];

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
    Serial.println("Begin Trigger Cycle");
   
    long sensorVals[Sensing::NUMBER_OF_SENSORS] = {0};
    bool currentSensorStates[Sensing::NUMBER_OF_SENSORS];
    bool hasChanged = false; 
    int8_t sampleCounter = 0;

    //collect samples
    while (sampleCounter < Sensing::NUMBER_OF_SAMPLES) {

      //trigger first sensor
      //rest are triggered via the tx to rx connections 
      digitalWrite(Sensing::TRIGGET_PIN, HIGH);
      delayMicroseconds(Sensing::TRIGGER_DURATION_MICROSECONDS);
      digitalWrite(Sensing::TRIGGET_PIN, LOW);
      
      //collect samples
      for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
      {
        sensorVals[i] += sensors[i].sense();
      }
      sampleCounter++;
    }
    
    //average samples
    //assign states
    //determine if there is a change
    for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
    {
      sensorVals[i] = sensorVals[i] / 5;  //simple average

      //assign state
      currentSensorStates[i] = (sensorVals[i] <= Sensing::SENSING_THRESHOLD_IN_CM);
      
      //has there been a change
      if (currentSensorStates[i] != sensorStates[i])
      {
        hasChanged = true;
        sensorStates[i] = currentSensorStates[i];
      }
    }

    //if there is no change don't send a message
    if(!hasChanged){
      Serial.println("No Change");
      continue;
    }

    /* ------------------SEND MESSAGE------------- */
    
    String message = "";
    // Create Message
    for (std::size_t i=0; i< Sensing::NUMBER_OF_SENSORS; i++)
    { 
      String sensorState = currentSensorStates[i] ? Network::ON : Network::OFF;
      message = message + sensorState + ":";
    }

    //Remove the last colon
    message.remove(message.length() - 1);

    //Send message
    Serial.println("Sending Message : " + message);
    bool success = client.sendMessage(message, Network::RASPBERRY_PI_SERVER_IP, Network::PORT, Network::MAX_TRIES);
    if(!success) {
      Serial.println("Failed to send message");
    }
    /* ------------------------------------------- */
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