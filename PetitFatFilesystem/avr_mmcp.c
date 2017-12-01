/*-------------------------------------------------------------------------*/
/* PFF - Low level disk control module for AVR            (C)ChaN, 2014    */
/*                                                                         */
/* Adapted and modified for the SEP course in 2015. Some modifications     */
/* taken from the Arduino port of the PFF library. See the following links */
/* for more information.                                                   */
/* http://elm-chan.org/fsw/ff/00index_p.html                               */
/* https://github.com/greiman/PetitFS                                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include <avr/io.h>
#include <util/delay_basic.h>

#include "../SPI/SPI_implement_me.h"

#include "pff.h"
#include "diskio.h"
#include "integer.h"




/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*                                                                         */
/*         ADAPT THE FOLLOWING CODE TO PORT THE LIBRARY                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

// MCU Clock Speed - needed for proper delay functions
#define F_CPU	16000000UL


// CS Pin and other definitions
// ADAPT THIS if you use another CS pin for the SD card!
#define SELECT()  (PORTB &= ~ (1 << PB2))	 	/* CS = L */
#define	DESELECT()	(PORTB |= (1 << PB2))	/* CS = H */
#define	SELECTING	!(PORTB & (1 << PB2))	/* CS status (true:CS low) */
#define	FORWARD(d)	xmit(d)			/* Data forwarding function (console out) */


/* This function initialises the SPI bus, the chip select pin etc.
 *
 * STUDENTS: To make it easier, this function is just a wrapper for SPI_Master_Init()
 * in SPI_implement_me.[{c|h}. See there, there is code missing!
 */
static void init_spi (void)
{
  SPI_Master_Init();
}



/* Send a byte to the card
 *
 * STUDENTS: To make it easier, this function is just a wrapper for SPI_Master_transmit_char()
 * in SPI_implement_me.[{c|h}. See there, there is code missing!
 */
inline void xmit_spi(BYTE d)
{
   SPI_Master_transmit_char(d,0,0);
}



/* Receive a byte from the card
 *
 * STUDENTS: To make it easier, this function is just a wrapper for SPI_Master_receive_char()
 * in SPI_implement_me.[{c|h}. See there, there is code missing!
 */
inline BYTE rcv_spi (void)
{
  return SPI_Master_receive_char();
}



/* Waits for more or less exactly 100µs */
static void dly_100us (void)
{
  // each count delays four CPU cycles.
  _delay_loop_2(F_CPU/(40000));
}



/* "Forwarding" function. If no working buffer is passed to the disk_readp function,
 * it will "forward" the read bytes to some other place instead (e..g a text console).
 * This forwarding should be implemented here. As this example application always passes
 * a buffer, the xmit function is not needed. Bytes passed to it simply get lost.
 */
static void xmit (__attribute__((unused)) char getslost)
{
  return;
}



/*-------------------------------------------------------------------------*/
/* End of platform dependent macros and functions. Do not touch the code   */
/* below unless you know what you are doing.                               */
/*-------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_BLOCK			0x08	/* Block addressing */


static
BYTE CardType;



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* 1st byte (Start + Index) */
	DWORD arg		/* Argument (32 bits) */
)
{
	BYTE n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	DESELECT();
	rcv_spi();
	SELECT();
	rcv_spi();

	/* Send a command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcv_spi();
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}




/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if _USE_WRITE
	if (CardType && SELECTING) disk_writep(0, 0);	/* Finalize write process if it is in progress */
#endif
	init_spi();		/* Initialize ports to control MMC */
	DESELECT();

	//PORTD |= (1<<PD5);

	for (n = 10; n; n--) rcv_spi();	/* 80 dummy clocks with CS=H */
	PORTD |= (1<<PD5);


	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* GO_IDLE_STATE */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2 */
			for (n = 0; n < 4; n++) ocr[n] = rcv_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {			/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) dly_100us();	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (tmr && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcv_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 (HC or SC) */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--) dly_100us();	/* Wait for leaving idle state */
			if (!tmr || send_cmd(CMD16, 512) != 0)			/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	DESELECT();
	rcv_spi();

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Forward to the stream) */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Byte offset to read from (0..511) */
	UINT count		/* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE rc;
	UINT bc;


	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {	/* READ_SINGLE_BLOCK */

		bc = 40000;	/* Time counter */
		do {				/* Wait for data packet */
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {	/* A data packet arrived */

			bc = 512 + 2 - offset - count;	/* Number of trailing bytes to skip */

			/* Skip leading bytes */
			while (offset--) rcv_spi();

			/* Receive a part of the sector */
			if (buff) {	/* Store data to the memory */
				do {
					*buff++ = rcv_spi();
				} while (--count);
			} else {	/* Forward data to the outgoing stream */
				do {
					FORWARD(rcv_spi());
				} while (--count);
			}

			/* Skip trailing bytes and CRC */
			do rcv_spi(); while (--bc);

			res = RES_OK;
		}
	}

	DESELECT();
	rcv_spi();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_writep (
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sc			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	UINT bc;
	static UINT wc;	/* Sector write counter */

	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = sc;
		while (bc && wc) {		/* Send data bytes to the card */
			xmit_spi(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF); xmit_spi(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) xmit_spi(0);	/* Fill left bytes and CRC with zeros */
			if ((rcv_spi() & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; rcv_spi() != 0xFF && bc; bc--)	/* Wait for ready */
					dly_100us();
				if (bc) res = RES_OK;
			}
			DESELECT();
			rcv_spi();
		}
	}

	return res;
}
#endif
