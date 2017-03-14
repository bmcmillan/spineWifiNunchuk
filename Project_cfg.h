#ifndef PROJECT_CFG_H
    #define PROJECT_CFG_H

#include "arduino.h"
#include <String.h>

/*******************************************************************************
**    Types                                                                   **
*******************************************************************************/

#define NULL    ( (void *)0 )



/*******************************************************************************
**    Configuration                                                           **
*******************************************************************************/    

/******** General Configuration ***********************************************/
#define BAUD_RATE_DEBUG       115200

//#define ENABLEDEVMODE //Output debugging information Nunchuck
//#define ENABLE_DEV_UDP

#define ENABLE_INCREASE_RESOLUTION_ACCEL

#define WIFI_CONNECT_TIMEOUT  20 // x 0,5s

#define UDP_PACKETS_TIMEOUT   50

#define WHEEL_PERIMETER_CM      (float)56.0  
#define MOTOR_GEAR_RATIO        (float)0.2105 /* 16/76 * 100 */

#define VBAT_MIN_MV       1980
#define VBAT_MAX_MV       2550

/******** Arduino Pins Configuration ******************************************/
#define BUTTON_PIN      0u

/* MicroOLED Definition  */
#define PIN_RESET_LCD 16
#define DC_JUMPER     0

/* Nunchuck/LCD I2C  */
#define PIN_SDA   5
#define PIN_SCL   4


/*******************************************************************************
**    Defines                                                                 **
*******************************************************************************/  

#define TRUE  1
#define FALSE 0


#include "ScreenMenu.h"


/*******************************************************************************
**    External data                                                           **
*******************************************************************************/ 

extern float esc_v_in;
extern float esc_current_motor;
extern float esc_current_in;
extern float esc_rpm;
extern float esc_amp_hours;
extern float esc_amp_hours_charged;
extern float esc_watt_hours;
extern float esc_watt_hours_charged;

extern unsigned char GetSystemState(void);

extern void MenuPrint_WifiInit(void);
extern void MenuPrint_Menu_1(void);
extern void MenuPrint_Menu_2(void);
extern void MenuPrint_Menu_3(void);
extern void MenuPrint_Menu_4(void);
extern void MenuPrint_Error(void);

extern byte NunchukIsZPressed(void);
extern byte NunchukIsCPressed(void);
extern byte NunchukX(void);
extern byte NunchukY(void);
extern byte NunchukY_ORG(void);
extern byte NunchukIsOK(void);
extern byte NunchukState(void);

extern void Wifi_Setup(void);
extern void Wifi_ServerNunchuck(void);
extern void Wifi_ClientEsc(void);
extern byte Wifi_Status(void);

#endif

