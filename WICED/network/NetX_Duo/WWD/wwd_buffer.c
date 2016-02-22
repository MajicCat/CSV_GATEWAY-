/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "nx_api.h"

#ifndef WICED_CUSTOM_NX_USER_H
#error NetX has been installed incorrectly - /network/NetX/verX.X/nx_user.h has been overwritten, please restore WICED nx_user.h
#endif /* ifndef WICED_LINK_MTU */

#include "network/wwd_buffer_interface.h"
#include "network/wwd_network_constants.h"
#include "platform/wwd_bus_interface.h"
#include "wwd_assert.h"
#include "wwd_bus_protocol.h"

#define IS_POOL_FULL( pool )   ( ( (pool) == NULL ) || ( (pool)->nx_packet_pool_available == (pool)->nx_packet_pool_total ) )

static NX_PACKET_POOL* rx_pool = NULL;
static NX_PACKET_POOL* tx_pool = NULL;

static NX_PACKET_POOL* application_defined_rx_pool = NULL;
static NX_PACKET_POOL* application_defined_tx_pool = NULL;


wwd_result_t wwd_buffer_init( void * pools_in )
{
    wiced_assert("Error: Invalid buffer pools\n", pools_in != NULL);
    tx_pool = &( (NX_PACKET_POOL*) pools_in )[0];
    rx_pool = &( (NX_PACKET_POOL*) pools_in )[1];

    return WWD_SUCCESS;
}

wwd_result_t host_buffer_add_application_defined_pool( void* pool_in, wwd_buffer_dir_t direction )
{
    if( direction == WWD_NETWORK_TX )
    {
        application_defined_tx_pool = (NX_PACKET_POOL*) pool_in;
    }
    else
    {
        application_defined_rx_pool = (NX_PACKET_POOL*) pool_in;
    }
    return WWD_SUCCESS;
}

wiced_bool_t host_buffer_pool_is_full( wwd_buffer_dir_t direction )
{
    wiced_bool_t result;

    if( direction == WWD_NETWORK_TX )
    {
        result = IS_POOL_FULL( tx_pool) && IS_POOL_FULL( application_defined_tx_pool );
    }
    else
    {
        result = IS_POOL_FULL( rx_pool) && IS_POOL_FULL( application_defined_rx_pool );
    }

    return result;
}

wwd_result_t host_buffer_check_leaked( void )
{
    wiced_assert( "TX Buffer leakage", host_buffer_pool_is_full( WWD_NETWORK_TX ) );
    wiced_assert( "RX Buffer leakage", host_buffer_pool_is_full( WWD_NETWORK_RX ) );
    return WWD_SUCCESS;
}

wwd_result_t internal_host_buffer_get( wiced_buffer_t * buffer, wwd_buffer_dir_t direction, unsigned short size, unsigned long timeout_ms )
{
    volatile UINT status;
    NX_PACKET **nx_buffer = (NX_PACKET **) buffer;
    NX_PACKET_POOL* pool = ( direction == WWD_NETWORK_TX ) ? tx_pool : rx_pool;
    wiced_assert("Error: pools have not been set up\n", pool != NULL);

    if ( size > WICED_LINK_MTU )
    {
        WPRINT_NETWORK_DEBUG(("Attempt to allocate a buffer larger than the MTU of the link\n"));
        return WWD_BUFFER_UNAVAILABLE_PERMANENT;
    }

    status = nx_packet_allocate( pool, nx_buffer, 0, timeout_ms );

    if( status == NX_NO_PACKET )
    {
        /* If there is no packet in the system pool, check whether there is any packet available in the application defined pool */
        /* and try to allocate from there */
        pool = ( direction == WWD_NETWORK_TX ) ? application_defined_tx_pool : application_defined_rx_pool;
        if ( pool != NULL )
        {
            status = nx_packet_allocate( pool, nx_buffer, 0, timeout_ms );
        }
    }

    /* No packet available from any pool */
    if ( status == NX_NO_PACKET )
    {
        return WWD_BUFFER_UNAVAILABLE_TEMPORARY;
    }

    if ( status != NX_SUCCESS )
    {
        return WWD_BUFFER_ALLOC_FAIL;
    }

    ( *nx_buffer )->nx_packet_length = size;
    ( *nx_buffer )->nx_packet_append_ptr = ( *nx_buffer )->nx_packet_prepend_ptr + size;
    return WWD_SUCCESS;
}

