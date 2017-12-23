/*
 * buttons.c
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#include <nds.h>
#include "buttons.h"

button_events_t button_listener(){
	scanKeys();
	unsigned held = keysHeld();

	if(held & KEY_TOUCH){
		touchPosition touch;
		touchRead(&touch);

		//TODO: distinguish between sub buttons and fire event
	}else if(held & KEY_DOWN){
		return BUTTON_D_EVENT;
	}else if(held & KEY_UP){
		return BUTTON_U_EVENT;
	}else if(held & KEY_LEFT){
		return BUTTON_L_EVENT;
	}else if(held & KEY_RIGHT){
		return BUTTON_R_EVENT;

	}
}
