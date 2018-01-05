/*
 * spi.h
 *	Code mostly from ndsmotion.c of libnds.
 *  Created on: Jan 1, 2018
 *      Author: nds
 *
 *      Tested with below code and a modified cart. Works!
			spi_on();
			spi_shiftbyte(0x41);
			spi_off();
 *
 */

#ifndef SPI_H_
#define SPI_H_


#define REG_AUXSPICNT	(*(vu16*)0x040001A0)
#define REG_AUXSPIDATA	(*(vu8*)0x040001A2)


// enables SPI bus at 4.19 MHz
void spi_on();

// disables SPI bus
void spi_off();

unsigned char spi_shiftbyte(unsigned char in_byte);

#endif /* SPI_H_ */