wwd_result_t host_buffer_get( wiced_buffer_t * buffer, wwd_buffer_dir_t direction, unsigned short size, wiced_bool_t wait )
{
    unsigned long wait_option = ( wait == WICED_TRUE ) ? NX_WAIT_FOREVER : NX_NO_WAIT;
    return internal_host_buffer_get(buffer, direction, size, wait_option);
}

void host_buffer_release( wiced_buffer_t buffer, wwd_buffer_dir_t direction )
{
    wiced_assert( "Error: Invalid buffer\n", buffer != NULL );

    if ( direction == WWD_NETWORK_TX )
    {
        NX_PACKET *nx_buffer = (NX_PACKET *) buffer;

        /* TCP transmit packet isn't returned immediately to the pool. The stack holds the packet temporarily
         * until ACK is received. Otherwise, the same packet is used for re-transmission.
         * Return prepend pointer to the original location which the stack expects (the start of IP header).
         * For other packets, resetting prepend pointer isn't required.
         */
        if ( nx_buffer->nx_packet_length > HOST_BUFFER_RELEASE_REMOVE_AT_FRONT_FULL_SIZE )
        {
            if ( host_buffer_add_remove_at_front( &buffer, HOST_BUFFER_RELEASE_REMOVE_AT_FRONT_FULL_SIZE ) != WWD_SUCCESS )
            {
                WPRINT_NETWORK_DEBUG(("Could not move packet pointer\r\n"));
            }
        }

        if ( NX_SUCCESS != nx_packet_transmit_release( nx_buffer ) )
        {
            WPRINT_NETWORK_ERROR(("Could not release packet - leaking buffer\n"));
        }
    }
    else
    {
        NX_PACKET *nx_buffer = (NX_PACKET *) buffer;
        if ( NX_SUCCESS != nx_packet_release( nx_buffer ) )
        {
            WPRINT_NETWORK_ERROR(("Could not release packet - leaking buffer\n"));
        }
    }
}

/*@exposed@*/ uint8_t* host_buffer_get_current_piece_data_pointer( wiced_buffer_t buffer )
{
    NX_PACKET *nx_buffer = (NX_PACKET *) buffer;
    wiced_assert("Error: Invalid buffer\n", buffer != NULL);
    return nx_buffer->nx_packet_prepend_ptr;
}

uint16_t host_buffer_get_current_piece_size( wiced_buffer_t buffer )
{
    NX_PACKET *nx_buffer = (NX_PACKET *) buffer;
    wiced_assert("Error: Invalid buffer\n", buffer != NULL);
    return (unsigned short) nx_buffer->nx_packet_length;
}

/*@exposed@*/ /*@null@*/ wiced_buffer_t host_buffer_get_next_piece( wiced_buffer_t buffer )
{
    NX_PACKET *nx_buffer = (NX_PACKET *) buffer;
    wiced_assert("Error: Invalid buffer\n", buffer != NULL);
    return nx_buffer->nx_packet_next;
}

