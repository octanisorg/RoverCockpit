/*
 * sound.c
 *
 *  Created on: Dec 22, 2017
 *      Author: nds
 */

#include <nds.h>
#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"
#include "soundeff.h"
#include "timesync.h"

#define SOUNDEFF_LIMIT_SEC 5

uint32 lastPlayed = 0;

//limit rate of sound effects
int isPlayable(){
	if((timesync_getEpoch() - lastPlayed) < SOUNDEFF_LIMIT_SEC){
		return 0;
	}else{
		lastPlayed = timesync_getEpoch();
		return 1;
	}
}

void soundeff_init(){

	//Init the sound library
	mmInitDefaultMem((mm_addr)soundbank_bin);
	mmLoadEffect(SFX_TOOLOW);
	mmLoadEffect(SFX_CHIMEHIGH);
	mmLoadEffect(SFX_CHIMELOW);
	mmLoadEffect(SFX_CHIME2);
	isPlayable();
}

void soundeff_batteryLow(){
	if(isPlayable()) mmEffect(SFX_TOOLOW);
}

void soundeff_stateArmed(){
	if(isPlayable()) mmEffect(SFX_CHIMEHIGH);
}

void soundeff_stateDisarmed(){
	if(isPlayable()) mmEffect(SFX_CHIMELOW);
}

void soundeff_wifiConnected(){
	if(isPlayable()) mmEffect(SFX_CHIME2);
}

void soundeff_wifiFramesSaved(){
	mmEffect(SFX_CHIME2);
}
