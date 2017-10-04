/*****************************************************************************
 * @file gecko_weak.h
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

#ifndef GECKO_WEAK_H_
#define GECKO_WEAK_H_

#include "native_gecko.h"

__attribute__ ((weak)) void gecko_evt_dfu_boot(uint32 version);
__attribute__ ((weak)) void gecko_evt_dfu_boot_failure(uint16 reason);
__attribute__ ((weak)) void gecko_evt_system_boot(uint16 major, uint16 minor, uint16 patch, uint16 build, uint32 bootloader, uint16 hw);
__attribute__ ((weak)) void gecko_evt_system_external_signal(uint32 extsignals);
__attribute__ ((weak)) void gecko_evt_system_awake(void);
__attribute__ ((weak)) void gecko_evt_system_hardware_error(uint16 status);
__attribute__ ((weak)) void gecko_evt_le_gap_scan_response(int8 rssi, uint8 packet_type, bd_addr address, uint8 address_type, uint8 bonding, uint8array data);
__attribute__ ((weak)) void gecko_evt_le_gap_adv_timeout(void);
__attribute__ ((weak)) void gecko_evt_le_gap_scan_request(uint8 handle, bd_addr address, uint8 address_type, uint8 bonding);
__attribute__ ((weak)) void gecko_evt_le_connection_opened(bd_addr address, uint8 address_type, uint8 master, uint8 connection, uint8 bonding);
__attribute__ ((weak)) void gecko_evt_le_connection_closed(uint16 reason, uint8 connection);
__attribute__ ((weak)) void gecko_evt_le_connection_parameters(uint8 connection, uint16 interval, uint16 latency, uint16 timeout, uint8 security_mode, uint16 txsize);
__attribute__ ((weak)) void gecko_evt_le_connection_rssi(uint8 connection, uint8 status, int8 rssi);
__attribute__ ((weak)) void gecko_evt_le_connection_phy_status(uint8 connection, uint8 phy);
__attribute__ ((weak)) void gecko_evt_le_connection_bt5_opened(bd_addr address, uint8 address_type, uint8 master, uint8 connection, uint8 bonding, uint8 advertiser);
__attribute__ ((weak)) void gecko_evt_gatt_mtu_exchanged(uint8 connection, uint16 mtu);
__attribute__ ((weak)) void gecko_evt_gatt_service(uint8 connection, uint32 service, uint8array uuid);
__attribute__ ((weak)) void gecko_evt_gatt_characteristic(uint8 connection, uint16 characteristic, uint8 properties, uint8array uuid);
__attribute__ ((weak)) void gecko_evt_gatt_descriptor(uint8 connection, uint16 descriptor, uint8array uuid);
__attribute__ ((weak)) void gecko_evt_gatt_characteristic_value(uint8 connection, uint16 characteristic, uint8 att_opcode, uint16 offset, uint8array value);
__attribute__ ((weak)) void gecko_evt_gatt_descriptor_value(uint8 connection, uint16 descriptor, uint16 offset, uint8array value);
__attribute__ ((weak)) void gecko_evt_gatt_procedure_completed(uint8 connection, uint16 result);
__attribute__ ((weak)) void gecko_evt_gatt_server_attribute_value(uint8 connection, uint16 attribute, uint8 att_opcode, uint16 offset, uint8array value);
__attribute__ ((weak)) void gecko_evt_gatt_server_user_read_request(uint8 connection, uint16 characteristic, uint8 att_opcode, uint16 offset);
__attribute__ ((weak)) void gecko_evt_gatt_server_user_write_request(uint8 connection, uint16 characteristic, uint8 att_opcode, uint16 offset, uint8array value);
__attribute__ ((weak)) void gecko_evt_gatt_server_characteristic_status(uint8 connection, uint16 characteristic, uint8 status_flags, uint16 client_config_flags);
__attribute__ ((weak)) void gecko_evt_gatt_server_execute_write_completed(uint8 connection, uint16 result);
__attribute__ ((weak)) void gecko_evt_endpoint_syntax_error(uint16 result, uint8 endpoint);
__attribute__ ((weak)) void gecko_evt_endpoint_data(uint8 endpoint, uint8array data);
__attribute__ ((weak)) void gecko_evt_endpoint_status(uint8 endpoint, uint32 type, int8 destination_endpoint, uint8 flags);
__attribute__ ((weak)) void gecko_evt_hardware_soft_timer(uint8 handle);
__attribute__ ((weak)) void gecko_evt_flash_ps_key(uint16 key, uint8array value);
__attribute__ ((weak)) void gecko_evt_test_dtm_completed(uint16 result, uint16 number_of_packets);
__attribute__ ((weak)) void gecko_evt_test_hcidump(uint32 time, uint8 direction, uint8 packet_type, uint16array data);
__attribute__ ((weak)) void gecko_evt_sm_passkey_display(uint8 connection, uint32 passkey);
__attribute__ ((weak)) void gecko_evt_sm_passkey_request(uint8 connection);
__attribute__ ((weak)) void gecko_evt_sm_confirm_passkey(uint8 connection, uint32 passkey);
__attribute__ ((weak)) void gecko_evt_sm_bonded(uint8 connection, uint8 bonding);
__attribute__ ((weak)) void gecko_evt_sm_bonding_failed(uint8 connection, uint16 reason);
__attribute__ ((weak)) void gecko_evt_sm_list_bonding_entry(uint8 bonding, bd_addr address, uint8 address_type);
__attribute__ ((weak)) void gecko_evt_sm_list_all_bondings_complete(void);
__attribute__ ((weak)) void gecko_evt_sm_confirm_bonding(uint8 connection, int8 bonding_handle);
__attribute__ ((weak)) void gecko_evt_homekit_setupcode_display(uint8 connection, uint8array setupcode);
__attribute__ ((weak)) void gecko_evt_homekit_paired(uint8 connection, uint16 reason);
__attribute__ ((weak)) void gecko_evt_homekit_pair_verified(uint8 connection, uint16 reason);
__attribute__ ((weak)) void gecko_evt_homekit_connection_opened(uint8 connection);
__attribute__ ((weak)) void gecko_evt_homekit_connection_closed(uint8 connection, uint16 reason);
__attribute__ ((weak)) void gecko_evt_homekit_identify(uint8 connection);
__attribute__ ((weak)) void gecko_evt_homekit_write_request(uint8 connection, uint16 characteristic, uint16 chr_value_size, uint16 authorization_size, uint16 value_offset, uint8array value);
__attribute__ ((weak)) void gecko_evt_homekit_read_request(uint8 connection, uint16 characteristic, uint16 offset);
__attribute__ ((weak)) void gecko_evt_homekit_error(uint8 connection, uint16 reason);
__attribute__ ((weak)) void gecko_evt_homekit_pairing_removed(uint8 connection, uint16 remaining_pairings, uint8array pairing_id);
__attribute__ ((weak)) void gecko_evt_user_message_to_host(uint8array data);

#endif /* GECKO_WEAK_H_ */
