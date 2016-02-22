/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#if   defined( RTOS_FreeRTOS )
    #define malloc_get_current_thread( )    ( xTaskGetCurrentTaskHandle( ) )
#elif defined( RTOS_NoOS     )
    #define malloc_get_current_thread( )    ( NULL)
#elif defined( RTOS_ThreadX  )
    #define malloc_get_current_thread( )    ( tx_thread_identify( ) )
#elif defined( RTOS_LinuxOS )
    #define malloc_get_current_thread( )    ( ( void* )pthread_self( ) )
#else
    #error Unknown RTOS
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void* malloc_thread_handle;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

extern void* calloc_named                  ( const char* name, size_t nelems, size_t elemsize );
extern void* calloc_named_hideleak         ( const char* name, size_t nelem, size_t elsize );
extern void* malloc_named                  ( const char* name, size_t size );
extern void* malloc_named_hideleak         ( const char* name, size_t size );
extern void  malloc_set_name               ( const char* name );
extern void  malloc_leak_set_ignored       ( leak_check_scope_t global_flag );
extern void  malloc_leak_set_base          ( leak_check_scope_t global_flag );
extern void  malloc_leak_check             ( malloc_thread_handle thread, leak_check_scope_t global_flag );
extern void  malloc_transfer_to_curr_thread( void* block );
extern void  malloc_transfer_to_thread     ( void* block, malloc_thread_handle thread );

#ifdef MALLOC_DEBUG_DISABLE_UNTIL_STARTED
extern void  malloc_debug_startup_finished( void );
#else
#define malloc_debug_startup_finished( )
#endif /* ifdef MALLOC_DEBUG_DISABLE_UNTIL_STARTED */

#ifdef __cplusplus
} /* extern "C" */
#endif
