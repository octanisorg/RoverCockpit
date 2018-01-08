/*
 * buttons.c
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#include <nds.h>
#include "buttons.h"
#include "states.h"
#include "graphics.h"

button1_sub_state button1_state;
button2_sub_state button2_state;
button3_sub_state button3_state;


button_events_t button_listener(){
	scanKeys();
	unsigned down = keysDown();

	if(down & KEY_TOUCH){
		return button_touch_event_analyse();
	}else if(down & KEY_START){
		return BUTTON_WIFI_CONNECT_EVENT;
	}else if(down & KEY_A){
		return BUTTON_ARM_EVENT;
	}else if(down & KEY_SELECT){
		return BUTTON_SAVE_EVENT;
	}


	if(down & KEY_DOWN){
		return BUTTON_D_EVENT;
	}else if(down & KEY_UP){
		return BUTTON_U_EVENT;
	}else if(down & KEY_LEFT){
		return BUTTON_L_EVENT;
	}else if(down & KEY_RIGHT){
		return BUTTON_R_EVENT;
	}else{
		return BUTTON_NO_EVENT;
	}


}


touch_button button_touchscreen_identify(touchPosition * touch){

	if(144<touch->px && touch->px<240 && 48<touch->py && touch->py<80){
		return TOUCH_BUTTON1;
	}
	else if(144<touch->px && touch->px<240 && 88<touch->py && touch->py<120){
		return TOUCH_BUTTON2;
	}
	else if(144<touch->px &&touch->px<240 && 128<touch->py && touch->py<160){
		return TOUCH_BUTTON3;
	}
	else{
		return NO_TOUCH_BUTTON;
	}

}

button_events_t button_touch_event_analyse(){
	touchPosition touch;
	touchRead(&touch);
	touch_button pressed_touch_button = button_touchscreen_identify(&touch); //Reads which button on the touch screen is pressed

	switch(pressed_touch_button){
	case TOUCH_BUTTON1:
		return BUTTON_ARM_EVENT;
	case TOUCH_BUTTON2:
		return BUTTON_WIFI_CONNECT_EVENT;
	case TOUCH_BUTTON3:
		return BUTTON_SAVE_EVENT;
	default: return BUTTON_NO_EVENT;
	}

}

void button_touch_init(){
	touch_button button;

	button=TOUCH_BUTTON1;
	button1_state=ARM_TOUCH_BUTTON_STATE;
	graphics_printDebug_SUB("ARM", button);


	button=TOUCH_BUTTON2;
	button2_state=CONNECT_TOUCH_BUTTON_STATE;
	graphics_printDebug_SUB("CONNECT", button);

	button=TOUCH_BUTTON3;
	button3_state=SAVE_TOUCH_BUTTON_STATE;
	graphics_printDebug_SUB("SAVE", button);
}

void button_touch_update(button_events_t button_events){

	switch(button_events){
	case BUTTON_ARM_EVENT:
		if(button1_state==ARM_TOUCH_BUTTON_STATE){
			button1_state=DISARM_TOUCH_BUTTON_STATE;
		graphics_printDebug_SUB("DISARM", TOUCH_BUTTON1);
		}else if(button1_state==DISARM_TOUCH_BUTTON_STATE){
			button1_state=ARM_TOUCH_BUTTON_STATE;
			graphics_printDebug_SUB("ARM", TOUCH_BUTTON1);
		}
		break;
	case BUTTON_WIFI_CONNECT_EVENT:
		if(button1_state==CONNECT_TOUCH_BUTTON_STATE){
			button2_state=DISCONNECT_TOUCH_BUTTON_STATE;
			graphics_printDebug_SUB("DISCONNECT", TOUCH_BUTTON2);
		}else if(button1_state==DISCONNECT_TOUCH_BUTTON_STATE){
			button2_state=CONNECT_TOUCH_BUTTON_STATE;
			graphics_printDebug_SUB("CONNECT", TOUCH_BUTTON2);
		}
		break;
	default: break;
	}
}

