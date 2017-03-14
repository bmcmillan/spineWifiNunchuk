#include "Project_cfg.h"

#include <Arduino.h>
#include <String.h>

typedef union
{
 float number;
 char  bytes[4];
} FLOATUNION_t;

#define UPD_NOT_CONNECTED      0u
#define UPD_SERVER_CONNECTED   1u 

const char *ssid = "FARADAY100";
const char *password = "faraday100";
const unsigned int UDP_Port = 9999;
WiFiUDP portUDP;


char packetBuffer[255];
char dataHeaderData[] = "$DATA:";
char dataHeaderConnect[] = "$CONNECT:";

unsigned int wifi_upd_packet_timeout = 0;
unsigned int wifi_upd_communcation_state = UPD_NOT_CONNECTED;


float esc_v_in = 0;
float esc_current_motor = 0;
float esc_current_in = 0;
float esc_rpm = 0;
float esc_amp_hours = 0;
float esc_amp_hours_charged = 0;
float esc_watt_hours = 0;
float esc_watt_hours_charged = 0;



void Wifi_SendNunchuckData(void);
void Wifi_SendNunchuckConnection(void);
bool Wifi_CheckPacket(char* dataBuff, char len);
void Wifi_ProcessPacketData(char* dataBuff);
void Wifi_SetEscDataDefault(void);


void Wifi_Setup()
{
  byte wifi_connect_count = 0;
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while ((WiFi.status() != WL_CONNECTED) && (wifi_connect_count < WIFI_CONNECT_TIMEOUT)){
    MenuPrint_WifiInit();
    delay(400);
    Serial.print(".");
    wifi_connect_count++;
  }

  if (wifi_connect_count < WIFI_CONNECT_TIMEOUT){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    /* Now start UDP ports */
    portUDP.begin(UDP_Port);
  }
  else {
    Serial.println("");
    Serial.println("WiFi NOT connected...");  
  }

  wifi_upd_packet_timeout = 0;
  wifi_upd_communcation_state = UPD_NOT_CONNECTED;
}

byte Wifi_Status(void)
{
  if (WiFi.status() != WL_CONNECTED) {
    /* Wifi not connected... */
    return(1);
  }
  else{
    if (wifi_upd_packet_timeout >= UDP_PACKETS_TIMEOUT){
      /* wifi connected but NO UDP packets received!! */
      return(2);
    }
    else{
      /* wifi connected and valid UDP packets received!! */
      return(0);
    }
  }
}

void Wifi_UDP_Manager(void)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        /* Transmit UDP connection package until we get a valid data package from the AP */
        if (wifi_upd_communcation_state == UPD_NOT_CONNECTED) {
            /* Send the connection packet to the AP (to communicate IP) */
            Wifi_SendNunchuckConnection();
        }

        /* Connected to the AP, UDP. Check for incoming packets */
        int packetSize = portUDP.parsePacket();
        bool new_packed_valid = false;
        
        if (packetSize) {
            int len = portUDP.read(packetBuffer, 254);
            if (len > 0) packetBuffer[len] = 0; /* in case of string, assure last 0 position */
            #if defined(ENABLE_DEV_UDP)
            Serial.println(packetBuffer);
            #endif
           
            /* Verify packet validity */
            new_packed_valid = Wifi_CheckPacket(packetBuffer, len);
           
            if (new_packed_valid){
                wifi_upd_communcation_state = UPD_SERVER_CONNECTED; /* Stop sending "Connection packet" */
                wifi_upd_packet_timeout = 0; /* reset timeout counter */
                Wifi_ProcessPacketData(packetBuffer);
                /* Give back sender confirmation, with nunchuck info */
                Wifi_SendNunchuckData();
                #if defined(ENABLE_DEV_UDP)
                Serial.print("Valid packet received: ");
                Serial.print(len, DEC);
                Serial.println(" bytes");
                #endif
            }
            else{
                wifi_upd_packet_timeout++; /* Increase timeout counter... */
                //#if defined(ENABLE_DEV_UDP)
                Serial.println("Invalid packet received.");
                //#endif
            }
        }
        else{
            wifi_upd_packet_timeout++; /* Increase timeout counter... */
        }

        /* monitor timeout, checked every 10ms */
        if (wifi_upd_packet_timeout >= UDP_PACKETS_TIMEOUT){
            //#if defined(ENABLE_DEV_UDP)
            Serial.println("Timeout UDP!!!");
            //#endif
            wifi_upd_packet_timeout = UDP_PACKETS_TIMEOUT;
            /* Connection lost... let's try to send again CONNECTION command */
            wifi_upd_communcation_state = UPD_NOT_CONNECTED;
            Wifi_SetEscDataDefault();
        }

    }
    else{
      /* Wifi connection lost with AP... To implement Wifi retry */
    }

}


