/*
Author          : Romit Raj
Last Modified   : 05/06/2023 (5th June)
Description     : The main sequencer
Repo            : https://github.com/QuicksandDesignStudio/esp32_mb1040_lv
*/

#include <Arduino.h>
#include <FreeRTOSConfig.h>

#include "Globals.hpp"
#include "WebClient.hpp"
#include "ProximitySensor.hpp"

bool          printMode{false};                           // Set to true to print sensor values to serial monitor instead of sending socket messages
bool          networkTrackerActive{true};                 // Is network tracker active. Do not edit.
WebClient     client{};                                   // The web client object
TaskHandle_t  networkTaskHandle;                          // The network task handle. Declared so the task can be deleted once the network is connected.

// Array of sensors
ProximitySensor sensors[Sensing::NUMBER_OF_SENSORS];      // Array of sensors
bool            sensorStates[Sensing::NUMBER_OF_SENSORS]; // Array of sensor states


// Print out the averaged sensor values to serial monitor
// Turn on printMode to use this function
void printSenorValues(long averagedValues[], std::size_t size)
{
  Serial.println("-----------------");
  for (std::size_t i=0; i< size; i++)
  {
    Serial.println(String(i) + " : " + String(averagedValues[i]));
  }
  Serial.println("-----------------");
}

// The sensing task pinned to core 1 of a 2 core esp32
void sensingTask(void *param) 
{
  // Setup the array of sensors
  for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
  {
    // This is kinda useless at the moment but be relevant later
    String name = String(i);                                                            
    
    // Create the sensor object
    sensors[i] = ProximitySensor(name, Sensing::SENSOR_TYPE, Sensing::SENSORPINS[i]);   
    
    // Set the initial state of the sensor to false
    sensorStates[i] = false;                                                            
  }

  while (true) {

    // Array to store cumulative sensor values
    long sensorVals[Sensing::NUMBER_OF_SENSORS] = {0};
    
    // Array to store current sensor states
    bool currentSensorStates[Sensing::NUMBER_OF_SENSORS];
    
    // Flag to indicate if any of the sensor states have changed
    bool hasChanged = false; 

    /*-------------------------------------------------------------------------------------------------------------*/
    /* Collect sensor data by batches. */
    int8_t batchCounter = 0;

    // Loop through all the batches
    while(batchCounter < Sensing::COLLECTION_BATCHES)
    {
      // Counter to keep track of number of samples collected
      int8_t sampleCounter = 0;
      
      // Calculate the range of sensors to collect data from
      int8_t collectFrom = (batchCounter * Sensing::NUMBER_OF_SENSORS / Sensing::COLLECTION_BATCHES);
      int8_t collectUpto = (Sensing::NUMBER_OF_SENSORS / Sensing::COLLECTION_BATCHES)*(batchCounter + 1) - 1;
      
      // Collect samples from the sensors in the range
      while (sampleCounter < Sensing::NUMBER_OF_SAMPLES)
      {
        // Trigger the first sensor in this batch
        // This will read from the first sensor and trigger the next sensor in the batch
        Sensing::activateSensingSequence(batchCounter);

        // collect distance data from the sensorts in the batch
        for (int8_t i=collectFrom; i<=collectUpto; i++)
        {
          // Add the sensor value to get the cumulative value
          sensorVals[i] += sensors[i].sense();
          //delay(10);
        }

        // Increment the sample counter
        sampleCounter++;
      }

      // Increment the batch counter
      batchCounter ++;
    }
    /*-------------------------------------------------------------------------------------------------------------*/
    
    /*-------------------------------------------------------------------------------------------------------------*/
    /* Take simple average of the sensor values and set states based on the comparision of these values to the sensing threshold */
    for (int i = 0; i < Sensing::NUMBER_OF_SENSORS; i++)
    {
      //take average
      //simple average - should be good enough but can be improved to moving average if needed
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
    /*-------------------------------------------------------------------------------------------------------------*/

    // Print the sensor values to serial monitor if printMode is true
    if(printMode) 
    {
      printSenorValues(sensorVals, sizeof(sensorVals)/sizeof(sensorVals[0]));
      continue;
    }

    // If nothing has changed, continue
    if (!hasChanged)
    {
      //Serial.println("No Change");
      continue;
    }

    /*-------------------------------------------------------------------------------------------------------------*/         
    /* Send a socket if any of the sensor states have changed */
    String message = "";
    
    // Create Message
    // The message format is state:state:state:state:state
    // This can be changed to match whatever format the socket server is expecting
    for (std::size_t i=0; i< Sensing::NUMBER_OF_SENSORS; i++)
    { 
      String sensorState = currentSensorStates[i] ? Network::ON : Network::OFF;
      message = message + sensorState + ":";
    }

    //Remove the last colon
    message.remove(message.length() - 1);

    //Send message
    //Serial.println("Sending Message : " + message);
    
    // Sending will be tried MAX_TRIES times
    bool success = client.sendMessage(message, Network::RASPBERRY_PI_SERVER_IP, Network::PORT, Network::MAX_TRIES);
    
    /*
    if(!success) {
      Serial.println("Failed to send message");
    }*/

    /* ------------------------------------------- */
  }
}

// Network Task checks if the network is connected, which is monitored by the Network::networkSetup flag
// This static flag is set to true by the WebClient class
// Once the network is connected, the sensing task is started and the network task is deleted
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
  // Set the trigger pins for the sensor states to output
  for (int8_t i=0; i< Sensing::COLLECTION_BATCHES; i++)
  {
    pinMode(Sensing::TRIGGER_PINS[i], OUTPUT);
  }
  
  
  Serial.begin(115200);

  // Start the WebClient. This connects to the Wifi and sets the static flag Network::networkSetup to true
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