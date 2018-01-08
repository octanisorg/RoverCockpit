#include <nds.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dswifi9.h>
#include "buttons.h"
#include "graphics.h"
#include "wifi.h"
#include "soundeff.h"
#include "timesync.h"

#include "states.h"

void handle_button_events(button_events_t button_events, main_states_t * state){

	switch(button_events)
	{
		case BUTTON_ARM_EVENT:
			if(*state == DISARMED_STATE){
				*state = ARMED_STATE;
				soundeff_stateArmed();
				button_touch_update(BUTTON_ARM_EVENT);
			}else if(*state == ARMED_STATE){
				*state = DISARMED_STATE;
				soundeff_stateDisarmed();
				button_touch_update(BUTTON_ARM_EVENT);
			}

		break;

		case BUTTON_WIFI_CONNECT_EVENT:
			if(*state == DISCONNECTED_STATE){
				*state = CONNECT_WIFI_STATE;
				button_touch_update(BUTTON_WIFI_CONNECT_EVENT);
			}else if(*state == ARMED_STATE || *state == DISARMED_STATE){
				*state = DISCONNECTED_STATE;
			}
		break;

		case BUTTON_SAVE_EVENT:
			if(*state == ARMED_STATE || *state == DISARMED_STATE){
				button_touch_update(BUTTON_SAVE_EVENT);
				wifi_dumpParsedFramesToLog();
				soundeff_wifiFramesSaved();
			}
		break;

		case BUTTON_L_EVENT:
		case BUTTON_R_EVENT:
		case BUTTON_U_EVENT:
		case BUTTON_D_EVENT:

			if(*state == ARMED_STATE){
				wifi_sendData((uint8 *)button_events, 1);
				graphics_draw_arrows(button_events);

			}

		break;

		case BUTTON_NO_EVENT:
		break;
	}
}


//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	timesync_init();
	soundeff_init();
	graphics_mainInit();
	graphics_subInit();


	main_states_t state = DISCONNECTED_STATE;
	button_events_t button_events;
	wifi_events_t wifi_events;


	while(1) {
		swiWaitForVBlank();
		button_events = button_listener();
		wifi_events = wifi_listener();



		handle_button_events(button_events, &state);

		if(wifi_events == WIFI_FRAMERX_EVENT){
			graphics_updateHUD();
		}



		switch(state) {
			case ARMED_STATE:
				graphics_printDebug("ARMED");
			break;

			case DISARMED_STATE:
				graphics_printDebug("DISARMED");
			break;


			case DISCONNECTED_STATE:
				graphics_printDebug("DISCONNECTED");
				wifi_disconnect();
				graphics_hud_setWifiStatus(0);
			break;

			case CONNECT_WIFI_STATE:
				//search for rover and connect, otherwise block!
				graphics_printDebug("CONNECTING");
				wifi_init();
				wifi_openSocket();
				soundeff_wifiConnected();
				graphics_printDebug_SUB("CONNECTED", 1);
				state = DISARMED_STATE;
			break;
		}

	}
	return 0;
}


