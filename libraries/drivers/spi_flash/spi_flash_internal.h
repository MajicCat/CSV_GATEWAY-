/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#ifndef INCLUDED_SPI_FLASH_INTERNAL_H
#define INCLUDED_SPI_FLASH_INTERNAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "spi_flash.h"
#include <stdint.h>

#ifndef SFLASH_SUPPORT_MACRONIX_PARTS
#define SFLASH_SUPPORT_MACRONIX_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_ISSI_LP_PARTS
#define SFLASH_SUPPORT_ISSI_LP_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_ISSI_CQ_PARTS
#define SFLASH_SUPPORT_ISSI_CQ_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_MICRON_PARTS
#define SFLASH_SUPPORT_MICRON_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_WINBOND_PARTS
#define SFLASH_SUPPORT_WINBOND_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_SST_PARTS
#define SFLASH_SUPPORT_SST_PARTS    0
#endif

#ifndef SFLASH_SUPPORT_EON_PARTS
#define SFLASH_SUPPORT_EON_PARTS    0
#endif

/* Status Register bit definitions */
#define SFLASH_STATUS_REGISTER_BUSY                          ( (unsigned char) 0x01 )
#define SFLASH_STATUS_REGISTER_WRITE_ENABLED                 ( (unsigned char) 0x02 )
#define SFLASH_STATUS_REGISTER_BLOCK_PROTECTED_0             ( (unsigned char) 0x04 )
#define SFLASH_STATUS_REGISTER_BLOCK_PROTECTED_1             ( (unsigned char) 0x08 )
#define SFLASH_STATUS_REGISTER_BLOCK_PROTECTED_2             ( (unsigned char) 0x10 ) /* SST & Macronix Only */
#define SFLASH_STATUS_REGISTER_BLOCK_PROTECTED_3             ( (unsigned char) 0x20 ) /* SST & Macronix Only */
#define SFLASH_STATUS_REGISTER_AUTO_ADDRESS_INCREMENT        ( (unsigned char) 0x40 ) /* SST Only */
#define SFLASH_STATUS_REGISTER_BLOCK_PROTECT_BITS_READ_ONLY  ( (unsigned char) 0x80 ) /* SST Only */
#define SFLASH_STATUS_REGISTER_QUAD_ENABLE                   ( (unsigned char) 0x40 ) /* Macronix Only */
#define SFLASH_STATUS_REGISTER_WRITE_PROTECT_PIN_ENABLE      ( (unsigned char) 0x80 ) /* Macronix Only */

/* MICRON - Enhanced Volatile Configuration Register bit definitions */
#define MICRON_SFLASH_ENH_VOLATILE_STATUS_REGISTER_HOLD      ( (unsigned char) 0x10 ) /* HOLD# */

/* WINBOND - Status Register-2 bit definitions */
#define WINBOND_SFLASH_STATUS_REGISTER2_QUAD_ENABLE          ( (unsigned char) 0x02 ) /* Quad Enable*/

/* Command definitions */
typedef enum
{

    SFLASH_WRITE_STATUS_REGISTER        = 0x01, /* WRSR                   */
    SFLASH_WRITE                        = 0x02,
    SFLASH_READ                         = 0x03,
    SFLASH_WRITE_DISABLE                = 0x04, /* WRDI                   */
    SFLASH_READ_STATUS_REGISTER         = 0x05, /* RDSR                   */
    SFLASH_WRITE_ENABLE                 = 0x06, /* WREN                   */
    SFLASH_FAST_READ                    = 0x0B,
    SFLASH_SECTOR_ERASE                 = 0x20, /* SE                     */
    SFLASH_READ_STATUS_REGISTER2        = 0x35, /* RDSR-2 - Winbond only  */
    SFLASH_BLOCK_ERASE_MID              = 0x52, /* SE                     */
    SFLASH_BLOCK_ERASE_LARGE            = 0xD8, /* SE                     */
    SFLASH_READ_ID1                     = 0x90, /* data size varies       */
    SFLASH_READ_ID2                     = 0xAB, /* data size varies       */
    SFLASH_READ_JEDEC_ID                = 0x9F, /* RDID                   */
    SFLASH_CHIP_ERASE1                  = 0x60, /* CE                     */
    SFLASH_CHIP_ERASE2                  = 0xC7, /* CE                     */
    SFLASH_ENABLE_WRITE_STATUS_REGISTER = 0x50, /* EWSR   - SST only      */
    SFLASH_READ_SECURITY_REGISTER       = 0x2B, /* RDSCUR - Macronix only */
    SFLASH_WRITE_SECURITY_REGISTER      = 0x2F, /* WRSCUR - Macronix only */
    SFLASH_ENTER_SECURED_OTP            = 0xB1, /* ENSO   - Macronix only */
    SFLASH_EXIT_SECURED_OTP             = 0xC1, /* EXSO   - Macronix only */
    SFLASH_DEEP_POWER_DOWN              = 0xB9, /* DP     - Macronix only */
    SFLASH_RELEASE_DEEP_POWER_DOWN      = 0xAB, /* RDP    - Macronix only */
    SFLASH_QUAD_READ                    = 0x6B, /* QREAD  - Macronix only */
    SFLASH_QUAD_WRITE                   = 0x32, /* QWRITE - ISSI only     */
    SFLASH_X4IO_WRITE                   = 0x38, /* 4PP    - Macronix only */
    SFLASH_X4IO_READ                    = 0xEB, /* 4READ  - Macronix only */
    SFLASH_DIE_ERASE_MICRON             = 0xC4, /* Die Erase - Micron only */
    SFLASH_SECTOR_ERASE_MICRON          = 0xD8, /* Sector Erase - Micron only */
    SFLASH_CLEAR_FLAG_STATUS_REGISTER   = 0x50,  /* Micro Only */
    SFLASH_READ_FLAG_STATUS_REGISTER    = 0x70,  /* Micron Only */
    SFLASH_WRITE_ENH_VOLATILE_REGISTER  = 0x61, /* WRITE Enhance status register, Micron only */
    SFLASH_READ_ENH_VOLATILE_REGISTER   = 0x65, /* READ Enhance status register, Micron only */
} sflash_command_t;


