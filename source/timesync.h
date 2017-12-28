/*
 * timesync.h
 *
 *  Created on: Dec 22, 2017
 *      Author: nds
 */

#ifndef TIMESYNC_H_
#define TIMESYNC_H_

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

void timesync_init();
void timesync_setEpoch(uint32 epoch);
uint32 timesync_getEpoch();
uint8 timesync_getMin();
uint8 timesync_getHour();
uint8 timesync_getSec();
uint8 timesync_getDay();
uint8 timesync_getMonth();
uint8 timesync_getYear();

#endif /* TIMESYNC_H_ */
