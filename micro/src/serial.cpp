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
 * @file serial.cpp
 * Device driver for USART.
 */

#include "serial.h"

Serial *Serial::uart1=NULL;
#if __CORTEX_M >= 3
Serial *Serial::uart2=NULL, *Serial::uart3=NULL;
#endif

Serial::Serial(USART_TypeDef *u, bounded_buffer *rxd)
 : dev(u), rxbuf(rxd), rx_overflow(false)
{
#if __CORTEX_M >= 3
	if(dev == USART1){
		dmawr=&DMAChannel::dma1_channel4;
		uart1=this;
	}
	else if(dev == USART2){
		dmawr=&DMAChannel::dma1_channel7;
		uart2=this;
	}
	else if(dev == USART3){
		dmawr=&DMAChannel::dma1_channel2;
		uart3=this;
	}
#else
	if(dev == USART1){
#ifndef SINGLETHREAD
		dmawr=&DMAChannel::dma1_channel2;
#endif
		uart1=this;
	}
#endif
	else{
		assert(!"invalid USART selection");
	}
}

Serial::~Serial()
{
#ifndef SINGLETHREAD
	mutex.wait();
#endif
	USART_Cmd(dev, DISABLE);
#ifndef SINGLETHREAD
	mutex.signal();
#endif

	if(this == uart1){
		uart1=NULL;
	}
#if __CORTEX_M >= 3
	else if(this == uart2){
		uart2=NULL;
	}
	else if(this == uart3){
		uart3=NULL;
	}
#endif
}

void Serial::setup(uint32_t baud)
{
	USART_InitTypeDef uinit;
	uinit.USART_BaudRate=baud;
	uinit.USART_WordLength=USART_WordLength_8b;
	uinit.USART_StopBits=USART_StopBits_1;
	uinit.USART_Parity=USART_Parity_No;
	uinit.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
	uinit.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	setup(&uinit);
}

void Serial::setup(USART_InitTypeDef *mode)
{
#ifndef SINGLETHREAD
	mutex.wait();
#endif

	uint16_t in_pins, out_pins;
	GPIO_TypeDef *gpiodev=GPIOA;
	NVIC_InitTypeDef nv;

	if(dev == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#if __CORTEX_M >= 3
		in_pins=GPIO_Pin_10;
		out_pins=GPIO_Pin_9;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
#else
		in_pins=GPIO_Pin_10;
		out_pins=GPIO_Pin_9;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
#endif
		nv.NVIC_IRQChannel=USART1_IRQn;
	}
#if __CORTEX_M >= 3
	else if(dev == USART2){
		in_pins=GPIO_Pin_3;
		out_pins=GPIO_Pin_2;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		nv.NVIC_IRQChannel=USART2_IRQn;
	}
	else if(dev == USART3){
		gpiodev=GPIOB;
		in_pins=GPIO_Pin_11;
		out_pins=GPIO_Pin_10;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		nv.NVIC_IRQChannel=USART3_IRQn;
	}
#endif
	else{
		assert(!"invalid USART selection");
	}

#if __CORTEX_M >= 3
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin=in_pins;
	gpio.GPIO_Mode=GPIO_Mode_IPU;
	gpio.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(gpiodev, &gpio);
	gpio.GPIO_Pin=out_pins;
	gpio.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(gpiodev, &gpio);
#else
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin=in_pins;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Mode=GPIO_Mode_IN;
	gpio.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(gpiodev, &gpio);
	gpio.GPIO_Pin=out_pins;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Init(gpiodev, &gpio);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
#endif

	USART_DeInit(dev);
	USART_Init(dev, mode);

#if __CORTEX_M >= 3
	nv.NVIC_IRQChannelPreemptionPriority=UARTIRQ_PRIORITY;
	nv.NVIC_IRQChannelSubPriority=0;
#else
	nv.NVIC_IRQChannelPriority=UARTIRQ_PRIORITY;
#endif
	nv.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&nv);

	USART_ITConfig(dev, USART_IT_RXNE, ENABLE);
	USART_Cmd(dev, ENABLE);

#ifndef SINGLETHREAD
	mutex.signal();
#endif
}

void Serial::transmit(uint32_t count, const uint8_t *data)
{
	if(count < 1){
		return;
	}

#ifndef SINGLETHREAD
	mutex.wait();

	dmawr->claim(&semdel);

	// initialise a DMA channel for writing
	DMA_InitTypeDef dmainit={
#if __CORTEX_M >= 3
			(uint32_t) &(dev->DR),
#else
			(uint32_t) &(dev->TDR),
#endif
			(uint32_t) data,
			DMA_DIR_PeripheralDST,
			count,
			DMA_PeripheralInc_Disable,
			DMA_MemoryInc_Enable,
			DMA_PeripheralDataSize_Byte,
			DMA_MemoryDataSize_Byte,
			DMA_Mode_Normal,
			Serial::DMA_PRIORITY,
			DMA_M2M_Disable
		};
	DMA_DeInit(dmawr->device());
	DMA_Init(dmawr->device(), &dmainit);

	// turn the DMA on
	dmawr->enable();
	USART_DMACmd(dev, USART_DMAReq_Tx, ENABLE);

	// wait for DMA-complete
	semdel.sem.dec();

	// disable DMA linkage and channel.
	USART_DMACmd(dev, USART_DMAReq_Tx, DISABLE);
	dmawr->release();

	// spin until transfer-complete
	while(USART_GetFlagStatus(dev, USART_FLAG_TC) == RESET)
		;

	// all done.
	mutex.signal();
#else

	// send bytes one at a time, polling between them.
	for(uint32_t i=0;i<count;++i){
		while(USART_GetFlagStatus(dev, USART_FLAG_TXE) == RESET)
			;

		USART_SendData(dev, data[i]);
	}

	// spin until transfer-complete
	while(USART_GetFlagStatus(dev, USART_FLAG_TC) == RESET)
		;

#endif
}

void Serial::transmit(const buffer &b)
{
	transmit(b.length(), (const uint8_t *) b);
}

void Serial::print(const char *p)
{
	transmit(strlen(p), (const uint8_t *) p);
}

void Serial::println(const char *p)
{
	print(p);
	transmit(2, (const uint8_t *) "\r\n");
}

void Serial::onInterrupt()
{
	// a byte is waiting, put in the the buffer
	if(USART_GetFlagStatus(dev, USART_FLAG_RXNE) != RESET){
		uint8_t b=uint8_t(USART_ReceiveData(dev) & 0xFF);
		rx_overflow |= !rxbuf->insertFromInterrupt(b);
	}
}

void USART1_IRQHandler(void)
{
	if(Serial::uart1 != NULL){
		Serial::uart1->onInterrupt();
	}
}

#if __CORTEX_M >= 3
void USART2_IRQHandler(void)
{
	if(Serial::uart2 != NULL){
		Serial::uart2->onInterrupt();
	}
}

void USART3_IRQHandler(void)
{
	if(Serial::uart3 != NULL){
		Serial::uart3->onInterrupt();
	}
}
#endif