bool Wifi_CheckPacket(char* dataBuff, char len)
{
    int iloop;
    
    /* Verify header information */
    int comp_header = memcmp(dataBuff, dataHeaderData, sizeof(dataHeaderData)-1);
   
    /* Calculate checksum of the packet */
    char checksum = 0;
    for (iloop = 0; iloop < (len - 1); iloop++) {
        checksum += dataBuff[iloop];
    }
   
    /* Check packet validity */
    if (!comp_header & (checksum == dataBuff[len-1])) {
        return true;
    }
    else {
        return false;
    }
}


void Wifi_ProcessPacketData(char* dataBuff)
{
    FLOATUNION_t float_temp;
    float float_temp2;
    char array_temp[4];
    byte buffer_ptr = sizeof(dataHeaderData) - 1; /* point to the first data byte */

    BytesToFloat(&dataBuff[buffer_ptr], &esc_v_in);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_current_motor);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_current_in);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_rpm);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_amp_hours);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_amp_hours_charged);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_watt_hours);
    buffer_ptr += 4;
    BytesToFloat(&dataBuff[buffer_ptr], &esc_watt_hours_charged);
    
    #if defined(ENABLE_DEV_UDP)
    Serial.print("Vin:");
    Serial.println(esc_v_in, 2); /* Float with 2 decimals */
    Serial.print("Motor Current:");
    Serial.println(esc_current_motor, 2); /* Float with 2 decimals */
    Serial.print("Battery Current:");
    Serial.println(esc_current_in, 2); /* Float with 2 decimals */
    Serial.print("RPMs:");
    Serial.println(esc_rpm, 2); /* Float with 2 decimals */
    Serial.print("AmpH:");
    Serial.println(esc_amp_hours, 2); /* Float with 2 decimals */
    Serial.print("AmpH Charge:");
    Serial.println(esc_amp_hours_charged, 2); /* Float with 2 decimals */
    Serial.print("WattsH:");
    Serial.println(esc_watt_hours, 2); /* Float with 2 decimals */
    Serial.print("WattsH Charged:");
    Serial.println(esc_watt_hours_charged, 2); /* Float with 2 decimals */
    #endif
}

void Wifi_SetEscDataDefault(void)
{
    esc_v_in = 0;
    esc_current_motor = 0;
    esc_current_in = 0;
    esc_rpm = 0;
    esc_amp_hours = 0;
    esc_amp_hours_charged = 0;
    esc_watt_hours = 0;
    esc_watt_hours_charged = 0;
}

void Wifi_SendNunchuckData(void)
{
    int iloop;
    char checksum = 0;
    
    portUDP.beginPacket(portUDP.remoteIP(),portUDP.remotePort());
    portUDP.write(dataHeaderData);
    portUDP.write(NunchukX());
    portUDP.write(NunchukY());
    portUDP.write(NunchukIsZPressed());
    portUDP.write(NunchukIsCPressed());
    portUDP.write(NunchukIsOK());
    portUDP.write('*');
    /* Calculate checksum */
    for (iloop = 0; iloop < (sizeof(dataHeaderData)-1); iloop++) {
        checksum += dataHeaderData[iloop];
    }
    checksum += '*';
    checksum += NunchukX() + NunchukY() + NunchukIsZPressed() + NunchukIsCPressed() + NunchukIsOK();
    portUDP.write(checksum);
    portUDP.endPacket();
}

void Wifi_SendNunchuckConnection(void)
{
    int iloop;
    char checksum = 0;
 
    /* Build and send "Connection" frame to be sent to AP */
    portUDP.beginPacket(WiFi.gatewayIP(), UDP_Port);
    portUDP.write(dataHeaderConnect);

    /* Calculate checksum */
    for (iloop = 0; iloop < (sizeof(dataHeaderConnect)-1); iloop++) {
        checksum += dataHeaderConnect[iloop];
    }
    portUDP.write(checksum);
    portUDP.endPacket();
}

void FloatToBytes(float val, char b[4])
{
  FLOATUNION_t float_temp;
  
  float_temp.number = val;
  
  b[3] = float_temp.bytes[3];
  b[2] = float_temp.bytes[2];
  b[1] = float_temp.bytes[1];
  b[0] = float_temp.bytes[0];
}

void BytesToFloat(char* b, float* val)
{
   FLOATUNION_t float_temp;

   float_temp.bytes[3] = b[3];
   float_temp.bytes[2] = b[2];
   float_temp.bytes[1] = b[1];
   float_temp.bytes[0] = b[0];
   *val = float_temp.number;
}

