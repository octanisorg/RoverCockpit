/*
 * wifi.c
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */



#include "wifi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dswifi9.h>
#include "graphics.h"

//Socket port
#define LOCAL_PORT 4210
#define OUT_PORT 4210
#define SSID "UDP_to_serial"

//Socket i/o configuration
struct sockaddr_in sa_out, sa_in;
int socket_id;


// Flags indicating whether the WiFi or the
// socket has been initialized
bool socket_opened = false;
bool wifi_connected = false;

int wifi_init(){

	if(wifi_connected) return 0;

	//desired access point to connect to
	Wifi_AccessPoint * desiredRover =  malloc(sizeof(Wifi_AccessPoint));
	Wifi_AccessPoint * foundRover = malloc(sizeof(Wifi_AccessPoint));

	strcpy(desiredRover->ssid, SSID);
	desiredRover->ssid_len = 13;
	desiredRover->channel = 1;


	Wifi_InitDefault(false);
	int status = ASSOCSTATUS_DISCONNECTED;
	int inRange = -1;

	while(1) {
		swiWaitForVBlank();

		//is the rover around?
		if(status != ASSOCSTATUS_ASSOCIATED){
			Wifi_ScanMode(); //should be called every time you want Wifi_FindMatchingAP to work
			inRange = Wifi_FindMatchingAP(1, desiredRover, foundRover);
			if(inRange == 0){

				Wifi_SetIP(0,0,0,0,0);
				Wifi_ConnectAP(foundRover, WEPMODE_NONE, 0, 0);

				//block until connected
				while(status != ASSOCSTATUS_ASSOCIATED && status != ASSOCSTATUS_CANNOTCONNECT) {
					status = Wifi_AssocStatus();
					graphics_hud_setWifiStatus(status);
					swiWaitForVBlank();
				}

			}else{
				//keep searching
				graphics_hud_setWifiStatus(0);
			}

		}else{
			//connected!
			graphics_hud_setWifiStatus(1);
			wifi_connected = true;
			return 1;
		}

		graphics_updateHUD();
		status = Wifi_AssocStatus();
	}
}


int wifi_openSocket()
{
	//If socket already opened return 0 (error)
 	if(socket_opened == true)
		return 0;

 	socket_id = socket(AF_INET,SOCK_DGRAM,0);  //UDP socket

 	//-----------Configure receiving side---------------------//

 	sa_in.sin_family = AF_INET; 			//Type of address (Inet)
	sa_in.sin_port = htons(LOCAL_PORT); 	//set input port
	sa_in.sin_addr.s_addr = 0x00000000; 	//Receive data from any address
	//Bind the socket
	if(bind(socket_id, (struct sockaddr*)&sa_in, sizeof(sa_in)) < 0)
		return 0; //Error binding the socket

	//-----------Configure sending side-----------------------//

	sa_out.sin_family = AF_INET;			//Type of address (Inet)
	sa_out.sin_port = htons(OUT_PORT);		//set output port (same as input)

	//Retrieve network parameters to obtain the broadcast address
	struct in_addr gateway, snmask, dns1, dns2;
	Wifi_GetIPInfo(&gateway, &snmask, &dns1, &dns2);
	unsigned long ip = Wifi_GetIP();
	unsigned long mask = snmask.s_addr;

	//Calculate broadcast address
	unsigned long broadcast_addr = ip | ~mask;

	//Destination address (broadcast)
	sa_out.sin_addr.s_addr = broadcast_addr;

	//Set socket to be non-blocking
	char nonblock = 1;
	ioctl(socket_id, FIONBIO, &nonblock);

	//Return successful flag
	socket_opened = 1;
	return socket_opened;
}

void wifi_closeSocket()
{
	//If socket not opened, nothing to do
	if(socket_opened == false)
		return;

	//Shutdown and close the socket in both directions
	shutdown(socket_id, SHUT_RDWR);
	closesocket(socket_id);
	socket_opened = false;
}

int wifi_sendData(char* data_buff, int bytes){
	//If no socket is opened return (error)
	if(socket_opened == false)
		return -1;

	//Send the data
	sendto(	socket_id,		//Socket id
			data_buff,	//buffer of data
			bytes,		//Bytes to send
			0,			//Flags (none)
			(struct sockaddr *)&sa_out,	//Output side of the socket
			sizeof(sa_out));				//Size of the structure

	//Return always true
	return 1;
}

int wifi_receiveData(char* data_buff, int bytes)
{
	int received_bytes;
	int info_size = sizeof(sa_in);

	//If no socket is opened, return (error)
	if(socket_opened == false)
		return -1;

	//Try to receive the data
	received_bytes = recvfrom(
			socket_id,		//Socket id
			data_buff,	//Buffer where to put the data
			bytes, 		//Bytes to receive (at most)
			~MSG_PEEK,	//Returned data is marked as read
			(struct sockaddr *)&sa_in, 	//Sender information
			&info_size); 					//Sender info size

	//Discard data sent by ourselves
	if(sa_in.sin_addr.s_addr == Wifi_GetIP())
		return 0;

	//Return the amount of received bytes
	return received_bytes;
}

