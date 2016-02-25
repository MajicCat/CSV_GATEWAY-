/*
 * bt_smartbridge.h
 *
 *  Created on: Feb 24, 2016
 *      Author: msenel
 */

#ifndef APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_
#define APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_

typedef struct data_q {
	uint8_t data[100];
	int		r_indx;
	int		w_indx;
} data_q_t;

#endif /* APPS_DEMO_BT_SMARTBRIDGE_BT_SMARTBRIDGE_H_ */
