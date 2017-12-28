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


void soundeff_init(){

	//Init the sound library
	mmInitDefaultMem((mm_addr)soundbank_bin);
	mmLoadEffect(SFX_TOOLOW);
	mmLoadEffect(SFX_CHIMEHIGH);
	mmLoadEffect(SFX_CHIMELOW);
	mmLoadEffect(SFX_CHIME2);

}

void soundeff_batteryLow(){
	//TODO: time limit alerts!
	//mmEffect(SFX_TOOLOW);
}

void soundeff_stateArmed(){
	mmEffect(SFX_CHIMEHIGH);
}

void soundeff_stateDisarmed(){
	mmEffect(SFX_CHIMELOW);
}

void soundeff_wifiConnected(){
	mmEffect(SFX_CHIME2);
}

