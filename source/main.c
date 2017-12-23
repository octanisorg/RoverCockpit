#include <nds.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dswifi9.h>
#include "buttons.h"
#include "graphics.h"
#include "wifi.h"

typedef enum {
	ARMED_STATE,
	DISARMED_STATE,
	CONNECT_WIFI_STATE,
	DISCONNECTED_STATE
} main_states_t;



void handle_button_events(button_events_t button_events, main_states_t * state){

	switch(button_events)
	{
		case BUTTON_ARM_EVENT:

			//if armed successful
				//state = ARMED_STATE;
			//else
				//state = IDLE_STATE

			*state = ARMED_STATE;
		break;

		case BUTTON_WIFI_CONNECT_EVENT:
			*state = CONNECT_WIFI_STATE;
		break;

		case BUTTON_SAVE_EVENT:
			//save
		break;

		case BUTTON_L_EVENT:
		case BUTTON_R_EVENT:
		case BUTTON_U_EVENT:
		case BUTTON_D_EVENT:

			//if ARMED

		break;

	}
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	graphics_mainInit();
	graphics_printDebug("TEST");

	//wifi test
	wifi_init();

	//Open Socket
	if(wifi_openSocket()){
		graphics_printDebug("SOCKET OPEN");
	}

	char msg[1];
	msg[0] = 'a';
	if(wifi_sendData(msg,1) == 1){
		graphics_printDebug("SENT         ");
	}else{
		graphics_printDebug("DATA NOT SENT");
	}


	wifi_receiveData(msg, 1);
	graphics_printDebug("             ");
	graphics_printDebug(msg);


	wifi_closeSocket();



	/*
	main_states_t state = IDLE_STATE;
	button_events_t button_events;

	while(1) {

		swiWaitForVBlank();
		//button_events = button_listener();
		//wifi_events = wifi_listener();

		switch(state) {
			case IDLE_STATE:

				//prioritize by ordering
				//handle_wifi_events(wifi_events, &state);
				handle_button_events(button_events, &state);

			break;

			case DRAW_ARROW_STATE:
				//TODO: graphics_sub_arrow(button_events)

				state = IDLE_STATE;
			break;


			case ARMED_STATE:
				handle_button_events(button_events, &state);
				//if armed successful
					//state = ARMED_STATE;
				//else
					//state = IDLE_STATE
			break;


			case WIFI_CONNECT_STATE:

			break;
		}

	}
	*/

	return 0;
}
