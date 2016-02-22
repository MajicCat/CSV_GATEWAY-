#include "wiced_resource.h"


const char resources_apps_DIR_bt_smartbridge_DIR_data_html_data[33] = "<!-- Details page suffixes -->\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html = { RESOURCE_IN_MEMORY, 32, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_data[85] = "<table border=\"0\" cellpadding=\"5\"><tr><td><b>Connected to : <br><br></b></td><td><b>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start = { RESOURCE_IN_MEMORY, 84, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_end_data[25] = "<br><br></b></td></tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_end = { RESOURCE_IN_MEMORY, 24, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_time_start_data[35] = "<tr><td><b>Current Time :</td><td>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_time_start = { RESOURCE_IN_MEMORY, 34, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_time_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_time_end_data[21] = " UTC</b></td></tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_time_end = { RESOURCE_IN_MEMORY, 20, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_time_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_date_start_data[35] = "<tr><td><b>Current Date :</td><td>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_date_start = { RESOURCE_IN_MEMORY, 34, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_date_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_date_end_data[21] = " UTC</b></td></tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_date_end = { RESOURCE_IN_MEMORY, 20, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_date_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_start_data[43] = "<th><td><b>Device Address :</td><td>\r\n" \
"</b>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_start = { RESOURCE_IN_MEMORY, 42, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_end_data[13] = "</td></th>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_end = { RESOURCE_IN_MEMORY, 12, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_device_address_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_start_data[34] = "<th><td><b>Device Name :</td><td>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_start = { RESOURCE_IN_MEMORY, 33, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_end_data[17] = "</b></td></th>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_end = { RESOURCE_IN_MEMORY, 16, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_device_name_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start_data[61] = "<tr><td><b><br><br>Primary Service :</b></td><td><br><br><b>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start = { RESOURCE_IN_MEMORY, 60, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end_data[17] = "</b></td></tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end = { RESOURCE_IN_MEMORY, 16, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_char_start_data[42] = "<tr><td>Characteristic [Value] :</td><td>";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_char_start = { RESOURCE_IN_MEMORY, 41, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_char_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_char_end_data[13] = "</td></tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_char_end = { RESOURCE_IN_MEMORY, 12, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_char_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_end_data[345] = "<td><form action=\"\" style=\"display:table-cell; vertical-align:top;\">\r\n" \
"<p>\r\n" \
"<input type=\"button\" onclick=\"do_ajax( \'/return\', null, null, 0, null, null, null, null );clearTimeout(timeoutID);reloadData();\" value=\"Return\" class=\"button white bigrounded\" />&nbsp;&nbsp;\r\n" \
"</p>\r\n" \
"</form></td>\r\n" \
"</tr></table>\r\n" \
"\r\n" \
"<!-- Scan/Connection page suffixes -->\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_end = { RESOURCE_IN_MEMORY, 344, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connecting_data[43] = "<td><center>Connecting ...</center></td>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connecting = { RESOURCE_IN_MEMORY, 42, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connecting_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_found_data[60] = "<table><tr><td>No device found</td><td></td></tr></table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_found = { RESOURCE_IN_MEMORY, 59, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_found_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_connected_data[64] = "<table><tr><td>No device connected</td><td></td></tr></table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_connected = { RESOURCE_IN_MEMORY, 63, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_no_device_connected_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_scanning_message_data[85] = "<table><tr><td>Scanning for Bluetooth Smart devices ...</td><td></td></tr></table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_scanning_message = { RESOURCE_IN_MEMORY, 84, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_scanning_message_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_start_data[302] = "<table border=\"0\" cellpadding=\"5\">\r\n" \
"    <tr>\r\n" \
"    <form action=\"\">\r\n" \
"    <td><p style=\"font-size:18pt;line-height:25pt;\"><b>Enter Passkey</b></td>\r\n" \
"    <td><input type=\"text\" id=\"passkey\" size=\"6\" maxlength=\"6\" onload=\"display_passkey()\" onfocus=\"clearTimeout(timeoutID)\" onblur=\"set_passkey()\" value=\"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_start = { RESOURCE_IN_MEMORY, 301, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_end_data[44] = "\"></td>\r\n" \
"    </form>\r\n" \
"    </tr>\r\n" \
"</table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_end = { RESOURCE_IN_MEMORY, 43, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_passkey_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_scan_title_data[493] = "<table border=\"0\" cellpadding=\"5\">\r\n" \
"    <tr>\r\n" \
"        <td><p style=\"font-size:18pt;line-height:25pt;\"><b>Scan Results</b></td>\r\n" \
"        <td>\r\n" \
"            <p>\r\n" \
"                <input type=\"button\" onclick=\"do_ajax( \'/rescan\', null, null, 0, null, null, null, null );\r\n" \
"                clearTimeout(timeoutID);\r\n" \
"                reloadData();\r\n" \
"                \" value=\"Rescan\" class=\"button white bigrounded\"\r\n" \
"                />&nbsp;&nbsp;\r\n" \
"            </p>\r\n" \
"        </td>\r\n" \
"    </tr>\r\n" \
"</table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_scan_title = { RESOURCE_IN_MEMORY, 492, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_scan_title_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_table_start_data[125] = "<table border=\"0\" cellpadding=\"5\">\r\n" \
"  <tr>\r\n" \
"    <th>Name</th>\r\n" \
"    <th>Address</th>\r\n" \
"    <th></th>\r\n" \
"    <th></th>\r\n" \
"  </tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_table_start = { RESOURCE_IN_MEMORY, 124, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_table_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_table_end_data[11] = "</table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_table_end = { RESOURCE_IN_MEMORY, 10, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_table_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_row_start_data[9] = "  <tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_row_start = { RESOURCE_IN_MEMORY, 8, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_row_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_row_end_data[10] = "  </tr>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_row_end = { RESOURCE_IN_MEMORY, 9, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_row_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_start_data[19] = "    <td><center>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_start = { RESOURCE_IN_MEMORY, 18, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_start_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_end_data[21] = "    </center></td>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_end = { RESOURCE_IN_MEMORY, 20, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_cell_end_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start1_data[27] = "<td><label id=\"scan_result";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start1 = { RESOURCE_IN_MEMORY, 26, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start1_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start2_data[3] = "\">";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start2 = { RESOURCE_IN_MEMORY, 2, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_start2_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end1_data[143] = "</label></b>\r\n" \
"<td><form action=\"\" style=\"display:table-cell; vertical-align:top;\">\r\n" \
"<p>\r\n" \
"<input type=\"button\" onclick=\"do_connect(\'scan_result";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end1 = { RESOURCE_IN_MEMORY, 142, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end1_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end2_data[133] = "\');clearTimeout(timeoutID);reloadData();\" value=\"Connect\" class=\"button blue bigrounded\" />&nbsp;&nbsp;\r\n" \
"</p>\r\n" \
"</form></td>\r\n" \
"</td>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end2 = { RESOURCE_IN_MEMORY, 132, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_dev_addr_end2_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connection_title_data[517] = "<table border=\"0\" cellpadding=\"5\">\r\n" \
"    <tr>\r\n" \
"        <td><p style=\"font-size:18pt;line-height:25pt;\"><b>Connection List</b></td>\r\n" \
"        <td>\r\n" \
"            <p>\r\n" \
"                <input type=\"button\" onclick=\"do_ajax( \'/clear_bond_info\', null, null, 0, null, null, null, null );\r\n" \
"                clearTimeout(timeoutID);\r\n" \
"                reloadData();\r\n" \
"                \" value=\"Clear Pairing Info\" class=\"button white bigrounded\"\r\n" \
"                />&nbsp;&nbsp;\r\n" \
"            </p>\r\n" \
"        </td>\r\n" \
"    </tr>\r\n" \
"</table>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connection_title = { RESOURCE_IN_MEMORY, 516, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connection_title_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start1_data[32] = "<td><label id=\"connected_device";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start1 = { RESOURCE_IN_MEMORY, 31, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start1_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start2_data[3] = "\">";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start2 = { RESOURCE_IN_MEMORY, 2, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_start2_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end1_data[151] = "</label></b>\r\n" \
"<td><form action=\"\" style=\"display:table-cell; vertical-align:top;\">\r\n" \
"<p>\r\n" \
"<input type=\"button\" onclick=\"do_disconnect(\'connected_device";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end1 = { RESOURCE_IN_MEMORY, 150, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end1_data }}};

const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end2_data[166] = "\');clearTimeout(timeoutID);reloadData();\" value=\"Disconnect\" class=\"button red bigrounded\" />&nbsp;&nbsp;\r\n" \
"<input type=\"button\" onclick=\"do_details(\'connected_device";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end2 = { RESOURCE_IN_MEMORY, 165, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end2_data }}};
const char resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end3_data[134] = "\');clearTimeout(timeoutID);reloadData();\" value=\"Details\" class=\"button green bigrounded\" />&nbsp;&nbsp;\r\n" \
"</p>\r\n" \
"</form></td>\r\n" \
"</td>\r\n" \
"";
const resource_hnd_t resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end3 = { RESOURCE_IN_MEMORY, 133, { .mem = { resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_device_end3_data }}};
