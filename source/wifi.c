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
#define SSID_CHANNEL 1
#define UDP_FRAME_SIZE 64
//number of 4 byte values in an RX frame
#define VALUES_IN_FRAME 17 

//CSV dump file, header and format
#define CSV_FILE "/rxlog.csv"
#define CSV_HEAD "id,epoch,powerOut,powerIn,gps,sats,hdop,prec,lat,lon,intTemp,extTemp,intHum,extPress,wind,heading,battery,health\n"
#define CSV_LINEFORMAT "%i,%i,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"

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


uint8 wifiFrameRx[UDP_FRAME_SIZE];
int frames_received = 0;
int bytes_sent = 0;

// Flags indicating whether the WiFi or the
// socket has been initialized
bool socket_opened = false;
bool wifi_connected = false;

Wifi_AccessPoint * desiredRover;
Wifi_AccessPoint * foundRover;

//linked list head
wifiParsedFrame_t * head = NULL;

void wifi_addToList(wifiParsedFrame_t * frame){
  wifiParsedFrame_t* new_node = (wifiParsedFrame_t*)malloc(sizeof(wifiParsedFrame_t));
  if(new_node == NULL) return;

  *new_node = *frame;
  new_node->next = head;
  head = new_node; 
}


void wifi_dumpParsedFramesToLog(){
  static int busy = 0;
  if(busy) return;

  busy = 1;
  FILE * f = fopen(CSV_FILE, "w+");

  //iterate through linked list
  wifiParsedFrame_t * node = head;
  int i = 0;


  
  fprintf(f, CSV_HEAD);
  while(i<frames_received){
    fprintf(f,  CSV_LINEFORMAT,
	    i,
	    node->epoch,
	    node->powerOut,     
	    node->powerIn,      
	    node->gps,     
	    node->sats, 
	    node->hdop, 
	    node->prec, 
	    node->lat,  
	    node->lon,       
	    node->intTemp,      
	    node->extTemp,     
	    node->intHum,  
	    node->extPress,    
	    node->wind,   
	    node->heading,      
	    node->battery,      
	    node->health
	    );

    node = node->next;
    i++;
  }

  fclose(f);
  busy = 0;
}


int wifi_init(){

  if(wifi_connected) return 0;

  //init fat for logging
  fatInitDefault();

  //desired access point to connect to
  desiredRover =  malloc(sizeof(Wifi_AccessPoint));
  foundRover = malloc(sizeof(Wifi_AccessPoint));

  strcpy(desiredRover->ssid, SSID);
  desiredRover->ssid_len = strlen(SSID);
  desiredRover->channel = SSID_CHANNEL;


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

  sa_in.sin_family = AF_INET; 			//Type of address (Inet)
  sa_in.sin_port = htons(LOCAL_PORT); 	//set input port
  sa_in.sin_addr.s_addr = 0x00000000; 	//Receive data from any address

  //Bind the socket
  if(bind(socket_id, (struct sockaddr*)&sa_in, sizeof(sa_in)) < 0)
    return 0; //Error binding the socket

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

  bytes_sent += bytes;
  
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
  
  int32 values[VALUES_IN_FRAME];

  int i,j,k;
  k=0;
  for(i=0; i<VALUES_IN_FRAME; i++){

    values[i]=0;
    //assemble one value using 4 consecutive RX bytes from wifi
    for(j=0; j<sizeof(int32); j++){
      values[i] += wifiFrameRx[j+k] << 8*j;
    }
    k=k+4;
  }

  //initialise frame
  wifiParsedFrame_t frame;
  frame.epoch = (uint32)values[16];
  frame.battery = (float)values[15]/1000;
  frame.powerOut = (float)values[0]/1000;
  frame.powerIn = (float)values[1]/1000;
  frame.gps  = (float)values[2];
  frame.sats = (float)values[3];
  frame.hdop = (float)values[4];
  frame.prec = (float)values[5];
  frame.lat = (float)values[6]/1e7;
  frame.lon = (float)values[7]/1e7;
  frame.intTemp = (float)values[8]/100;
  frame.extTemp = (float)values[9]/100;
  frame.intHum = (float)values[10]/100;
  frame.extPress = (float)values[11];
  frame.wind = (float)values[12];
  frame.heading = (float)values[13]/100;
  frame.battery = (float)values[15]/1000;
  frame.health = (float)values[14];
  frame.next = NULL;

  //copy frame to linked list
  wifi_addToList(&frame);

  //set corresponding UI elements
  graphics_hud_setPowerOut(frame.powerOut);
  graphics_hud_setPowerIn(frame.powerIn);
  graphics_hud_setGPS(frame.gps);
  graphics_hud_setSats(frame.sats);
  graphics_hud_setHdop(frame.hdop);
  graphics_hud_setPrec(frame.prec);
  graphics_hud_setLat(frame.lat);
  graphics_hud_setLon(frame.lon);
  graphics_hud_setIntTemp(frame.intTemp);
  graphics_hud_setExtTemp(frame.extTemp);
  graphics_hud_setIntHum(frame.intHum);
  graphics_hud_setExtPress(frame.extPress);
  graphics_hud_setWind(frame.wind);
  graphics_hud_setHeading(frame.heading);
  graphics_hud_setBattery(frame.battery);
  graphics_hud_setHealth(frame.health);

  //sync NDS time to rover time
  timesync_setEpoch((uint32)values[16]);

}

wifi_events_t wifi_listener(){
  int bytesRx = wifi_receiveData(wifiFrameRx,VALUES_IN_FRAME*sizeof(int32));
  if(bytesRx == VALUES_IN_FRAME*sizeof(int32)){
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

int wifi_getTxByteCount(){
  return bytes_sent;
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
