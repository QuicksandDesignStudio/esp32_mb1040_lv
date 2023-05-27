#ifndef WebClient_hpp
#define WebClient_hpp

#include "Globals.hpp"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>



class WebClient {

private:
    // MODIFY THESE TO MATCH NETWORK SETTINGS
    const char      *c_ssid     = "qs_internal_blr_01"; // Network Name
    const char      *c_password = "laikaalba";          // Network Password
    const char      *hostname   = "SB_1";               // Hostname
    HardwareSerial  *m_serial;
    WiFiClient      m_wifiClient;

public:
    WebClient() {}

    void startClient(HardwareSerial *serial) {
        m_serial = serial;

        // Setup WiFi
        WiFi.mode(WIFI_STA);            // Station mode - no server setup as it causes socket latency 
        WiFi.begin(c_ssid, c_password); 

        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        m_serial->println("Failed To Connect To WiFi");
        return;
        }

        if (!MDNS.begin(hostname)) {
        m_serial->println("Error starting MDNS");
        }

        m_wifiClient = WiFiClient();
        m_wifiClient.setTimeout(Network::CLIENT_TIME_OUT_IN_SECONDS);

        m_serial->println(WiFi.localIP());
        m_serial->println(WiFi.getHostname());

        Network::networkSetup = true;
    }

    bool sendMessage(String message, const char* ipAddr, int port, int tryTimes) {
        int8_t counter = 0;

        //try upto tryTimes
        while(counter < tryTimes) {
            if(!m_wifiClient.connect(ipAddr, port)) {
                m_serial->println("Connection Failed");
                counter++;
                continue;
            }

            //send the message
            m_wifiClient.print(message);
            m_serial->println("Message Sent");

            //close the socket
            m_wifiClient.stop();
            m_serial->println("Socket closed");
            return true;
        }
        return false;
    }
};


#endif