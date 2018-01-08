/*
 * wifi.h
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <nds.h>
#include <dswifi9.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef enum {
	WIFI_RX_EVENT,
	WIFI_FRAMERX_EVENT
} wifi_events_t;


//WiFi initialization
int wifi_init();
//Socket initialization
int wifi_openSocket();
//Receive data trough the socket
int wifi_receiveData(uint8* data_buff, int bytes);
//Send data through the socket
int wifi_sendData(uint8* data_buff, int bytes);
//Close the Socket
void wifi_closeSocket();

void wifi_disconnect();

//produces events
wifi_events_t wifi_listener();

int wifi_getRxFrameCount();

int wifi_getTxByteCount();

void wifi_dumpParsedFramesToLog();

#endif /* WIFI_H_ */
