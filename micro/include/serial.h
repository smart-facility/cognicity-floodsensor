/* -*- C++ -*- */
/*
    Copyright (C) 2016 William Brodie-Tyrrell
    william@brodie-tyrrell.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

/**
 * @file serial.h
 * Device driver for USART.
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "cmsis_device.h"
#include "conditions.h"
#include "bbuffer.h"
#include "dmachan.h"

#define __need_NULL
#include <stddef.h>
#include <assert.h>

extern "C" void USART1_IRQHandler(void);
#if __CORTEX_M >= 3
extern "C" void USART2_IRQHandler(void);
extern "C" void USART3_IRQHandler(void);
#endif

/**
 * USART interface.  Received data goes (via interrupts) into
 * a bounded_buffer.  DMA used for transmission only.
 *
 * Configures GPIO only for RX and TX, does nothing for RTS/CTS or CLK.
 */
class Serial {
	static constexpr uint32_t DMA_PRIORITY=DMA_Priority_Medium;
	static constexpr uint8_t UARTIRQ_PRIORITY=3;
	static constexpr uint8_t DMAIRQ_PRIORITY=3;

public:

	Serial(USART_TypeDef *u, bounded_buffer *rxd);
	~Serial();

	/// arbitrary-mode setup
	void setup(USART_InitTypeDef *mode);
	/// 8N1, no flow control, full duplex.
	void setup(uint32_t baud);

	/// send a collection of bytes.  Assumes no 9-bit data.
	void transmit(uint32_t count, const uint8_t *data);
	/// send a collection of bytes
	void transmit(const buffer &b);
	/// send a C-string
	void print(const char *p);
	/// send a C-string plus CRLF
	void println(const char *p);

	/// determine whether the receive-buffer has overflowed and
	/// data was lost.  Clears the data-lost flag when called.
	bool receiveOverflow();

private:

	void onInterrupt();

	USART_TypeDef *dev;		///< pointer to underlying hardware
#ifndef SINGLETHREAD
	Mutex mutex;			///< atomic transmissions
	SemDelegate semdel;		///< waiting for DMA complete
	DMAChannel *dmawr;
#endif
	bounded_buffer *rxbuf;	///< receive-buffer
	bool rx_overflow;

	static Serial *uart1;
#if __CORTEX_M >= 3
	static Serial *uart2, *uart3;
#endif

	friend void USART1_IRQHandler();
#if __CORTEX_M >= 3
	friend void USART2_IRQHandler();
	friend void USART3_IRQHandler();
#endif
};


#endif /* SERIAL_H_ */
