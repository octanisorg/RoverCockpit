#include <nds.h>
#include <stdio.h>



#include "scientifica16.h"
#include "nintendo8.h"
#include "mainHUD.h"
#include "Background.h"



//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	//SCREEN FONT TEST
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE;
	//BGCTRL[2] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;
	BGCTRL[1] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(26) | BG_TILE_BASE(4);
	BGCTRL[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);


	//swiCopy(scientifica16Tiles, BG_TILE_RAM(1), scientifica16TilesLen/2);
	swiCopy(nintendo8Tiles, BG_TILE_RAM(4), nintendo8TilesLen/2);
	swiCopy(nintendo8Pal, BG_PALETTE, nintendo8PalLen/2);

	swiCopy(mainHUDTiles, BG_TILE_RAM(1), mainHUDTilesLen/2);
	swiCopy(mainHUDMap, BG_MAP_RAM(0), mainHUDMapLen/2);
	//swiCopy(mainHUDPal, BG_PALETTE, mainHUDPalLen/2);


	uint16 * map = BG_MAP_RAM(26);

	//clear map
	int i;
	for(i=0; i<1024; i++) map[i] = 55;
	map[34] = 9;

	//can print up to 16 chars per line, each char 16x16px
	void printChar8(uint16 * map, int charIndex, int x, int y){
		int origin = x*2+y*64;
		map[origin] = charIndex*2;
		map[origin+1] = charIndex*2 + 1;
		map[origin+32] = charIndex*2+64;
		map[origin+32+1] = charIndex*2+1+64;

	}



/*
	printChar16(map, '3', 0,0);
	printChar16(map, '.', 1,0);
	printChar16(map, '7', 2,0);
	printChar16(map, 'v', 3,0);
	printChar16(map, 'H', 3,1);
*/

	/*
	char * b = "Battery";

	int y=0;
	int i = 0;
	while(*b != '\0'){

		printChar16(map, (uint8)*b, 2*i++,y);
		b++;
	}
*/

	//initialize background of the sub engine:

	VRAM_C_CR = VRAM_ENABLE	| VRAM_C_SUB_BG;
	REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG0_ACTIVE;

	BGCTRL_SUB[0] = BG_32x32 | BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1);

	swiCopy(BackgroundTiles,BG_TILE_RAM_SUB(1), BackgroundTilesLen/2);
    swiCopy(BackgroundPal, BG_PALETTE_SUB, BackgroundPalLen/2);
    swiCopy(BackgroundMap, BG_MAP_RAM_SUB(0), BackgroundMapLen/2);



	while(1) {
		swiWaitForVBlank();
	}
	return 0;
}


