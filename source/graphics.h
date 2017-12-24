/*
 * graphics.h
 *
 *  Created on: Dec 20, 2017
 *      Author: nds
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "math.h"
#include "soundeff.h"

typedef struct{
    float power_in;
    float power_out;
    float gps;
    float sats;
    float hdop;
    float prec;
    float lat;
    float lon;
    float int_temp;
    float ext_temp;
    float int_hum;
    float ext_press;
    float wind;
    float heading;
    float health;
    float battery;
} hudData_t;

void graphics_hud_setGPS(float f);
void graphics_hud_setSats(float f);
void graphics_hud_setLat(float f);
void graphics_hud_setLon(float f);
void graphics_hud_setWind(float f);
void graphics_hud_setPowerOut(float f);
void graphics_hud_setPowerIn(float f);
void graphics_hud_setExtPress(float f);
void graphics_hud_setHdop(float f);
void graphics_hud_setPrec(float f);
void graphics_hud_setIntTemp(float f);
void graphics_hud_setExtTemp(float f);
void graphics_hud_setHeading(float f);
void graphics_hud_setBattery(float f);
void graphics_hud_setHealth(float f);
void graphics_hud_setIntHum(float f);

void graphics_printDebug(char * word);
void graphics_printDebug2(char * word);

void graphics_hud_setWifiStatus(int status);
void graphics_updateHUD();
void graphics_mainInit();



#endif /* GRAPHICS_H_ */
