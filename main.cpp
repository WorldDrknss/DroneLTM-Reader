// BETAFLIGHT 4.5 ADD 'TELEMETRY_LTM' to CUSTOM DEFINES.
#include <Arduino.h>
#include <SoftwareSerial.h>

#define LTM_RX_PIN (14)
#define LTM_TX_PIN (12)
#define LTM_BAUD 9600
#define DEBUG_BAUD 115200
#define DEBUG

#include <ltm.h>

void setup()
{

  #ifdef DEBUG
  //Start Hardware Serial Interface
  Serial.begin(DEBUG_BAUD);
  Serial.println("DEBUG MODE ENABLED");
  #endif
  
  // Start Software Serial Interface
  ltmBegin();

  //Enable BUILTIN LED
  pinMode(LED_BUILTIN, OUTPUT);
}

void process_loop()
{
  #ifdef DEBUG
    Serial.printf("Frame: %d / Pitch: %d / Roll: %d / Head: %d / Alt: %d / Lat: %d / Lon: %d / FM: %s\n", ltmFrameType, ltmData.pitch, ltmData.roll, ltmData.heading, ltmData.altitude, ltmData.latitude, ltmData.longitude, LTM_FLIGHT_MODES[ltmData.flightmode]);
    // Serial.println(LTM_DATA.altitude);
  #endif
}

void loop()
{
  ltmLoop();
  process_loop();
}