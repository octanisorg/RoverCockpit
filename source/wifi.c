/*
 * wifi.c
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */



#include "wifi.h"
#include "graphics.h"
#include "timesync.h"

//Socket port
#define LOCAL_PORT 4210
#define OUT_PORT 4210
#define SSID "UDP_to_serial"


//Socket i/o configuration
struct sockaddr_in sa_out, sa_in;
int socket_id;


typedef struct wifiParsedFrame{
    float powerOut;
    float powerIn;
    float gps;
    float sats;
    float hdop;
    float prec;
    float lat;
    float lon;
    float intTemp;
    float extTemp;
    float intHum;
    float extPress;
    float wind;
    float heading;
    float battery;
    float health;
    uint32 epoch;
    struct wifiParsedFrame* next;
} wifiParsedFrame_t;


uint8 wifiFrameRx[64];
int frames_received = 0;

// Flags indicating whether the WiFi or the
// socket has been initialized
bool socket_opened = false;
bool wifi_connected = false;

Wifi_AccessPoint * desiredRover;
Wifi_AccessPoint * foundRover;

//linked list head
wifiParsedFrame_t * head = NULL;

wifiParsedFrame_t * wifi_addToList(wifiParsedFrame_t * frame, wifiParsedFrame_t * next){
	wifiParsedFrame_t* new_node = (wifiParsedFrame_t*)malloc(sizeof(wifiParsedFrame_t));
	if(new_node == NULL)
	{
		printf("Error creating a new node.\n");
		exit(0);
	}
	new_node = frame;
	new_node->next = next;

	return new_node;
}


void wifi_dumpParsedFramesToLog(){

	FILE * f = fopen("/rxlog.txt", "w+");

	//iterate through linked list
	wifiParsedFrame_t * node = head;
	int i = 0;
	while(i<3){
		fprintf(f, "e: %i", node->epoch);
		swiWaitForVBlank();
		node = node->next;
		i++;
	}

	fclose(f);
}


int wifi_init(){

	if(wifi_connected) return 0;

	//init fat for logging
	fatInitDefault();

	//desired access point to connect to
	desiredRover =  malloc(sizeof(Wifi_AccessPoint));
	foundRover = malloc(sizeof(Wifi_AccessPoint));

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

		status = Wifi_AssocStatus();
	}
}


int wifi_openSocket()
{
	//If socket already opened return 0 (error)
 	if(socket_opened == true)
		return 0;

 	frames_received = 0;
 	socket_id = socket(AF_INET,SOCK_DGRAM,0);  //UDP socket

 	//-----------Configure receiving side---------------------//

 	sa_in.sin_family = AF_INET; 			//Type of address (Inet)
	sa_in.sin_port = htons(LOCAL_PORT); 	//set input port
	sa_in.sin_addr.s_addr = 0x00000000; 	//Receive data from any address
	//Bind the socket
	if(bind(socket_id, (struct sockaddr*)&sa_in, sizeof(sa_in)) < 0)
		return 0; //Error binding the socket

	/*
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
	*/


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

int wifi_sendData(uint8* data_buff, int bytes){
	//If no socket is opened return (error)
	if(socket_opened == false)
		return -1;

	//Send the data
	sendto(	socket_id,		//Socket id
			data_buff,	//buffer of data
			bytes,		//Bytes to send
			0,			//Flags (none)
			(struct sockaddr *)&sa_in,	//Output side of the socket
			sizeof(sa_in));				//Size of the structure

	//Return always true
	return 1;
}

int wifi_receiveData(uint8* data_buff, int bytes)
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


void wifi_parseFrame(){
	int32 values[17];

	int i,j,k;
	k=0;
	for(i=0; i<17; i++){

		values[i]=0;
		for(j=0; j<4; j++){
			values[i] += wifiFrameRx[j+k] << 8*j;
		}
		k=k+4;
	}

	//add frame to linked list
	wifiParsedFrame_t * frame;
	frame->epoch = (uint32)values[16];

	head = wifi_addToList(frame, head);

	graphics_hud_setPowerOut((float)values[0]/1000);
	graphics_hud_setPowerIn((float)values[1]/1000);
	graphics_hud_setGPS((float)values[2]);
	graphics_hud_setSats((float)values[3]);
	graphics_hud_setHdop((float)values[4]);
	graphics_hud_setPrec((float)values[5]);
	graphics_hud_setLat((float)values[6]/1e7);
	graphics_hud_setLon((float)values[7]/1e7);
	graphics_hud_setIntTemp((float)values[8]/100);
	graphics_hud_setExtTemp((float)values[9]/100);
	graphics_hud_setIntHum((float)values[10]/100);
	graphics_hud_setExtPress((float)values[11]);
	graphics_hud_setWind((float)values[12]);
	graphics_hud_setHeading((float)values[13]/100);
	graphics_hud_setBattery((float)values[15]/1000);
	graphics_hud_setHealth((float)values[14]);

	timesync_setEpoch((uint32)values[16]);

}

wifi_events_t wifi_listener(){
	int bytesRx = wifi_receiveData(wifiFrameRx,68);
	if(bytesRx == 68){
		wifi_parseFrame();
		frames_received++;
		return WIFI_FRAMERX_EVENT;
	}else{
		return WIFI_RX_EVENT;
	}
}

int wifi_getRxFrameCount(){
	return frames_received;
}

void wifi_disconnect(){
	if(wifi_connected){
		wifi_closeSocket();
		Wifi_DisconnectAP();
		wifi_connected = false;
		free(foundRover);
		free(desiredRover);
	}
}
