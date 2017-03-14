#include "Project_cfg.h"

uint8_t state = 0;
uint8_t button_released = 1;
  

#define BUTTON_MENU_PRESSED()           NunchukIsCPressed()
#define BUTTON_MENU_PRESSED_RELEASED()  (BUTTON_MENU_PRESSED() && button_released)
#define BUTTON_BOTH_PRESSED()           NunchukIsCPressed() && NunchukIsZPressed()


#define  i_INIT        ((uint8_t)0)
#define  i_MENU_1      ((uint8_t)1)
#define  i_MENU_2      ((uint8_t)2)
#define  i_MENU_3      ((uint8_t)3)
#define  i_MENU_4      ((uint8_t)4)
#define  i_MENU_ERROR  ((uint8_t)5)

 /* Index variable. */
static uint8_t ind_ScreenMenu = i_INIT;


void ScreenMenu(void)
{
    /* Next state analyser. */
    switch(ind_ScreenMenu){
        case i_INIT:
            /* State actions (Moore). */
            state = GetSystemState();
            /* Next state selection. */
            if(state == 0){
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_1;
            }
            else{
                ind_ScreenMenu = i_MENU_ERROR;
            }
        break;
        case i_MENU_1:
            /* State actions (Moore). */
            MenuPrint_Menu_1();
            state = GetSystemState();
            /* Next state selection. */
            if(state != 0){	/* [1]. */
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_ERROR;
            }else if(!BUTTON_MENU_PRESSED()){	/* [2]. */
                /* Transition actions (Meally). */
                button_released = 1;
            }else if(BUTTON_MENU_PRESSED_RELEASED()){	/* [3]. */
                /* Transition actions (Meally). */
                button_released = 0;
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_2;
            }
        break;
        case i_MENU_2:
            /* State actions (Moore). */
            MenuPrint_Menu_2();
            state = GetSystemState();
            /* Next state selection. */
            if(state != 0){	/* [1]. */
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_ERROR;
            }else if(!BUTTON_MENU_PRESSED()){	/* [2]. */
                /* Transition actions (Meally). */
                button_released = 1;
            }else if(BUTTON_MENU_PRESSED_RELEASED()){	/* [3]. */
                /* Transition actions (Meally). */
                button_released = 0;
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_3;
            }
        break;
        case i_MENU_3:
            /* State actions (Moore). */
            MenuPrint_Menu_3();
            state = GetSystemState();
            /* Next state selection. */
            if(state != 0){	/* [1]. */
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_ERROR;
            }else if(!BUTTON_MENU_PRESSED()){	/* [2]. */
                /* Transition actions (Meally). */
                button_released = 1;
            }else if(BUTTON_MENU_PRESSED_RELEASED()){	/* [3]. */
                /* Transition actions (Meally). */
                button_released = 0;
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_4;
            }
        break;
        case i_MENU_4:
            /* State actions (Moore). */
            MenuPrint_Menu_4();
            state = GetSystemState();
            /* Next state selection. */
            if(state != 0){	/* [1]. */
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_ERROR;
            }else if(!BUTTON_MENU_PRESSED()){	/* [2]. */
                /* Transition actions (Meally). */
                button_released = 1;
            }else if(BUTTON_MENU_PRESSED_RELEASED()){	/* [3]. */
                /* Transition actions (Meally). */
                button_released = 0;
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_1;
            }
        break;
        case i_MENU_ERROR:
            /* State actions (Moore). */
            state = GetSystemState();
            MenuPrint_Error();
            /* Next state selection. */
            if((state == 0) ||
               BUTTON_MENU_PRESSED_RELEASED()){
                /* Pointing to the next state. */
                ind_ScreenMenu = i_MENU_1;
            }
        break;

    }
}
