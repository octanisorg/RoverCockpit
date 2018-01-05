/*
 * spi.c
 *
 *  Created on: Jan 1, 2018
 *      Author: nds
 */
#include "spi.h"
#include <nds.h>
#include <nds/system.h>
#include <stdio.h>
#include "nds/card.h"
#include "nds/dma.h"
#include "nds/memory.h"
#include "nds/bios.h"


void spi_waitBusy(){
	   while (REG_AUXSPICNT & /*BUSY*/0x80);
}

// sends and receives 1 byte on the SPI bus
unsigned char spi_shiftbyte(unsigned char in_byte){

	unsigned char out_byte;
	REG_AUXSPIDATA = in_byte; // send the output byte to the SPI bus
	spi_waitBusy(); // wait for transmission to complete
	out_byte=REG_AUXSPIDATA; // read the input byte from the SPI bus
	return out_byte;
}


void spi_on(){
	 REG_AUXSPICNT = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40 | 0;
}

void spi_off(){
	 REG_AUXSPICNT = 0;
}