wwd_result_t host_buffer_add_remove_at_front( wiced_buffer_t * buffer, int32_t add_remove_amount )
{
    NX_PACKET **nx_buffer = (NX_PACKET **) buffer;
    UCHAR * new_start = ( *nx_buffer )->nx_packet_prepend_ptr + add_remove_amount;

    wiced_assert("Error: Invalid buffer\n", buffer != NULL);

    if ( new_start < ( *nx_buffer )->nx_packet_data_start )
    {
#ifdef SUPPORT_BUFFER_CHAINING

        /* Requested position will not fit in current buffer - need to chain one in front of current buffer */
        NX_PACKET *new_nx_buffer;

        if ( NX_SUCCESS != ( status = nx_packet_allocate( (*nx_buffer)->nx_packet_pool_owner, &new_nx_buffer, 0, NX_NO_WAIT ) ) )
        {
            WPRINT_NETWORK_DEBUG(("Could not allocate another buffer to prepend in front of existing buffer\n"));
            return -1;
        }
        /* New buffer has been allocated - set it up at front of chain */
        (*new_nx_buffer)->nx_packet_length = -add_remove_amount;
        (*new_nx_buffer)->nx_packet_append_ptr = (*nx_buffer)->nx_packet_prepend_ptr - add_remove_amount;
        (*new_nx_buffer)->nx_packet_next = nx_buffer;
        *nx_buffer = new_nx_buffer;
        new_start = (*nx_buffer)->nx_packet_prepend_ptr;

#else /* ifdef SUPPORT_BUFFER_CHAINING */
        /* Trying to move to a location before start - not supported without buffer chaining*/
        WPRINT_NETWORK_ERROR(("Attempt to move to a location before start - not supported without buffer chaining\n"));
        return WWD_BUFFER_POINTER_MOVE_ERROR;

#endif /* ifdef SUPPORT_BUFFER_CHAINING */
    }
    else if ( new_start > ( *nx_buffer )->nx_packet_data_end )
    {

#ifdef SUPPORT_BUFFER_CHAINING
        /* moving to a location after end of current buffer - remove buffer from chain */
        NX_PACKET *new_head_nx_buffer = (*nx_buffer)->nx_packet_next;
        if ( new_head_nx_buffer == NULL )
        {
            /* there are no buffers after current buffer - can't move to requested location */
            WPRINT_NETWORK_DEBUG(("Can't move to requested location - there are no buffers after current buffer\n"));
            return -3;
        }
        new_head_nx_buffer->nx_packet_length -= (new_start - nx_buffer->nx_packet_append_ptr);
        new_head_nx_buffer->nx_packet_prepend_ptr += (new_start - nx_buffer->nx_packet_append_ptr);
        (*nx_buffer)->nx_packet_next = NULL;

        if ( NX_SUCCESS != (status = nx_packet_release( *nx_buffer ) ) )
        {
            WPRINT_NETWORK_DEBUG(("Could not release packet after removal from chain- leaking buffer\n"));
            return -4;
        }

        *nx_buffer = new_head_nx_buffer;
        new_start = (*nx_buffer)->nx_packet_prepend_ptr;

#else /* ifdef SUPPORT_BUFFER_CHAINING */
        /* Trying to move to a location after end of buffer - not supported without buffer chaining */
        WPRINT_NETWORK_ERROR(("Attempt to move to a location after end of buffer - not supported without buffer chaining\n"));
        return WWD_BUFFER_POINTER_MOVE_ERROR;

#endif /* ifdef SUPPORT_BUFFER_CHAINING */
    }
    else
    {
        ( *nx_buffer )->nx_packet_prepend_ptr = new_start;
        if (( *nx_buffer )->nx_packet_append_ptr < ( *nx_buffer )->nx_packet_prepend_ptr )
        {
            ( *nx_buffer )->nx_packet_append_ptr = ( *nx_buffer )->nx_packet_prepend_ptr;
        }
        ( *nx_buffer )->nx_packet_length = (ULONG) ( ( *nx_buffer )->nx_packet_length - (ULONG) add_remove_amount );
    }
    return WWD_SUCCESS;
}

wwd_result_t host_buffer_set_size( wiced_buffer_t buffer, unsigned short size )
{
    NX_PACKET *nx_buffer = (NX_PACKET *) buffer;
    if ( size > WICED_LINK_MTU )
    {
        WPRINT_NETWORK_DEBUG(("Attempt to set a length larger than the MTU of the link\n"));
        return WWD_BUFFER_SIZE_SET_ERROR;
    }
    nx_buffer->nx_packet_length = size;
    nx_buffer->nx_packet_append_ptr = nx_buffer->nx_packet_prepend_ptr + size;
    return WWD_SUCCESS;
}

void host_buffer_push_to_fifo( wiced_buffer_fifo_t* fifo, wiced_buffer_t buffer, wwd_interface_t interface )
{
    buffer->nx_packet_ip_interface = (struct NX_INTERFACE_STRUCT*)interface;
    buffer->nx_packet_queue_next   = NULL;

    if ( fifo->first == NULL )
    {
        fifo->first = buffer;
        fifo->last  = buffer;
    }
    else
    {
        fifo->last->nx_packet_queue_next = buffer;
        fifo->last                       = buffer;
    }
}

wiced_buffer_t host_buffer_pop_from_fifo( wiced_buffer_fifo_t* fifo, wwd_interface_t* interface )
{
    wiced_buffer_t buffer = NULL;

    if ( fifo->first != NULL )
    {
        buffer      = fifo->first;
        *interface  = (wwd_interface_t)buffer->nx_packet_ip_interface;
        fifo->first = buffer->nx_packet_queue_next;
    }

    return buffer;
}

void packet_release_notify( void* pool )
{
#ifdef PLAT_NOTIFY_FREE
    if ( ( pool == rx_pool ) || ( pool == application_defined_rx_pool ) )
    {
        host_platform_bus_buffer_freed( WWD_NETWORK_RX );
    }
    else
    {
        host_platform_bus_buffer_freed( WWD_NETWORK_TX );
    }
#endif /* ifdef PLAT_NOTIFY_FREE */

    UNUSED_PARAMETER( pool );
}
