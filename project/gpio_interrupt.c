/*****************************************************************************
 * @file gpio_interrupt.c
 * @brief GPIO Interrupt handler functions
 * @author Nick Brook
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2017 Nick Brook, https://www.nickbrook.me</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Nick Brook has no
 * obligation to support this Software. Nick Brook is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Nick Brook will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include <gpio_interrupt.h>
#include "em_gpio.h"
#include "native_gecko.h"
#include "stdio.h"

#define NUMBER_OF_INTERRUPTS 16
#define NUMBER_OF_PINS_PER_GROUP 4

static const uint32 GPIO_signal_key = 0x80000000;

static bool interruptInUse[NUMBER_OF_INTERRUPTS];
static GPIO_Port_TypeDef interruptPorts[NUMBER_OF_INTERRUPTS];
static unsigned int interruptPins[NUMBER_OF_INTERRUPTS];


static bool pinForInterrupt(int interrupt, GPIO_Port_TypeDef *port, int *pin) {
	if(interrupt < 0 || interrupt > NUMBER_OF_INTERRUPTS || !interruptInUse[interrupt]) {
		return false;
	}
	*port = interruptPorts[interrupt];
	*pin = interruptPins[interrupt];
	return true;
}

static int interruptForPin(GPIO_Port_TypeDef port, unsigned int pin) {
#ifndef _GPIO_EXTIPINSELL_MASK
	return pin;
#endif
	int groupStart = (pin / NUMBER_OF_PINS_PER_GROUP) * NUMBER_OF_PINS_PER_GROUP;
	for(int i = groupStart; i < groupStart + NUMBER_OF_PINS_PER_GROUP; i++) {
		if(interruptInUse[i] && interruptPorts[i] == port && interruptPins[i] == pin) {
			return i;
		}
	}
	return -1;
}

static int setInterruptForPin(GPIO_Port_TypeDef port, unsigned int pin) {
#ifndef _GPIO_EXTIPINSELL_MASK
	interruptInUse[pin] = true;
	interruptPorts[pin] = port;
	interruptPins[pin] = pin;
	return pin;
#endif
	int interrupt = interruptForPin(port, pin);
	if(interrupt != -1) {
		return interrupt;
	}
	int groupStart = (pin / NUMBER_OF_PINS_PER_GROUP) * NUMBER_OF_PINS_PER_GROUP;
	for(int i = groupStart; i < groupStart + NUMBER_OF_PINS_PER_GROUP; i++) {
		if(!interruptInUse[i]) {
			interruptInUse[i] = true;
			interruptPorts[i] = port;
			interruptPins[i] = pin;
			return i;
		}
	}
	return -1;
}

bool gpio_IntConfig(GPIO_Port_TypeDef port, unsigned int pin, bool risingEdge, bool fallingEdge, bool enable) {
	int interrupt = setInterruptForPin(port, pin);
	if (interrupt == -1) {
		return false;
	}
	/* Disable interrupt before changing parameters. Mitigates bug in interrupt select register which triggers interrupt when changed. */
	BUS_RegBitWrite(&(GPIO->IEN), interrupt, false);
	GPIO_ExtIntConfig(port, pin, interrupt, risingEdge, fallingEdge, enable);
	if (enable) {
		/* Enable interrupt after changing parameters */
		GPIO_IntClear(1 << interrupt);
		BUS_RegBitWrite(&(GPIO->IEN), interrupt, true);
	}
	return true;
}

void gpio_IntClear(GPIO_Port_TypeDef port, unsigned int pin) {
	int interrupt = interruptForPin(port, pin);
	if(interrupt != -1) {
		GPIO_IntClear(1 << interrupt);
	}
}

bool gpio_IntSetEnabled(GPIO_Port_TypeDef port, unsigned int pin, bool enabled) {
	int interrupt = interruptForPin(port, pin);
	if (interrupt == -1) {
		return false;
	}
	if(enabled) {
		GPIO_IntClear(1 << interrupt);
		GPIO_IntEnable(1 << interrupt);
	} else {
		GPIO_IntDisable(1 << interrupt);
	}
	return true;
}

void gpio_IntRemoveUsage(GPIO_Port_TypeDef port, unsigned int pin) {
	int interrupt = interruptForPin(port, pin);
	if (interrupt == -1) {
		return;
	}
	interruptInUse[interrupt] = false;
}

/**
* Handle GPIO interrupts and trigger system_external_signal event
*/
void GPIO_ODD_IRQHandler()
{
  uint32_t flags = GPIO_IntGetEnabled() & 0x0000AAAA;
  GPIO_IntClear(flags);
  //Send gecko_evt_system_external_signal_id event to the main loop
  gecko_external_signal(flags | GPIO_signal_key);
}

/**
* Handle GPIO interrupts and trigger system_external_signal event
*/
void GPIO_EVEN_IRQHandler()
{
  uint32_t flags = GPIO_IntGetEnabled() & 0x00005555;
  GPIO_IntClear(flags);
  //Send gecko_evt_system_external_signal_id event to the main loop
  gecko_external_signal(flags | GPIO_signal_key);
}

// Get the port and pin from the interrupt registers
void gpio_get_port_pin_from_interrupt(int interruptNumber, GPIO_Port_TypeDef *port, unsigned int *pin) {
	if(interruptNumber < 8) { // low register
		*port = (GPIO->EXTIPSELL >> (_GPIO_EXTIPSELL_EXTIPSEL1_SHIFT * interruptNumber)) & _GPIO_EXTIPSELL_EXTIPSEL0_MASK;
	} else { // high register
		int tmp = interruptNumber - 8;
		*port = (GPIO->EXTIPSELH >> (_GPIO_EXTIPSELH_EXTIPSEL9_SHIFT * tmp)) & _GPIO_EXTIPSELH_EXTIPSEL8_MASK;
	}
	// check for pin to interrupt mapping
#if defined(_GPIO_EXTIPINSELL_MASK)
	if(interruptNumber < 8) {
		*pin = (GPIO->EXTIPINSELL >> (_GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT * interruptNumber)) & _GPIO_EXTIPINSELL_EXTIPINSEL0_MASK;
	} else {
		int tmp = interruptNumber - 8;
		*pin = (GPIO->EXTIPINSELH >> (_GPIO_EXTIPINSELH_EXTIPINSEL9_SHIFT * tmp)) & _GPIO_EXTIPINSELH_EXTIPINSEL8_MASK;
	}
	*pin += (interruptNumber / 4) * 4;
#else
	*pin = interruptNumber;
#endif
}

void gecko_evt_system_external_signal(uint32 extsignals) {
	if((extsignals & GPIO_signal_key) == 0) {
		gecko_evt_system_external_signal_non_gpio(extsignals);
		return;
	}
	for (int i = 0; i < 16; i++) {
		if (extsignals & 1) { // interrupt on the interrupt i
			// check interrupt selection for port
			GPIO_Port_TypeDef port;
			int pin;
//			gpio_get_port_pin_from_interrupt(i, &port, &pin);
			if(pinForInterrupt(i, &port, &pin)) {
				unsigned int state = GPIO_PinInGet(port, pin);
				gpio_IntEvent(port, pin, state != 0);
			}
		}
		extsignals = extsignals >> 1;
	}
}
