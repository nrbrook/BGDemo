/*****************************************************************************
 * @file gecko_weak.c
 * @brief Gecko weak event functions
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

#include "gecko_weak.h"
#include "gecko_weak_handler.h"
#include "native_gecko.h"

void gecko_evt_handle(struct gecko_cmd_packet* evt) {
	switch (BGLIB_MSG_ID(evt->header)) {
		case gecko_evt_dfu_boot_id: {
			struct gecko_msg_dfu_boot_evt_t* data = &evt->data.evt_dfu_boot;
			gecko_evt_dfu_boot(data->version);
		} break;
		case gecko_evt_dfu_boot_failure_id: {
			struct gecko_msg_dfu_boot_failure_evt_t* data = &evt->data.evt_dfu_boot_failure;
			gecko_evt_dfu_boot_failure(data->reason);
		} break;
		case gecko_evt_system_boot_id: {
			struct gecko_msg_system_boot_evt_t* data = &evt->data.evt_system_boot;
			gecko_evt_system_boot(data->major, data->minor, data->patch, data->build, data->bootloader, data->hw);
		} break;
		case gecko_evt_system_external_signal_id: {
			struct gecko_msg_system_external_signal_evt_t* data = &evt->data.evt_system_external_signal;
			gecko_evt_system_external_signal(data->extsignals);
		} break;
		case gecko_evt_system_awake_id: {
			// no data struct with this name: struct gecko_msg_system_awake_evt_t* data = &evt->data.evt_system_awake;
			gecko_evt_system_awake();
		} break;
		case gecko_evt_system_hardware_error_id: {
			struct gecko_msg_system_hardware_error_evt_t* data = &evt->data.evt_system_hardware_error;
			gecko_evt_system_hardware_error(data->status);
		} break;
		case gecko_evt_le_gap_scan_response_id: {
			struct gecko_msg_le_gap_scan_response_evt_t* data = &evt->data.evt_le_gap_scan_response;
			gecko_evt_le_gap_scan_response(data->rssi, data->packet_type, data->address, data->address_type, data->bonding, data->data);
		} break;
		case gecko_evt_le_gap_adv_timeout_id: {
			// no data struct with this name: struct gecko_msg_le_gap_adv_timeout_evt_t* data = &evt->data.evt_le_gap_adv_timeout;
			gecko_evt_le_gap_adv_timeout();
		} break;
		case gecko_evt_le_gap_scan_request_id: {
			struct gecko_msg_le_gap_scan_request_evt_t* data = &evt->data.evt_le_gap_scan_request;
			gecko_evt_le_gap_scan_request(data->handle, data->address, data->address_type, data->bonding);
		} break;
		case gecko_evt_le_connection_opened_id: {
			struct gecko_msg_le_connection_opened_evt_t* data = &evt->data.evt_le_connection_opened;
			gecko_evt_le_connection_opened(data->address, data->address_type, data->master, data->connection, data->bonding);
		} break;
		case gecko_evt_le_connection_closed_id: {
			struct gecko_msg_le_connection_closed_evt_t* data = &evt->data.evt_le_connection_closed;
			gecko_evt_le_connection_closed(data->reason, data->connection);
		} break;
		case gecko_evt_le_connection_parameters_id: {
			struct gecko_msg_le_connection_parameters_evt_t* data = &evt->data.evt_le_connection_parameters;
			gecko_evt_le_connection_parameters(data->connection, data->interval, data->latency, data->timeout, data->security_mode, data->txsize);
		} break;
		case gecko_evt_le_connection_rssi_id: {
			struct gecko_msg_le_connection_rssi_evt_t* data = &evt->data.evt_le_connection_rssi;
			gecko_evt_le_connection_rssi(data->connection, data->status, data->rssi);
		} break;
		case gecko_evt_le_connection_phy_status_id: {
			struct gecko_msg_le_connection_phy_status_evt_t* data = &evt->data.evt_le_connection_phy_status;
			gecko_evt_le_connection_phy_status(data->connection, data->phy);
		} break;
		case gecko_evt_le_connection_bt5_opened_id: {
			struct gecko_msg_le_connection_bt5_opened_evt_t* data = &evt->data.evt_le_connection_bt5_opened;
			gecko_evt_le_connection_bt5_opened(data->address, data->address_type, data->master, data->connection, data->bonding, data->advertiser);
		} break;
		case gecko_evt_gatt_mtu_exchanged_id: {
			struct gecko_msg_gatt_mtu_exchanged_evt_t* data = &evt->data.evt_gatt_mtu_exchanged;
			gecko_evt_gatt_mtu_exchanged(data->connection, data->mtu);
		} break;
		case gecko_evt_gatt_service_id: {
			struct gecko_msg_gatt_service_evt_t* data = &evt->data.evt_gatt_service;
			gecko_evt_gatt_service(data->connection, data->service, data->uuid);
		} break;
		case gecko_evt_gatt_characteristic_id: {
			struct gecko_msg_gatt_characteristic_evt_t* data = &evt->data.evt_gatt_characteristic;
			gecko_evt_gatt_characteristic(data->connection, data->characteristic, data->properties, data->uuid);
		} break;
		case gecko_evt_gatt_descriptor_id: {
			struct gecko_msg_gatt_descriptor_evt_t* data = &evt->data.evt_gatt_descriptor;
			gecko_evt_gatt_descriptor(data->connection, data->descriptor, data->uuid);
		} break;
		case gecko_evt_gatt_characteristic_value_id: {
			struct gecko_msg_gatt_characteristic_value_evt_t* data = &evt->data.evt_gatt_characteristic_value;
			gecko_evt_gatt_characteristic_value(data->connection, data->characteristic, data->att_opcode, data->offset, data->value);
		} break;
		case gecko_evt_gatt_descriptor_value_id: {
			struct gecko_msg_gatt_descriptor_value_evt_t* data = &evt->data.evt_gatt_descriptor_value;
			gecko_evt_gatt_descriptor_value(data->connection, data->descriptor, data->offset, data->value);
		} break;
		case gecko_evt_gatt_procedure_completed_id: {
			struct gecko_msg_gatt_procedure_completed_evt_t* data = &evt->data.evt_gatt_procedure_completed;
			gecko_evt_gatt_procedure_completed(data->connection, data->result);
		} break;
		case gecko_evt_gatt_server_attribute_value_id: {
			struct gecko_msg_gatt_server_attribute_value_evt_t* data = &evt->data.evt_gatt_server_attribute_value;
			gecko_evt_gatt_server_attribute_value(data->connection, data->attribute, data->att_opcode, data->offset, data->value);
		} break;
		case gecko_evt_gatt_server_user_read_request_id: {
			struct gecko_msg_gatt_server_user_read_request_evt_t* data = &evt->data.evt_gatt_server_user_read_request;
			gecko_evt_gatt_server_user_read_request(data->connection, data->characteristic, data->att_opcode, data->offset);
		} break;
		case gecko_evt_gatt_server_user_write_request_id: {
			struct gecko_msg_gatt_server_user_write_request_evt_t* data = &evt->data.evt_gatt_server_user_write_request;
			gecko_evt_gatt_server_user_write_request(data->connection, data->characteristic, data->att_opcode, data->offset, data->value);
		} break;
		case gecko_evt_gatt_server_characteristic_status_id: {
			struct gecko_msg_gatt_server_characteristic_status_evt_t* data = &evt->data.evt_gatt_server_characteristic_status;
			gecko_evt_gatt_server_characteristic_status(data->connection, data->characteristic, data->status_flags, data->client_config_flags);
		} break;
		case gecko_evt_gatt_server_execute_write_completed_id: {
			struct gecko_msg_gatt_server_execute_write_completed_evt_t* data = &evt->data.evt_gatt_server_execute_write_completed;
			gecko_evt_gatt_server_execute_write_completed(data->connection, data->result);
		} break;
		case gecko_evt_endpoint_syntax_error_id: {
			struct gecko_msg_endpoint_syntax_error_evt_t* data = &evt->data.evt_endpoint_syntax_error;
			gecko_evt_endpoint_syntax_error(data->result, data->endpoint);
		} break;
		case gecko_evt_endpoint_data_id: {
			struct gecko_msg_endpoint_data_evt_t* data = &evt->data.evt_endpoint_data;
			gecko_evt_endpoint_data(data->endpoint, data->data);
		} break;
		case gecko_evt_endpoint_status_id: {
			struct gecko_msg_endpoint_status_evt_t* data = &evt->data.evt_endpoint_status;
			gecko_evt_endpoint_status(data->endpoint, data->type, data->destination_endpoint, data->flags);
		} break;
		case gecko_evt_hardware_soft_timer_id: {
			struct gecko_msg_hardware_soft_timer_evt_t* data = &evt->data.evt_hardware_soft_timer;
			gecko_evt_hardware_soft_timer(data->handle);
		} break;
		case gecko_evt_flash_ps_key_id: {
			struct gecko_msg_flash_ps_key_evt_t* data = &evt->data.evt_flash_ps_key;
			gecko_evt_flash_ps_key(data->key, data->value);
		} break;
		case gecko_evt_test_dtm_completed_id: {
			struct gecko_msg_test_dtm_completed_evt_t* data = &evt->data.evt_test_dtm_completed;
			gecko_evt_test_dtm_completed(data->result, data->number_of_packets);
		} break;
		case gecko_evt_test_hcidump_id: {
			struct gecko_msg_test_hcidump_evt_t* data = &evt->data.evt_test_hcidump;
			gecko_evt_test_hcidump(data->time, data->direction, data->packet_type, data->data);
		} break;
		case gecko_evt_sm_passkey_display_id: {
			struct gecko_msg_sm_passkey_display_evt_t* data = &evt->data.evt_sm_passkey_display;
			gecko_evt_sm_passkey_display(data->connection, data->passkey);
		} break;
		case gecko_evt_sm_passkey_request_id: {
			struct gecko_msg_sm_passkey_request_evt_t* data = &evt->data.evt_sm_passkey_request;
			gecko_evt_sm_passkey_request(data->connection);
		} break;
		case gecko_evt_sm_confirm_passkey_id: {
			struct gecko_msg_sm_confirm_passkey_evt_t* data = &evt->data.evt_sm_confirm_passkey;
			gecko_evt_sm_confirm_passkey(data->connection, data->passkey);
		} break;
		case gecko_evt_sm_bonded_id: {
			struct gecko_msg_sm_bonded_evt_t* data = &evt->data.evt_sm_bonded;
			gecko_evt_sm_bonded(data->connection, data->bonding);
		} break;
		case gecko_evt_sm_bonding_failed_id: {
			struct gecko_msg_sm_bonding_failed_evt_t* data = &evt->data.evt_sm_bonding_failed;
			gecko_evt_sm_bonding_failed(data->connection, data->reason);
		} break;
		case gecko_evt_sm_list_bonding_entry_id: {
			struct gecko_msg_sm_list_bonding_entry_evt_t* data = &evt->data.evt_sm_list_bonding_entry;
			gecko_evt_sm_list_bonding_entry(data->bonding, data->address, data->address_type);
		} break;
		case gecko_evt_sm_list_all_bondings_complete_id: {
			// no data struct with this name: struct gecko_msg_sm_list_all_bondings_complete_evt_t* data = &evt->data.evt_sm_list_all_bondings_complete;
			gecko_evt_sm_list_all_bondings_complete();
		} break;
		case gecko_evt_sm_confirm_bonding_id: {
			struct gecko_msg_sm_confirm_bonding_evt_t* data = &evt->data.evt_sm_confirm_bonding;
			gecko_evt_sm_confirm_bonding(data->connection, data->bonding_handle);
		} break;
		case gecko_evt_homekit_setupcode_display_id: {
			struct gecko_msg_homekit_setupcode_display_evt_t* data = &evt->data.evt_homekit_setupcode_display;
			gecko_evt_homekit_setupcode_display(data->connection, data->setupcode);
		} break;
		case gecko_evt_homekit_paired_id: {
			struct gecko_msg_homekit_paired_evt_t* data = &evt->data.evt_homekit_paired;
			gecko_evt_homekit_paired(data->connection, data->reason);
		} break;
		case gecko_evt_homekit_pair_verified_id: {
			struct gecko_msg_homekit_pair_verified_evt_t* data = &evt->data.evt_homekit_pair_verified;
			gecko_evt_homekit_pair_verified(data->connection, data->reason);
		} break;
		case gecko_evt_homekit_connection_opened_id: {
			struct gecko_msg_homekit_connection_opened_evt_t* data = &evt->data.evt_homekit_connection_opened;
			gecko_evt_homekit_connection_opened(data->connection);
		} break;
		case gecko_evt_homekit_connection_closed_id: {
			struct gecko_msg_homekit_connection_closed_evt_t* data = &evt->data.evt_homekit_connection_closed;
			gecko_evt_homekit_connection_closed(data->connection, data->reason);
		} break;
		case gecko_evt_homekit_identify_id: {
			struct gecko_msg_homekit_identify_evt_t* data = &evt->data.evt_homekit_identify;
			gecko_evt_homekit_identify(data->connection);
		} break;
		case gecko_evt_homekit_write_request_id: {
			struct gecko_msg_homekit_write_request_evt_t* data = &evt->data.evt_homekit_write_request;
			gecko_evt_homekit_write_request(data->connection, data->characteristic, data->chr_value_size, data->authorization_size, data->value_offset, data->value);
		} break;
		case gecko_evt_homekit_read_request_id: {
			struct gecko_msg_homekit_read_request_evt_t* data = &evt->data.evt_homekit_read_request;
			gecko_evt_homekit_read_request(data->connection, data->characteristic, data->offset);
		} break;
		case gecko_evt_homekit_error_id: {
			struct gecko_msg_homekit_error_evt_t* data = &evt->data.evt_homekit_error;
			gecko_evt_homekit_error(data->connection, data->reason);
		} break;
		case gecko_evt_homekit_pairing_removed_id: {
			struct gecko_msg_homekit_pairing_removed_evt_t* data = &evt->data.evt_homekit_pairing_removed;
			gecko_evt_homekit_pairing_removed(data->connection, data->remaining_pairings, data->pairing_id);
		} break;
		case gecko_evt_user_message_to_host_id: {
			struct gecko_msg_user_message_to_host_evt_t* data = &evt->data.evt_user_message_to_host;
			gecko_evt_user_message_to_host(data->data);
		} break;
	}
}

void gecko_evt_loop() {
	while (1) {
		/* Event pointer for handling events */
		struct gecko_cmd_packet* evt;

		/* Check for stack event. */
		evt = gecko_wait_event();

		gecko_evt_handle(evt);
	}
}

