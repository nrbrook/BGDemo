/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "boards.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "aat.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#ifdef FEATURE_BOARD_DETECTED
#include "bspconfig.h"
#include "pti.h"
#endif

/* Device initialization header */
#include "InitDevice.h"

#ifdef FEATURE_SPI_FLASH
#include "em_usart.h"
#include "mx25flash_spi.h"
#endif /* FEATURE_SPI_FLASH */

#include "retargetswo.h"
#include "logging.h"
#include "gecko_weak_handler.h"
#include "gpio_interrupt.h"
#include "app_timer.h"
#include "em_adc.h"
#include "joystick.h"
#include "graphics.h"

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

#ifdef FEATURE_PTI_SUPPORT
static const RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;
#endif

/* Gecko configuration parameters (see gecko_configuration.h) */
static const gecko_configuration_t config = { .config_flags = 0, .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE, .bluetooth.max_connections = MAX_CONNECTIONS, .bluetooth.heap = bluetooth_stack_heap,
		.bluetooth.heap_size = sizeof(bluetooth_stack_heap), .bluetooth.sleep_clock_accuracy = 100, // ppm
		.gattdb = &bg_gattdb_data, .ota.flags = 0, .ota.device_name_len = 3, .ota.device_name_ptr = "OTA",
#ifdef FEATURE_PTI_SUPPORT
		.pti = &ptiInit,
#endif
		};

/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu = 0;

#ifndef FEATURE_IOEXPANDER
/* Periodically called Display Polarity Inverter Function for the LCD.
Toggles the the EXTCOMIN signal of the Sharp memory LCD panel, which prevents building up a DC
bias according to the LCD's datasheet */
static void (*dispPolarityInvert)(void *);
#endif /* FEATURE_IOEXPANDER */

/**
 * @brief  Main function
 */
int main(void) {
#ifdef FEATURE_SPI_FLASH
	/* Put the SPI flash into Deep Power Down mode for those radio boards where it is available */
	MX25_init();
	MX25_DP();
	/* We must disable SPI communication */
	USART_Reset(USART1);

#endif /* FEATURE_SPI_FLASH */

	/* Initialize peripherals */
	enter_DefaultMode_from_RESET();

	RETARGET_SwoInit();

	/* Enable GPIO_EVEN interrupt vector in NVIC */
	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	/* Configure interrupts on falling edge */
	gpio_IntConfig(BUTTON_LED0_PORT, BUTTON_LED0_PIN, false, true, true);
	gpio_IntConfig(BUTTON_LED1_PORT, BUTTON_LED1_PIN, false, true, true);
	gpio_IntConfig(BUTTON_LED2_PORT, BUTTON_LED2_PIN, false, true, true);
	gpio_IntConfig(BUTTON_LED3_PORT, BUTTON_LED3_PIN, false, true, true);

	/* Initialize stack */
	gecko_init(&config);

	/* Enter main event loop */
	gecko_evt_loop();
}

void toggleState(int num) {
	static const GPIO_Port_TypeDef ports[] = {BUTTON_LED0_PORT, BUTTON_LED1_PORT, BUTTON_LED2_PORT, BUTTON_LED3_PORT};
	static const int pins[] = {BUTTON_LED0_PIN, BUTTON_LED1_PIN, BUTTON_LED2_PIN, BUTTON_LED3_PIN};
	static bool states[] = {false, false, false, false};
	if(num > 3) {
		return;
	}
	if (states[num]) { // set to input, enable interrupt
		LogInfo("Toggling %d to input", num);
		// set to input with filter enabled
		GPIO_PinModeSet(ports[num], pins[num], gpioModeInput, 1);
		// enable interrupt
		gpio_IntSetEnabled(ports[num], pins[num], true);
	} else { // disable interrupt, set LED on
		LogInfo("Toggling %d to output", num);
		// disable interrupt
		gpio_IntSetEnabled(ports[num], pins[num], false);
		// set pin to output, 0 to enable LED
		GPIO_PinModeSet(ports[num], pins[num], gpioModePushPull, 0);
	}
	states[num] = !states[num];
#ifdef GRAPHICS_H // ifdef so you can copy paste this code before following the display steps
	graphSetButtonState(num, states[num]);
#endif
}

void gpio_IntEvent(GPIO_Port_TypeDef port, unsigned int pin, bool state) {
	if (port == BUTTON_LED0_PORT && pin == BUTTON_LED0_PIN) {
		toggleState(1);
	} else if (port == BUTTON_LED1_PORT && pin == BUTTON_LED1_PIN) {
		toggleState(0);
	} else if (port == BUTTON_LED2_PORT && pin == BUTTON_LED2_PIN) {
		toggleState(3);
	} else if (port == BUTTON_LED3_PORT && pin == BUTTON_LED3_PIN) {
		toggleState(2);
	}
}

