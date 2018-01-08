/*
 * graphics.c
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#include "graphics.h"
#include <time.h>
#include "wifi.h"
#include "timesync.h"

#include "CGA8x8thick.h"
#include "mainHUD.h"
#include "AH.h"
#include "compassSprite.h"
#include "Background.h"



#define NEXTLINE 32
#define POWER_IN NEXTLINE + 1
#define POWER_OUT (POWER_IN + 3*NEXTLINE)
#define INT_TEMP (POWER_OUT + 6*NEXTLINE)
#define EXT_TEMP (INT_TEMP + 3*NEXTLINE)
#define INT_HUM (EXT_TEMP + 3*NEXTLINE)
#define EXT_PRESS (INT_HUM + 3*NEXTLINE)
#define WIND (EXT_PRESS + 3*NEXTLINE)

#define GPS (10)
#define HDOP (17)
#define LAT (23)

#define SATS (GPS + NEXTLINE + 1)
#define PREC (HDOP + NEXTLINE)
#define LON (LAT + NEXTLINE)
#define HEADING (13*NEXTLINE + 18)
#define DEBUGMSG 4*NEXTLINE + 9
#define FIRTST_BUT_POS 7*NEXTLINE +18
#define NDSCONN 19*NEXTLINE + 27

#define BATTERY 15*NEXTLINE + 27
#define HEALTH 11*NEXTLINE + 27


#define WHITE_OFFSET 256
#define RED_OFFSET 2*256
#define GREEN_OFFSET 3*256

hudData_t hudData;
u16 * map = BG_MAP_RAM(26);
u16 * map_SUB = BG_MAP_RAM_SUB(26);
u16 * compassSprite;



void graphics_printChar(int index, u16 c){
	map[index] = c; //16 bit CHAR!!!!!!! otherwise color will be truncated!!!!
}

void graphics_printChar_SUB(int index, u16 c){
	map_SUB[index] = c+WHITE_OFFSET; //16 bit CHAR!!!!!!! otherwise color will be truncated!!!!
}


void graphics_print(int index, char * word ){

	int l = 0;
	while(*(word+l) != '\0'){
		graphics_printChar(index+l, *(word+l));
		l++;
	}

}

void graphics_print_SUB(int index, char * word ){

	int l = 0;
	while(*(word+l) != '\0'){
		graphics_printChar_SUB(index+l, *(word+l));
		l++;
	}

}

void graphics_hud_setPowerIn(float f){
	hudData.power_in = f;
}

void graphics_hud_setPowerOut(float f){
	hudData.power_out = f;
}

void graphics_hud_setSats(float f){
	hudData.sats = f;
}

void graphics_hud_setGPS(float f){
	hudData.gps = f;
}

void graphics_hud_setLat(float f){
	hudData.lat = f;
}

void graphics_hud_setLon(float f){
	hudData.lon = f;
}

void graphics_hud_setWind(float f){
	hudData.wind = f;
}

void graphics_hud_setExtPress(float f){
	hudData.ext_press = f;
}


void graphics_hud_setHdop(float f){
	hudData.hdop = f;
}
void graphics_hud_setPrec(float f){
	hudData.prec = f;
}
void graphics_hud_setIntTemp(float f){
	hudData.int_temp = f;
}
void graphics_hud_setExtTemp(float f){
	hudData.ext_temp = f;
}
void graphics_hud_setHeading(float f){
	hudData.heading = f;
}

void graphics_hud_setBattery(float f){
	hudData.battery = f;
}

void graphics_hud_setHealth(float f){
	hudData.health = f;
}

void graphics_hud_setIntHum(float f){
	hudData.int_hum = f;
}


void graphics_hud_setWifiStatus(int status){
	if(status){
		graphics_printChar(NDSCONN, 'O'+GREEN_OFFSET);
		graphics_printChar(NDSCONN+1, 'K'+GREEN_OFFSET);
		graphics_printChar(NDSCONN+2, 29+GREEN_OFFSET);

	}else{
		graphics_printChar(NDSCONN, 'N'+RED_OFFSET);
		graphics_printChar(NDSCONN+1, 'O'+RED_OFFSET);
		graphics_printChar(NDSCONN+2, 'K'+RED_OFFSET);
	}
}

void graphics_compassInit(){
	//Set up memory bank to work in sprite mode (offset since we are using VRAM A for backgrounds)
	VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_SPRITE_0x06400000;

	//Initialize sprite manager and the engine
	oamInit(&oamMain, SpriteMapping_1D_64, false);

	//Allocate space for the graphic to show in the sprite
	compassSprite = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);

	//Copy data for the graphic (palette and bitmap)
	swiCopy(compassSpritePal, SPRITE_PALETTE, compassSpritePalLen/2);
	swiCopy(compassSpriteTiles, compassSprite, compassSpriteTilesLen/2);
}


void graphics_mainInit(){


	//SCREEN FONT TEST
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG1_ACTIVE| DISPLAY_BG0_ACTIVE;
	BGCTRL[1] = BG_32x32 | BG_COLOR_16 | BG_MAP_BASE(26) | BG_TILE_BASE(4);
	BGCTRL[0] = BG_32x32 | BG_COLOR_16 | BG_MAP_BASE(0) | BG_TILE_BASE(1);

	swiCopy(CGA8x8thickTiles, BG_TILE_RAM(4), CGA8x8thickTilesLen/2);
	swiCopy(CGA8x8thickPal, BG_PALETTE, CGA8x8thickPalLen/2);

	swiCopy(mainHUDTiles, BG_TILE_RAM(1), mainHUDTilesLen/2);
	swiCopy(mainHUDMap, BG_MAP_RAM(0), mainHUDMapLen/2);


	//clear map of main screen
	int i;
	for(i=0; i<1024; i++) map[i] = 0;


	//setup compass
	graphics_compassInit();

	//Position
	oamSet(&oamMain, 	// oam handler
		0,				// Number of sprite
		124,120,			// Coordinates
		1,				// Priority
		0,				// Palette to use
		SpriteSize_64x64,			// Sprite size
		SpriteColorFormat_256Color,	// Color format
		compassSprite,			// Loaded graphic to display
		1,				// Affine rotation to use (-1 none)
		false,			// Double size if rotating
		false,			// Hide this sprite
		false, false,	// Horizontal or vertical flip
		false			// Mosaic
		);


		//Update the sprites
		oamUpdate(&oamMain);

}

void graphics_subInit(){

	VRAM_C_CR = VRAM_ENABLE	| VRAM_C_SUB_BG;
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE;

	BGCTRL_SUB[1] = BG_32x32 | BG_COLOR_16 | BG_MAP_BASE(0) | BG_TILE_BASE(1);
	BGCTRL_SUB[0] = BG_32x32 | BG_COLOR_16 | BG_MAP_BASE(26) | BG_TILE_BASE(2);


	swiCopy(BackgroundTiles,BG_TILE_RAM_SUB(1), BackgroundTilesLen/2);
    swiCopy(CGA8x8thickPal, BG_PALETTE_SUB, CGA8x8thickPalLen/2);
    swiCopy(CGA8x8thickTiles,BG_TILE_RAM_SUB(2), CGA8x8thickTilesLen/2);
    swiCopy(BackgroundMap, BG_MAP_RAM_SUB(0), BackgroundMapLen/2);

	//clear map of sub screen
    int i;
	for(i=0; i<1024; i++) map_SUB[i] = 0;

}




void graphics_printFloat(int index, float f, char unit, char * format){
	char word[8];
	sprintf(word, format, f);

	int len = strlen(word);
	word[len] = unit;
	word[len+1] = '\0';

	graphics_print(index, word);
}

void graphics_printDebug(char * word){
	int i;
	for(i=0; i<24; i++) map[DEBUGMSG + i] = 0; //clear line;
	graphics_print(DEBUGMSG, word);
	}

void graphics_printDebug_SUB(char * word, int button){
	int i;
	int position;
	position=FIRTST_BUT_POS+5*button*NEXTLINE;
	for(i=0; i<24; i++) map_SUB[position + i] = 0; //clear line
	graphics_print_SUB(position,word);

	}

void graphics_printDebug2(char * word){
	int i;
	for(i=0; i<24; i++) map[DEBUGMSG + NEXTLINE + i] = 0; //clear line
	graphics_print(DEBUGMSG + NEXTLINE, word);
}


void graphics_updateHUD(){

	graphics_printFloat(INT_TEMP, hudData.int_temp, 'C', "%2.1f");
	graphics_printFloat(EXT_TEMP, hudData.ext_temp, 'C', "%2.1f");
	graphics_printFloat(INT_HUM, hudData.int_hum, '%', "%2.1f");

	graphics_printFloat(LAT, hudData.lat, 248, "%2.4f"); //TX
	graphics_printFloat(LON, hudData.lon, 248, "%2.4f"); //RX
	graphics_printFloat(SATS, hudData.sats, ' ', "%02.0f");
	graphics_printFloat(EXT_PRESS, hudData.ext_press, 135, "%02.0f");
	graphics_printFloat(WIND, hudData.wind, 255, "%02.0f");

	graphics_printFloat(GPS, hudData.gps, ' ', "%02.0f");
	graphics_printFloat(PREC, hudData.prec, 'm', "%02.0f");
	graphics_printFloat(HDOP, hudData.hdop, ' ', "%02.0f");

	graphics_printFloat(POWER_IN, hudData.power_in, 'W', "%05.02f");
	graphics_printFloat(POWER_OUT, hudData.power_out, 'W', "%05.02f");
	graphics_printFloat(BATTERY, hudData.battery, 'V', "%02.2f");
	graphics_printFloat(HEALTH, hudData.health, '%', "%02.0f");

	graphics_printFloat(HEADING, hudData.heading, 248, "%03.0f");

	//update rx/tx frames
	graphics_printFloat(NDSCONN + NEXTLINE, (float)wifi_getRxFrameCount(),25, "%3.0f");
	graphics_printFloat(NDSCONN + 2*NEXTLINE, (float)wifi_getTxByteCount(),24, "%3.0f");





	//rotate compass
	float dalpha = (32767)/360;
	float alpha = hudData.heading*dalpha;
	oamRotateScale(&oamMain, 1, alpha, 256,256);
	oamUpdate(&oamMain);

	//emit warning sounds
	if(hudData.battery <= 3.4) soundeff_batteryLow();

}


