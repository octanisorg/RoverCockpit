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

//WiFi initialization
int wifi_init();
//Socket initialization
int wifi_openSocket();
//Receive data trough the socket
int wifi_receiveData(char* data_buff, int bytes);
//Send data through the socket
int wifi_sendData(char* data_buff, int bytes);
//Close the Socket
void wifi_closeSocket();



#endif /* WIFI_H_ */
