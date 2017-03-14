#include "Project_cfg.h"
#include "Display_Images.h"

MicroOLED oledLcd(PIN_RESET_LCD, DC_JUMPER);

#define LCD_BATT_WIDTH   20
#define LCD_BATT_HEIGTH  35
#define LCD_BATT_POSX     1
#define LCD_BATT_POSY    11


void DisplayInit(void)
{
    oledLcd.begin(PIN_SDA, PIN_SCL);
    delay(10);
    oledLcd.clear(PAGE);
    oledLcd.clear(ALL);
    oledLcd.setFontType(0);
    oledLcd.setTextColor(WHITE);
    oledLcd.setTextSize(0);
    oledLcd.drawBitmap(FaradayMotionSmall);
    oledLcd.display(); 
    /* show Intro picture during 2s */
    for(uint8_t loop=0; loop<10; loop++){
        delay(200);
        yield();
    }
}

void MenuPrint_WifiInit(void)
{
    static uint8_t logo_state = 0;
    
    oledLcd.clear(PAGE);
    switch(logo_state)
    {
        case 0:
            oledLcd.drawBitmap(wifi_logo_small_0);
            logo_state++;
        break;
        case 1:
            oledLcd.drawBitmap(wifi_logo_small_1);
            logo_state++;
        break;
        case 2:
            oledLcd.drawBitmap(wifi_logo_small_2);
            logo_state++;
        break;
        case 3:
            oledLcd.drawBitmap(wifi_logo_small_3);
            logo_state = 0;
        break;
        default:
            logo_state = 0;
        break;
    }
    oledLcd.display();
}


void MenuPrint_Menu_1(void)
{
    char batt_level = 0;
    oledLcd.clear(PAGE);
    
    long batt_info = (long)(esc_v_in * 100);
    if (batt_info < VBAT_MIN_MV){
        batt_info = VBAT_MIN_MV;
    }
    long batt_level_percentage = ((batt_info - VBAT_MIN_MV)*100u)/(VBAT_MAX_MV-VBAT_MIN_MV);
    batt_level = map(batt_info, VBAT_MIN_MV, VBAT_MAX_MV, 0, 8);
    switch(batt_level)
    {
        case 0:
            oledLcd.drawBitmap(battery_00);
        break;
        case 1:
            oledLcd.drawBitmap(battery_0);
        break;
        case 2:
            oledLcd.drawBitmap(battery_1);
        break;  
        case 3:
            oledLcd.drawBitmap(battery_2);
        break;  
        case 4:
            oledLcd.drawBitmap(battery_3);
        break;  
        case 5:
            oledLcd.drawBitmap(battery_4);
        break;
        case 6:
            oledLcd.drawBitmap(battery_5);
        break;
        case 7:
        case 8:
            oledLcd.drawBitmap(battery_6);
        break;
    }
    yield();
   
    oledLcd.setCursor(LCD_BATT_WIDTH + 5, 10);
    oledLcd.setTextSize(1);
    oledLcd.print(esc_v_in, 1);
    oledLcd.print("V");
   
    oledLcd.setCursor(LCD_BATT_WIDTH + 8, 28);
    if (batt_level_percentage < 10){
        oledLcd.print(" ");
    }
    oledLcd.print(batt_level_percentage);
    oledLcd.print("%");
   
    oledLcd.display();
}

void MenuPrint_Menu_2(void)
{
    oledLcd.clear(PAGE);
    oledLcd.setTextSize(2);
    oledLcd.setCursor(3, 10);
    /* Get conversion factor based on Gears and Wheel perimeter */
    float conv_factor = ((float)WHEEL_PERIMETER_CM * (float)MOTOR_GEAR_RATIO * 6.0 ) / 10000.0;
    /* Convert from RPMs to km/h */
    float current_speed = conv_factor * (float)esc_rpm;
    if (current_speed >= 100){
        oledLcd.print(current_speed,0);
    }
    else{
        oledLcd.print(current_speed,1);
    }
    yield();
    oledLcd.setTextSize(1);
    oledLcd.setCursor(28, 33);
    oledLcd.print("km/h");  
    
    oledLcd.display();
}

void MenuPrint_Menu_3(void)
{
    oledLcd.clear(PAGE);
    
    oledLcd.setCursor(5, 5);
    oledLcd.print(esc_current_in, 1);
    oledLcd.print("A");
   
    oledLcd.setCursor(5, 20);
    oledLcd.print(esc_current_motor, 1);
    oledLcd.print("A");
    yield();
    /*
    oledLcd.setCursor(1, 20);
    oledLcd.print(, 1);
    oledLcd.print("A");
   
    oledLcd.setCursor(1, 25);
    oledLcd.print(, 1);
    oledLcd.print("A out");
    */
   
    /*extern float esc_amp_hours_charged;
    extern float esc_watt_hours_charged;
    esc_amp_hours
    esc_watt_hours*/
    
    oledLcd.display();
}


void MenuPrint_Menu_4(void)
{
    oledLcd.clear(PAGE);
    //oledLcd.setCursor(0, 5);
    //oledLcd.print("Debug"); 
    //byte posicio_x = map(NunchukX(), 0, 255, 0, 63);
    //byte posicio_y = map(NunchukY(), 0, 255, 47, 5);
    //oledLcd.pixel(posicio_x,posicio_y);
    //oledLcd.setCursor(10, 15);
    //oledLcd.print(NunchukY(), DEC);
   
    byte remapY = map(NunchukY(), 125, 255, 1, 40);
    byte remapY_ORG = map(NunchukY_ORG(), 125, 255, 1, 40);
    oledLcd.rect(20, 7, 5, remapY);
    oledLcd.rect(40, 7, 5, remapY_ORG);
   
    oledLcd.display();
}

void MenuPrint_Error(void)
{
    oledLcd.clear(PAGE);
    oledLcd.setCursor(0, 5);
    byte result_wifi = Wifi_Status();
    byte result_nunchuk = NunchukIsOK();
    if (result_wifi == 1){
        oledLcd.print("Wifi fail"); 
        oledLcd.setCursor(0, 15);
        oledLcd.print("Check PWR"); 
        oledLcd.setCursor(0, 25);
        oledLcd.print("and reset"); 
        oledLcd.setCursor(0, 35);
        oledLcd.print("Nunchuck");
    }
    else if (result_wifi == 2){
        oledLcd.print("UDP timeout...");
    }
    else if (result_nunchuk > 0){
        oledLcd.print("NuncK Err");
    }
    oledLcd.display();
}
