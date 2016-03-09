/*
 * bt_wifi_queue.h
 *
 *  Created on: March 09, 2016
 *      Author: msenel
 */

#ifndef APPS_DEMO_BT_SMARTBRIDGE_BT_WIFI_QUEUE_H_
#define APPS_DEMO_BT_SMARTBRIDGE_BT_WIFI_QUEUE_H_


#define DATA_Q_SZ	1000

typedef struct data_q {
	uint8_t 	data[DATA_Q_SZ];
	int		r_indx;
	int		w_indx;
} data_q_t;

extern wiced_result_t data_q_has_space(data_q_t *data_q);
extern int data_q_data_avail_ctr(data_q_t data_q);
extern wiced_result_t write_to_data_q(uint8_t new_data, data_q_t *data_q);
extern wiced_result_t flush_data_q (char *tx_data, data_q_t *data_q);
#endif /* APPS_DEMO_BT_SMARTBRIDGE_BT_WIFI_QUEUE_H_*/
