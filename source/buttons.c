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
	unsigned held = keysHeld();

	if(down & KEY_TOUCH){
		return button_touch_event_analyse();
		//TODO: distinguish between sub buttons and fire event
	}else if(down & KEY_START){
		return BUTTON_WIFI_CONNECT_EVENT;
	}else if(down & KEY_A){
		return BUTTON_ARM_EVENT;
	}else if(down & KEY_SELECT){
		return BUTTON_SAVE_EVENT;
	}


	if(held & KEY_DOWN){
		return BUTTON_D_EVENT;
	}else if(held & KEY_UP){
		return BUTTON_U_EVENT;
	}else if(held & KEY_LEFT){
		return BUTTON_L_EVENT;
	}else if(held & KEY_RIGHT){
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
		switch(button1_state){
		case ARM_TOUCH_BUTTON_STATE:
			return BUTTON_ARM_EVENT;
		case DISARM_TOUCH_BUTTON_STATE:
			return BUTTON_NO_EVENT;
		}
		case TOUCH_BUTTON2:
			switch(button2_state){
			case CONNECT_TOUCH_BUTTON_STATE:
				return BUTTON_WIFI_CONNECT_EVENT;
			default: break;

			}
			case TOUCH_BUTTON3:
				switch(button3_state){
				case SAVE_TOUCH_BUTTON_STATE:
					return BUTTON_SAVE_EVENT;
				default: break;
				}
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
		button1_state=DISARM_TOUCH_BUTTON_STATE;
		graphics_printDebug_SUB("DISARM", TOUCH_BUTTON1);
		break;
		/* Adding event???
	case BUTTON_ARM_EVENT:
		button=TOUCH_BUTTON1;
		button1_state=ARM_TOUCH_BUTTON_STATE;
		graphics_printDebug_SUB("ARM", button);
		 */
	case BUTTON_WIFI_CONNECT_EVENT:
		button2_state=CONNECTING_TOUCH_BUTTON_STATE;
		graphics_printDebug_SUB("CONNECTING", TOUCH_BUTTON2);
		break;
	case BUTTON_SAVE_EVENT:
		button2_state=SAVING_TOUCH_BUTTON_STATE;
		graphics_printDebug_SUB("SAVING",TOUCH_BUTTON3);
	default: break;
	}
}

