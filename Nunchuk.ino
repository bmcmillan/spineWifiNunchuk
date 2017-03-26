#include "Project_cfg.h"

//Contains the previous checksum
byte previousNunchukChecksum = 0;
//How many times in a row was the checksum the same
int equalNunchukChecksumCount = 0;
//The max number of same checksums
int maxNunchukChecksumCount = 100;
//
byte nunchuckState = 1;

//Nunchuk configuration
uint8_t defaultNunchukMax = 250;
uint8_t NunchukLowerLimitInputAccel  = 135u;
uint8_t NunchukLowerLimitOutputAccel = 150u;
uint8_t NunchukUpperLimitInputAccel  = 255u;
uint8_t NunchukUpperLimitOutputAccel = 225u;

uint8_t NunchukLowerLimitInputBrake  = 45u;
uint8_t NunchukInputAccelFilter      = 160u;
uint8_t defaultNunchukNeutral = 127;


void NunchukReadSetDefault(void);


uint8_t Nunchuk_X = defaultNunchukNeutral;
uint8_t Nunchuk_Y = defaultNunchukNeutral;
uint8_t Nunchuk_Y_org = defaultNunchukNeutral;
uint8_t Nunchuk_zButton = 0;
uint8_t Nunchuk_cButton = 0;

ArduinoNunchuk nunchuk = ArduinoNunchuk();


void NunchukInit(void)
{
    nunchuk.init(PIN_SDA, PIN_SCL);
    /*nunchuk.init();*/
    NunchukReadSetDefault();
}

byte NunchukIsZPressed(void)
{
    return(Nunchuk_zButton);
}

byte NunchukIsCPressed(void)
{
    return(Nunchuk_cButton);
}

byte NunchukX(void)
{
    return(Nunchuk_X);
}
byte NunchukY(void)
{
    return(Nunchuk_Y);
}
byte NunchukY_ORG(void)
{
    return(Nunchuk_Y_org);
}

byte NunchukIsOK(void)
{
    return(nunchuckState==0);
}

byte NunchukState(void)
{
    return(nunchuckState);
}

void NunchukRead(void)
{
  uint8_t Nunchuk_Y_remap;
  
  nunchuk.update();
  yield();

  if (isNunchukDataValid(nunchuk))
  {
    yield();
    if (isNunchukChecksumValid(nunchuk))
    {
       #if defined(ENABLE_INCREASE_RESOLUTION_ACCEL)
       /* Remap the value reading from Nunchuck to have better resolution in the middle */
       if (nunchuk.analogY <= defaultNunchukNeutral)
       {
            Nunchuk_Y_remap = map(nunchuk.analogY, 0, defaultNunchukNeutral, NunchukLowerLimitInputBrake, defaultNunchukNeutral);
            yield();
            Nunchuk_Y = (uint8_t)((((uint16_t)Nunchuk_Y * (uint16_t)(32U - BRAKE_LOW_PASS_FILTER_FACTOR)) + ((uint16_t)Nunchuk_Y_remap * (uint16_t)BRAKE_LOW_PASS_FILTER_FACTOR))/ 32U);
        }
        else if (nunchuk.analogY < NunchukLowerLimitInputAccel)
        {
            Nunchuk_Y = map(nunchuk.analogY, defaultNunchukNeutral, NunchukLowerLimitInputAccel, defaultNunchukNeutral, NunchukLowerLimitOutputAccel);
        }
        else /* if (nunchuk.analogY < NunchukUpperLimitInputAccel)*/
        {
            Nunchuk_Y_remap = map(nunchuk.analogY, NunchukLowerLimitInputAccel, NunchukUpperLimitInputAccel, NunchukLowerLimitOutputAccel, NunchukUpperLimitOutputAccel);
            yield();
            if (nunchuk.analogY < NunchukInputAccelFilter){
              Nunchuk_Y = Nunchuk_Y_remap;
            }
            else{ /* start to apply low pass filter */
              Nunchuk_Y = (uint8_t)((((uint16_t)Nunchuk_Y * (uint16_t)(32U - ACCEL_LOW_PASS_FILTER_FACTOR)) + ((uint16_t)Nunchuk_Y_remap * (uint16_t)ACCEL_LOW_PASS_FILTER_FACTOR))/ 32U);
            }
        }

        #else
        Nunchuk_Y = nunchuk.analogY;
        #endif
        yield();
        Nunchuk_Y_org = nunchuk.analogY;
        Nunchuk_X = nunchuk.analogX;
        Nunchuk_zButton = nunchuk.zButton;
        Nunchuk_cButton = nunchuk.cButton;
        nunchuckState = 0;
        #if defined(ENABLEDEVMODE)
        Serial.println("DATA");
        Serial.print(nunchuk.analogX, DEC);
        Serial.print(' ');
        Serial.print(nunchuk.analogY, DEC);
        Serial.print(' ');
        Serial.print(nunchuk.accelX, DEC);
        Serial.print(' ');
        Serial.print(nunchuk.accelY, DEC);
        Serial.print(' ');
        Serial.print(nunchuk.accelZ, DEC);
        Serial.print(' ');
        Serial.print(nunchuk.zButton, DEC);
        Serial.print(' ');
        Serial.println(nunchuk.cButton, DEC);
        #endif
    }
    else
    {
      /* We had too many repetitive samples with the same checksum values in a row... so something is wrong!! */
      NunchukReadSetDefault();
      nunchuckState = 1;
#if defined(ENABLEDEVMODE)
      Serial.print("EqualChecksumCount: ");
      Serial.println(equalNunchukChecksumCount);
#endif
    }
  }
  else
  {
    //The nunchuk is not connected and the reciever is giving us garbage
    NunchukReadSetDefault();
    nunchuckState = 2;
#if defined(ENABLEDEVMODE)
    Serial.println("Connect Nunchuk");
#endif
  }
}

