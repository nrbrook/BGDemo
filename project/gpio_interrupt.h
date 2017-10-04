/*****************************************************************************
 * @file gpio_interrupt.h
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

#ifndef GPIO_INTERRUPT_H_
#define GPIO_INTERRUPT_H_

#include "em_gpio.h"
#include "bg_types.h"

/* Using the configuration functions below instead of the SDK functions
 * 1. Mitigates a bug in the interrupt select registers which triggers an
 *    interrupt when the configuration is changed with GPIO_IntConfig
 * 2. Avoids a limitation of the SDK GPIO_IntConfig function where pins of the
 *    same number on different ports cannot be used at the same time (only on devices that have the GPIO_EXTIPINSEL registers). The interrupt remapping is done internally.
 * 3. Clears pending interrupts before enabling interrupts
 *
 * Pins are grouped into 4s (pins 0-3, 4-7, 8-11, 12-15). There can only be
 * 4 pins in each pin group configured for interrupts, e.g. PA0, PA1, PB1, PC2
 * would be a full group. Configuring an interrupt with gpio_IntConfig
 * 'consumes' one interrupt from its group until you call gpio_IntRemoveUsage,
 * at which point the interrupt can be used for other pins.
 */

/***************************************************************************//**
 * @brief
 *   Configure GPIO interrupt.
 *
 * @details
 *    There is a hardware limitation of 16 interrupts configured simultaneously.
 *    Configuring an interrupt with this function 'consumes' one interrupt
 *    for that port/pin until you call gpio_IntRemoveUsage(), at which point the
 *    interrupt can be used for other pins.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The pin number on the port.
 *
 * @param[in] risingEdge
 *   Set to true if interrupts shall be enabled on rising edge, otherwise false.
 *
 * @param[in] fallingEdge
 *   Set to true if interrupts shall be enabled on falling edge, otherwise false.
 *
 * @param[in] enable
 *   Set to true if interrupt shall be enabled after configuration completed,
 *   false to leave disabled. See gpio_IntSetEnabled().
 *
 * @return
 *	True if an interrupt was available to be configured, false if all interrupts are consumed. If false, free an interrupt using gpio_IntRemoveUsage()
 ******************************************************************************/
bool gpio_IntConfig(GPIO_Port_TypeDef port, unsigned int pin, bool risingEdge, bool fallingEdge, bool enable);

/***************************************************************************//**
 * @brief
 *   Clear a pending GPIO interrupt.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The pin number on the port.
 ******************************************************************************/
void gpio_IntClear(GPIO_Port_TypeDef port, unsigned int pin);

/***************************************************************************//**
 * @brief
 *   Enable or disable a GPIO interrupt.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The pin number on the port.
 *
 * @param[in] enabled
 *   Enable or disable the interrupt for this pin
 *
 * @return
 *   True if an interrupt has been previously configured for this port/pin using gpio_IntConfig(), false if not.
 ******************************************************************************/
bool gpio_IntSetEnabled(GPIO_Port_TypeDef port, unsigned int pin, bool enabled);

/***************************************************************************//**
 * @brief
 *   Mark a pin as unused, freeing an interrupt for use on other pins.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The pin number on the port.
 ******************************************************************************/
void gpio_IntRemoveUsage(GPIO_Port_TypeDef port, unsigned int pin);

/***************************************************************************//**
 * @brief
 *   Handles the external signals gecko event in order to emit gpio_IntEvent()
 *
 * @note
 *   If using gecko_weak library, you do not need to call this manually. Instead, implement the next event handler function below
 *
 * @param[in] extsignals
 *   The extsignals data
 ******************************************************************************/
void gecko_evt_system_external_signal(uint32 extsignals);

/***************************************************************************//**
 * @brief
 *   A weak function that you should implement to handle non-GPIO external signals.
 *
 * @note
 *   Only relevant if using the gecko_weak library. Use this to handle other ext signals instead of gecko_evt_system_external_signal()/
 *
 * @param[in] extsignals
 *   The extsignals data
 ******************************************************************************/
void gecko_evt_system_external_signal_non_gpio(uint32 extsignals) __attribute__((weak));

/***************************************************************************//**
 * @brief
 *   A weak function that you should implement to handle GPIO interrupts.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The pin number on the port.
 *
 * @param[in] state
 *   Defines if the pin is high or low.
 ******************************************************************************/
void gpio_IntEvent(GPIO_Port_TypeDef port, unsigned int pin, bool state) __attribute__((weak));

#endif /* GPIO_INTERRUPT_H_ */
