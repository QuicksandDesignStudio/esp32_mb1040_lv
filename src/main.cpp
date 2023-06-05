#include <Arduino.h>
#include <FreeRTOSConfig.h>

#include "Globals.hpp"
#include "WebClient.hpp"
#include "ProximitySensor.hpp"

bool          printMode{false};
bool          networkTrackerActive{true};
WebClient     client{};
TaskHandle_t  networkTaskHandle;

// Array of sensors
ProximitySensor sensors[Sensing::NUMBER_OF_SENSORS];
bool            sensorStates[Sensing::NUMBER_OF_SENSORS];


void printSenorValues(long averagedValues[], std::size_t size)
{
  Serial.println("-----------------");
  for (std::size_t i=0; i< size; i++)
  {
    Serial.println(String(i) + " : " + String(averagedValues[i]));
  }
  Serial.println("-----------------");
}

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

    
    int8_t batchCounter = 0;
    while(batchCounter < Sensing::COLLECTION_BATCHES)
    {
      int8_t sampleCounter = 0;
      int8_t collectFrom = (batchCounter * Sensing::NUMBER_OF_SENSORS / Sensing::COLLECTION_BATCHES);
      int8_t collectUpto = (Sensing::NUMBER_OF_SENSORS / Sensing::COLLECTION_BATCHES)*(batchCounter + 1) - 1;
      
      while (sampleCounter < Sensing::NUMBER_OF_SAMPLES)
      {
        Sensing::activateSensingSequence(batchCounter);
        for (int8_t i=collectFrom; i<=collectUpto; i++)
        {
          sensorVals[i] += sensors[i].sense();
          delay(10);
        }
        sampleCounter++;
      }

      batchCounter ++;
    }

    for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
    {
      //take average
      sensorVals[i] = sensorVals[i] / Sensing::NUMBER_OF_SAMPLES; 
      //assign state
      currentSensorStates[i] = (sensorVals[i] <= Sensing::SENSING_THRESHOLD_IN_CM);
      //has it changed 
      if (currentSensorStates[i] != sensorStates[i])
      {
        hasChanged = true;
        sensorStates[i] = currentSensorStates[i];
      }
    }

    if(printMode) 
    {
      printSenorValues(sensorVals, sizeof(sensorVals)/sizeof(sensorVals[0]));
      continue;
    }
    
    if (!hasChanged)
    {
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
  for (int8_t i=0; i< Sensing::COLLECTION_BATCHES; i++)
  {
    pinMode(Sensing::TRIGGER_PINS[i], OUTPUT);
  }
  
  
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