/** GECKO EVENTS **/

/* This boot event is generated when the system boots up after reset.
 * Here the system is set to start advertising immediately after boot procedure. */
void gecko_evt_system_boot(uint16 major, uint16 minor, uint16 patch, uint16 build, uint32 bootloader, uint16 hw) {
#ifdef GRAPHICS_H // ifdef so you can copy paste this code before following the display steps
	/* Initialise the graphics */
	graphInit();
	graphSetJoystickDirection(JoystickDirectionNone);

	graphSetButtonState(0, false);
	graphSetButtonState(1, false);
	graphSetButtonState(2, false);
	graphSetButtonState(3, false);

	/* Disable Joystick pin – enabled due to a bug in the display library */
	GPIO_PinModeSet(gpioPortA, 0, gpioModeDisabled, 0);
#endif

	/* Set advertising parameters. 100ms advertisement interval. All channels used.
	 * The first two parameters are minimum and maximum advertising interval, both in
	 * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
	gecko_cmd_le_gap_set_adv_parameters(160, 160, 7);

	/* Start general advertising and enable connections. */
	gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);

	/* Start reading ADC 4 times a second */
	gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(250), AppTimerIDADCRead, false);

	/* Update now */
	gecko_evt_hardware_soft_timer(AppTimerIDADCRead);
}

void handleJoystickRead(JoystickDirection direction) {
	static JoystickDirection previousDirection = JoystickDirectionNone;
	if(direction == previousDirection) { return; }
	previousDirection = direction;
	LogInfo("New joystick direction: %d", direction);
#ifdef GRAPHICS_H // ifdef so you can copy paste this code before following the display steps
	graphSetJoystickDirection(direction);
#endif
}

void gecko_evt_hardware_soft_timer(uint8 handle) {
	switch ((AppTimerID) handle) {
	case AppTimerIDStop: break;
	case AppTimerIDADCRead:
		/* Start reading */
		ADC_Start(ADC0, adcStartSingle);
		/* Wait until it's read */
		while ((ADC0->IF & ADC_IF_SINGLE) == 0)
			;
		/* Get ADC single result */
		uint32_t sample = ADC_DataSingleGet(ADC0);

		/* Get the joystick direction from ADC reading */
		JoystickDirection d = JoystickDirectionNone;
		if(sample < 50) {
			d = JoystickDirectionCenter;
		} else if(sample < 2040) {
			d = JoystickDirectionS;
		} else if(sample < 2450) {
			d = JoystickDirectionW;
		} else if(sample < 3130) {
			d = JoystickDirectionE;
		} else if(sample < 3505) {
			d = JoystickDirectionN;
		}
		handleJoystickRead(d);

		break;
#ifndef FEATURE_IOEXPANDER
	case AppTimerIDDispPolInv:
	  /*Toggle the the EXTCOMIN signal, which prevents building up a DC bias  within the
	   * Sharp memory LCD panel */
	  dispPolarityInvert(0);
	  break;
#endif /* FEATURE_IOEXPANDER */
	}
}

void gecko_evt_le_connection_closed(uint16 reason, uint8 connection) {
	/* Check if need to boot to dfu mode */
	if (boot_to_dfu) {
		/* Enter to DFU OTA mode */
		gecko_cmd_system_reset(2);
	} else {
		/* Restart advertising after client has disconnected */
		gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
	}
}

void gecko_evt_gatt_server_user_write_request(uint8 connection, uint16 characteristic, uint8 att_opcode, uint16 offset, uint8array value) {
	/* Events related to OTA upgrading
	 ----------------------------------------------------------------------------- */

	/* Check if the user-type OTA Control Characteristic was written.
	 * If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
	if (characteristic == gattdb_ota_control) {
		/* Set flag to enter to OTA mode */
		boot_to_dfu = 1;
		/* Send response to Write Request */
		gecko_cmd_gatt_server_send_user_write_response(connection,
		gattdb_ota_control, bg_err_success);

		/* Close connection to enter to DFU OTA mode */
		gecko_cmd_endpoint_close(connection);
	}
}

/**************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 *****************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  #ifndef FEATURE_IOEXPANDER

  dispPolarityInvert =  pFunction;
  /* Start timer with required frequency */
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(1000 / frequency), AppTimerIDDispPolInv, false);

  #endif /* FEATURE_IOEXPANDER */

  return 0;
}

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
