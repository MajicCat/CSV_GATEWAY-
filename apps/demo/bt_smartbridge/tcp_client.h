/*
 * tcp_client.h
 *
 *  Created on: Feb 18, 2016
 *      Author: msenel
 */

#ifndef APPS_DEMO_BT_SMARTBRIDGE_TCP_CLIENT_H_
#define APPS_DEMO_BT_SMARTBRIDGE_TCP_CLIENT_H_
#include "bt_wifi_queue.h"

extern data_q_t wifi_to_bt_data;

extern wiced_result_t tcp_client_init(wiced_interface_t interface);
#endif /* APPS_DEMO_BT_SMARTBRIDGE_TCP_CLIENT_H_ */
