/*  Santa Signaller
 *  
 *  This code is part of the Santa signaller project.
 *  Instructions how to assemble and upload can be found on
 *  https://klushok.etv.tudelft.nl/projects
 *  
 *  The code has been developed for and has been tested
 *  on a ESP8266 chip.
 *  
 *  It depends on a custom WiFiSettings library which, at 
 *  the time of writing, can be found on 
 *  https://github.com/klushok-etv/ESP-WiFiSettings
 *   
 * author: Bram den Ouden
 * 10-12-2020
 */



// https://github.com/arduino-libraries/NTPClient
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// include custom wifisettings lib (if error, make sure you initialised the submodule)
#include "src/ETV-WiFiSettings/WiFiSettings.h"
#include "LittleFS.h"

// array of pins connected to led groups
const uint8_t pin[] = {D1, D2, D3};
uint8_t activeLed = 0;

// intial values for blink behaviour
uint16_t blink_delay = 500;
unsigned long prevMillis = 0;

// Connect to europe pool with offset of 1 hour (s), update from NTP server every 12 hours (ms)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 43200000);

// timestaps of begin/end of event
const unsigned long epochEventStart = 1608854400;
const unsigned long epochEventEnd = 1609027199;

// initial values for some extra variables
signed long delta = 9999;
unsigned long t = 0;

void setup() {

  // Set all pins to output
  for (uint8_t i = 0; i < sizeof(pin); i++) {
    pinMode(pin[i], OUTPUT);
  }

  // initialize serial communication
  Serial.begin(115200);

  // initialize file system
  LittleFS.begin();

  //  Remove previously stored wifi settings
  //    WiFiSettings.clearWiFiSettings();

  // Turn on all leds when wifi credentials are required
  WiFiSettings.onPortal = []() {
    for (uint8_t i = 0; i < sizeof(pin); i++) {
      digitalWrite(pin[i], HIGH);
    }
  };

  // Open wifisettings portal after timeout of 10s
  WiFiSettings.connect(true, 10, "Santas-little-helper-");

  // start connection to NTP server
  timeClient.begin();
}

void loop() {
  if (millis() - prevMillis >= blink_delay)
  {
    // cycle through all available led groups
    activeLed = (activeLed + 1) % sizeof(pin);

    // Turn on 1 led group at a time
    for (uint8_t i = 0; i < sizeof(pin); i++) {
      if (activeLed == i) digitalWrite(pin[i], HIGH);
      else digitalWrite(pin[i], LOW);
    }

    // update blink delay once every cycle
    if (activeLed == 0) {
      // get current timestamp
      timeClient.update();
      t = timeClient.getEpochTime();

      // seconds until event starts
      delta = epochEventStart - t;
      if (delta < 0) delta += 31556952; // countdown until next year!

      // set blink interval
      blink_delay = delta / 1000;

      // feed the terminal
      Serial.printf("Only %d more seconds to go!\n", delta);
      Serial.printf("Only %.2f more days to go!\n", delta / 86400.0);
    }

    // Event happening
    if (t >= epochEventStart && t <= epochEventEnd) {
      Serial.println("IT'S THE SEASON TO BE JOLLY FALALALAAAAA LALALAAAAA");
      blink_delay = random(100, 500);
    }

    // set previous millis to current timestamp
    prevMillis = millis();
  }
}