#define SFLASH_DUMMY_BYTE ( 0xA5 )



#define SFLASH_MANUFACTURER( id )      ( (uint8_t) ( ( (id) & 0x00ff0000 ) >> 16 ) )
#define SFLASH_SUPPORTED(NAME)         ( SFLASH_SUPPORT_##NAME##_PARTS )
#define SFLASH_MANUFACTURER_SUPPORTED( id, NAME )  ( SFLASH_SUPPORTED( NAME ) && (SFLASH_MANUFACTURER( id ) == SFLASH_MANUFACTURER_##NAME ) )

#define SFLASH_MANUFACTURER_SST        ( (uint8_t) 0xBF )
#define SFLASH_MANUFACTURER_MACRONIX   ( (uint8_t) 0xC2 )
#define SFLASH_MANUFACTURER_EON        ( (uint8_t) 0x1C )
#define SFLASH_MANUFACTURER_ISSI_CQ    ( (uint8_t) 0x7F )
#define SFLASH_MANUFACTURER_ISSI_LP    ( (uint8_t) 0x9D )
#define SFLASH_MANUFACTURER_MICRON     ( (uint8_t) 0x20 )
#define SFLASH_MANUFACTURER_WINBOND    ( (uint8_t) 0xEF )

#define SFLASH_ID_MX25L8006E           ( (uint32_t) 0xC22014 )
#define SFLASH_ID_MX25L1606E           ( (uint32_t) 0xC22015 )
#define SFLASH_ID_MX25L6433F           ( (uint32_t) 0xC22017 )
#define SFLASH_ID_MX25L25635F          ( (uint32_t) 0xC22019 )
#define SFLASH_ID_MX25U1635F           ( (uint32_t) 0xC22535 )
#define SFLASH_ID_MX66U51235F          ( (uint32_t) 0xC2253A )
#define SFLASH_ID_SST25VF080B          ( (uint32_t) 0xBF258E )
#define SFLASH_ID_EN25QH16             ( (uint32_t) 0x1C3015 )
#define SFLASH_ID_ISSI25CQ032          ( (uint32_t) 0x7F9D46 )
#define SFLASH_ID_N25Q512A             ( (uint32_t) 0x20BB20 )
#define SFLASH_ID_ISSI25LP064          ( (uint32_t) 0x9D6017 )
#define SFLASH_ID_N25Q064A             ( (uint32_t) 0x20BA17 )
#define SFLASH_ID_W25Q64FV             ( (uint32_t) 0xEF4017 )

#define SFLASH_ID_DEFAULT              ( (uint32_t) 0x000000 )

typedef struct
{
        unsigned char id[3];
} device_id_t;

int sflash_read_ID              ( const sflash_handle_t* handle, /*@out@*/ /*@dependent@*/ device_id_t* data_addr );
int sflash_write_enable         ( const sflash_handle_t* handle );
int sflash_read_status_register ( const sflash_handle_t* handle, /*@out@*/  /*@dependent@*/ unsigned char* dest_addr );
int sflash_read_status_register2 ( const sflash_handle_t* handle, /*@out@*/  /*@dependent@*/ unsigned char* dest_addr );
int sflash_write_status_register( const sflash_handle_t* handle, unsigned char value );




#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_SPI_FLASH_INTERNAL_H */
