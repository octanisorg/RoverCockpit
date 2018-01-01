/*
 * timesync.c
 *
 *  Created on: Dec 22, 2017
 *      Author: nds
 */


#include "timesync.h"
#include "graphics.h"
#include <string.h>

struct tm  * rover_utc;
uint32 rover_epoch = 0;

void timesync_tick(){
	rover_epoch++;
	//print time
	uint32 t = timesync_getEpoch();
	struct tm * tt = gmtime(&t);
	char buffer[80];

	strftime(buffer,80,"%Y-%m-%dT%H:%M:%S UTC", tt);
	graphics_printDebug2(buffer);
}

void timesync_init(){
	TIMER_CR(0) = TIMER_ENABLE | TIMER_DIV_1024 | TIMER_IRQ_REQ;
	TIMER_DATA(0) = TIMER_FREQ_1024(1);

	irqSet(IRQ_TIMER0, &timesync_tick);
	irqEnable(IRQ_TIMER0);
}

void timesync_setEpoch(uint32 epoch){
	rover_epoch = epoch;
}


uint32 timesync_getEpoch(){
	return rover_epoch;
}
uint8 timesync_getMin(){
	return rover_utc->tm_min;
}
uint8 timesync_getHour(){
	return rover_utc->tm_hour;
}
uint8 timesync_getSec(){
	return rover_utc->tm_sec;
}
uint8 timesync_getDay(){
	return rover_utc->tm_mday;
}
uint8 timesync_getMonth(){
	return rover_utc->tm_mon;
}
uint8 timesync_getYear(){
	return rover_utc->tm_year;
}




