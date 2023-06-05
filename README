# LBE Sensing with ESP32 and MaxBotix MB 1040 LV

## Platform IO
This is a pltform IO project. The platformio.ini is available for those that want to recreate this. https://platformio.org/

## Objective
To use 6 (or any number) of MaxBotix MB 1040 LV sensors to detect promiximity and send out a socket message to a remote socket server - in this case a raspberry pi. This use case only needed the either the presence of absence of something to be sent and not the actual distance.  

## Sensing with multiple sensors
- Sensing with multiple sensors requires one to use the trigger mode for sensing to ensure that sensors do not interfere with each other. A sensor is triggered by keeping the RX pin high for 30 microseconds and then turning it low. See function - Sensing::activateSensingSequence() in Globals.hpp. Also see the datasheet for this sensor here - https://maxbotix.com/pages/lv-maxsonar-ez-datasheet

- MaxBotix MB 1040 LV sensors can be chained to sense one after the other my connecting the TX pin of one to the RX pin of the other. However as more and more sensors are chained, the ranging frequency falls off and it takes longer and longer to detect things that are close by. See this blogpost - https://maxbotix.com/blogs/blog/using-multiple-ultrasonic-sensors?_pos=1&_sid=62b99fb11&_ss=r

- Since in this use case close by sensing was needed at a reasonable rate only two sensors were chained to each other. This created 3 sets of 2 sensors each. Each set was triggered one after the other to collect sensor samples. 

## Code and Customisation
- Almost all the constant parameters are in Globals.hpp except some network ones (see next point). They should be adequately documented to help anyone customise. 
- The WebClient.hpp contains the WebClient class, which connects to the WiFi and sends out socket messages. It contains some needed network parameters that one can adjust. 
- ProximitySensor.hpp contains the ProximitySensor class that declares a senor and manages the sensing. This class can be inherited from to handle other kinds of sensors, including other MaxBotix sensors, which have different attributes, for e.g. different scale factors. 
- Main.cpp sequences the main activities. 1st it configures the network via the WebCient class and then goes into the sensing mode. It uses xTaskCreatePinnedToCore method of FreeRTOS, which is a real-time operating system kernel. One could also use the standard loop() method of the Arduino library. xTaskCreatePinnedToCore was used to handle potential concurrent tasks in the future. 
- The sensingTask() function is the core of the this project. It should be fairly self-explanatory and the code is commented to provide more information.

## Disclaimer.
This project was created by people who are neither experts of the esp32 frameworks or of ultrasonic sensing. It is almost certain that the whole thing can be optimised and made better. We would love to hear on how we can improve this. If you would like to improve this repo then please reach out at romit@quicksand.co.in. The learnings will be soon documented in a blog post, which will be linked here. 