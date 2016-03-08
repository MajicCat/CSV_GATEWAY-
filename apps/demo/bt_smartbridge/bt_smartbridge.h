/*
 * bt_smartbridge.h
 *
 *  Created on: Feb 24, 2016
 *      Author: msenel
 */

#ifndef APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_
#define APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_

#define DATA_Q_SZ	1000

typedef struct data_q {
	uint8_t 	data[DATA_Q_SZ];
	int		r_indx;
	int		w_indx;
} data_q_t;

extern data_q_t bt_to_wifi_data;
#endif /* APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_ */
