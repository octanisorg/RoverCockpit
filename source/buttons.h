/*
 * buttons.h
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "states.h"

typedef enum {
	BUTTON_NO_EVENT,
	BUTTON_ARM_EVENT,
	BUTTON_WIFI_CONNECT_EVENT,
	BUTTON_SAVE_EVENT,
	BUTTON_U_EVENT,
	BUTTON_L_EVENT,
	BUTTON_R_EVENT,
	BUTTON_D_EVENT

} button_events_t;

typedef enum  {
	TOUCH_BUTTON1,
	TOUCH_BUTTON2,
	TOUCH_BUTTON3,
	NO_TOUCH_BUTTON
} touch_button;

//NOT NEEDED!

typedef enum {
	ARM_TOUCH_BUTTON_STATE,
	DISARM_TOUCH_BUTTON_STATE,

} button1_sub_state;

typedef enum {
	CONNECT_TOUCH_BUTTON_STATE,
	CONNECTING_TOUCH_BUTTON_STATE,
	CONNECTED_TOUCH_BUTTON_STATE,

} button2_sub_state;

typedef enum {
	SAVE_TOUCH_BUTTON_STATE,
	SAVING_TOUCH_BUTTON_STATE
} button3_sub_state;

button_events_t button_listener();

touch_button button_touchscreen_identify(touchPosition * touch);
button_events_t button_touch_event_analyse();
void button_touch_init();
void button_touch_update(button_events_t button_events);

#endif /* BUTTONS_H_ */
