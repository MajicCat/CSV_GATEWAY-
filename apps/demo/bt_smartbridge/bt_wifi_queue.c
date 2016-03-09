/*
 * bt_wifi_queue.c
 *
 *  Created on: March 09, 2016
 *      Author: msenel
 */
#include "wiced.h"
#include "bt_wifi_queue.h"


/* Utility function to check space */
wiced_result_t data_q_has_space(data_q_t *data_q)
{
	int r = data_q->r_indx;
	int w = data_q->w_indx;

	if ((w == 0) && (r == 0))
		return WICED_SUCCESS;

	if (w <= r)
                return ( ((w+1) <= r) ? WICED_SUCCESS : WICED_ERROR);
	else
		return ( (((w+1) % DATA_Q_SZ) > r ) ? WICED_SUCCESS : WICED_ERROR);
}

/* Utility function to check available entries in the data_q  */
/* FIXME: Need to do this in two steps. What happens if it fails? */
int data_q_data_avail_ctr(data_q_t data_q)
{
	int r = data_q.r_indx;
	int w = data_q.w_indx;

	if ((w == 0) && (r == 0))
	  return 0;

	return ((w >= r) ? (w - r + 1) : (99 + r - w) );

}

/* The caller needs to check whether there is enough space.
 * Otherwise ASSERT here
 */
wiced_result_t write_to_data_q(uint8_t new_data, data_q_t *data_q)
{
	int w = data_q->w_indx;

	/* TODO: Assert here
	if (data_q_has_space(data_q) != WICED_SUCCESS) {
		WPRINT_APP_INFO( ("[ERROR] Data Queue FULL: r%d - w%d\r\n",
				data_q->r_indx, data_q->w_indx) );
		return WICED_ERROR;
	}
	*/

	data_q->data[(w+1)%DATA_Q_SZ] = new_data;
	data_q->w_indx = (data_q->w_indx + 1) % DATA_Q_SZ;

	return WICED_SUCCESS;
}


/* FIXME: Need to read operation in two steps.
 * We are screwed, if the operation fails
 */
wiced_result_t flush_data_q (char *tx_data, data_q_t *data_q)
{
	int data_to_copy = data_q_data_avail_ctr(*data_q);

	*tx_data = (char)0xbe;
	*(tx_data + 1) = (char)0xef;
	tx_data += 2      ;
	while (data_to_copy > 0) {
		*tx_data = (char)data_q->data[data_q->r_indx];
		tx_data++;
		data_q->r_indx = (data_q->r_indx + 1) % DATA_Q_SZ;
		data_to_copy--;
	}

	*(tx_data + 1) = 0xde;
	*(tx_data + 2) = 0xad;
	WPRINT_APP_INFO(("[TCPClient]: What happened to queue w:%d, r:%d\n",
				data_q->w_indx, data_q->r_indx ));

  	return WICED_SUCCESS;
}
