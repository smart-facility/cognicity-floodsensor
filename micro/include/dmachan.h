/* -*- C++ -*- */
/*
    Copyright (C) 2015-2016 William Brodie-Tyrrell
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
 * @file dmachan.h
 * DMA-channel abstraction
 */


#ifndef DMACHAN_H_
#define DMACHAN_H_

#include "cmsis_device.h"
#include "conditions.h"
#include "intchain.h"


/**
 * Increment a semaphore when receiving an IRQ; handy for
 * waking a thread on transfer-complete.
 */
class SemDelegate : public IRQDelegate {
public:
	virtual void onIRQ();

	Semaphore sem;
};

extern "C" void DMA1_Channel1_IRQHandler(void);
extern "C" void DMA1_Channel2_3_IRQHandler(void);
extern "C" void DMA1_Channel4_5_IRQHandler(void);


/**
 * Control over a single DMA channel, permitting it to be
 * shared between drivers for multiple devices.
 */
class DMAChannel {
public:

	static constexpr uint8_t DMAIRQ_PRIORITY=1;

	/// blocking-wait until channel is available for use
	/// driver should enable IRQ-request line on relevant device after this
	/// MUST be called from a thread, not interrupt
	/// @param driver pointer to driver that will own the channel until release
	/// @return true if no other device has owned/modified the channel since this newly-requested owner
	bool claim(IRQDelegate *driver);

#if SINGLETHREAD
	/// is this channel in use, i.e. between claim()/release() ?
	bool claimed() const { return isclaimed; }
#endif

	/// turn on a DMA channel and enable interrupts
	void enable();

	/// disable and relinquish the channel so that another driver may acquire it
	/// driver must disable the IRQ-request line on relevant device first.
	/// MUST be called from a thread, not interrupt
	void release();

	/// determine if the transfer is finished
	inline bool transferComplete() const
	{
		return tc;
	}

	/// get device-pointer, for
	inline DMA_Channel_TypeDef *device() const
	{
		return dev;
	}

	static DMAChannel dma1_channel1;
	static DMAChannel dma1_channel2;
	static DMAChannel dma1_channel3;
	static DMAChannel dma1_channel4;
	static DMAChannel dma1_channel5;

private:

	DMAChannel(DMA_Channel_TypeDef *dma, uint32_t tcf, uint8_t irqn);

	void onIRQ();

	uint32_t tc_flag;
	DMA_Channel_TypeDef *dev;
	IRQDelegate *owner;
#if SINGLETHREAD
	bool isclaimed;
#else
	Mutex mutex;
#endif
	bool tc;

	friend void ::DMA1_Channel1_IRQHandler(void);
	friend void ::DMA1_Channel2_3_IRQHandler(void);
	friend void ::DMA1_Channel4_5_IRQHandler(void);
};



#endif /* DMACHAN_H_ */
