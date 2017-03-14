#include "Project_cfg.h"
#include "ScreenMenu.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Metro.h>
#include <ER_MicroOLED.h>
#include <ArduinoNunchuk.h>

//Metro timers
Metro tick_10ms = Metro(10);
Metro tick_200ms = Metro(200);


void setup()
{
    /* Serial communication setup */
    Serial.begin(BAUD_RATE_DEBUG);
    Serial.println(F("Nunchuck Faraday Controller"));
   
    delay(50);  
   
    /* nunchuck configuration setup */
    NunchukInit();
   
    /* LCD configuration setup */
    DisplayInit();
      
    /* Wifi communication setup */
    Wifi_Setup();
}

void loop()
{
    if (tick_10ms.check() == 1)
    {
        /* Read Data from Nunchuck  */
        NunchukRead();
        /* Check UPD communication with AccesPoint */
        Wifi_UDP_Manager();
    }
    yield();

    if (tick_200ms.check() == 1)
    {
        /* Update user menu */
        ScreenMenu();
    }
}

unsigned char GetSystemState(void)
{
  byte result_wifi = Wifi_Status();
  byte result_nunchuk = NunchukState();

  if ((result_wifi == 0) && (result_nunchuk == 0)){
    /* Connections are OK!!! */
    return 0;
  }
  else{
    return 1;
  }
}