//Checks if the values returned from the nunchuk seems valid
bool isNunchukDataValid(ArduinoNunchuk nunchuk)
{
  if (nunchuk.analogX < 0 || nunchuk.analogX > 255)
    return false;
  if (nunchuk.analogY < 0 || nunchuk.analogY > 255)
    return false;
  if (nunchuk.accelX < 0 || nunchuk.accelX > 1023)
    return false;
  if (nunchuk.accelY < 0 || nunchuk.accelY > 1023)
    return false;
  if (nunchuk.accelZ < 0 || nunchuk.accelZ > 1023)
    return false;
  if (nunchuk.zButton < 0 || nunchuk.zButton > 1)
    return false;
  if (nunchuk.cButton < 0 || nunchuk.cButton > 1)
    return false;
  return true;
}

//Creates a checksum and checks if it is good/fresh data
bool isNunchukChecksumValid(ArduinoNunchuk nunchuk)
{
  int sum = 0;
  sum += nunchuk.analogX;
  sum += nunchuk.analogY;
  sum += nunchuk.accelX;
  sum += nunchuk.accelY;
  sum += nunchuk.accelZ;
  sum += nunchuk.zButton;
  sum += nunchuk.cButton;
  //After first initialisation, there is a hickup and this resets this hickup, 3579 is the max value from all properties
  if (sum == 3579)
  {
   // Wire.endTransmission(false);
    delay(1);
    nunchuk.init(PIN_SDA, PIN_SCL);
    /*nunchuk.init();*/
    delay(1);
    return false;
  }
  //Create the checksum
  byte checkSum = sum % 256;
  if (previousNunchukChecksum == checkSum)
    equalNunchukChecksumCount ++;
  else
    equalNunchukChecksumCount = 0;
  previousNunchukChecksum = checkSum;
  if (equalNunchukChecksumCount > maxNunchukChecksumCount)
    return false;
  return true;
}

void NunchukReadSetDefault(void)
{
    Nunchuk_X = defaultNunchukNeutral;
    Nunchuk_Y = defaultNunchukNeutral;
    Nunchuk_zButton = 0;
    Nunchuk_cButton = 0;

}

