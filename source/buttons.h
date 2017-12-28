/*
 * buttons.h
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

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

button_events_t button_listener();
#endif /* BUTTONS_H_ */
