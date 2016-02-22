/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Crypto test Application
 *
 * Test cyrpto apis, with 128 byte aes, 256 byte sha hash
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app test 128 byte aes and 256 byte sha hash with hmacsha256_signed_aes128cbc_encrypted_80K.bin
 *
 */
#include "wiced.h"
#include "crypto_core.h"
#include "typedefs.h"
#include "crypto_api.h"
#include "wicedfs.h"
#include "platform_resource.h"
#include <crypto_api.h>
#include "spi_flash.h"
#include <malloc.h>
#include "hwcrypto_test.h"
/** @file
 *
 */

/******************************************************
 *                      Macros
 ******************************************************/
#define TEST_FAIL_ACTION( msg ) { printf msg; }
#define ERROR_CHECK(   condition, description ) { if (!(condition)) { TEST_FAIL_ACTION(("Crypto Test failed: %s (" #condition ") at %s:%d\n" ,description, __FILE__, __LINE__ )); } return 0;}

#define CRYPTO_OPTIMIZED_ALIGNMENT  MAX(CRYPTO_ALIGNMENT, PLATFORM_L1_CACHE_BYTES)
/******************************************************
 *                    Constants
 ******************************************************/

/* The MAX_CHUNK_SIZE
 * 1. should be multiple of 64
 * 2. should not exceed ((SPUM_MAX_PAYLOAD_SIZE) - 64)
 */
#define MAX_CHUNK_SIZE      ((8*1024))
#define CRYPTO_ALIGNMENT    16

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

wiced_result_t sha256_hmac_signature_verify( char *file_name, uint8_t* hmacsha256_key, uint32_t hmacshakeylen );
wiced_result_t aes128_decrypt_and_verify( char *enc_file_name, char* dec_file_name, uint8_t *aes128_key, uint8_t *aes128_iv );
wiced_result_t aes128decrypt_sha256hmac_verify( char *file_name, uint8_t *hmacsha256_key, uint32_t hmacsha256_keysize, uint8_t *aes128_key, uint8_t *aes128_iv );
static void cryptotest_calculate_input_output_keypad( uint8_t* key, uint32_t keylen, uint8_t i_key_pad[ ], uint8_t o_key_pad[ ] );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/*!
 ******************************************************************************
 * Test cyrpto apis, with 128 byte aes, 256 byte sha hash
 *
 * @return  0 for success, otherwise error
 */
void hwcrypto_test( )
{
    //int i;
    uint8_t hmacsha256_key[ ] = "aaaaaaaaaaaaaaaa";
    uint8_t aes128_key[ ] = "BcmEncryptionKey";
    uint8_t *aes128_iv;

    aes128_iv = malloc( AES_BLOCK_SZ );
    if ( NULL == aes128_iv )
    {
        printf( "malloc of iv fails\n" );
        return;
    }

    /* Change this if AES 128 iv is not all 0 */
    memset( aes128_iv, 0, AES_BLOCK_SZ );

    platform_hwcrypto_init( );
    printf( "Crypto Test Start\n" );

    printf( "Calling aes128decrypt_sha256hmac_verify() ...\n" );
    aes128decrypt_sha256hmac_verify( "hmacsha256_signed_aes128cbc_encrypted_80K.bin", hmacsha256_key, sizeof( hmacsha256_key ), aes128_key, aes128_iv );

    free( aes128_iv );
    //i = 0;

    printf( "Crypto Test End \n" );
}

/* Decrypt and sign verify a file CHUNK by CHUNK */

wiced_result_t aes128decrypt_sha256hmac_verify( char *file_name, uint8_t *keyhmacsha256, uint32_t hmacsha256_keysize, uint8_t *aes128_key, uint8_t *aes128_iv )
{

    uint8_t        i_key_pad[ HMAC256_INNERHASHCONTEXT_SIZE ] = { 0 };
    uint8_t        o_key_pad[ HMAC256_OUTERHASHCONTEXT_SIZE ] = { 0 };
    uint8_t        store_buffer[ HMAC256_OUTERHASHCONTEXT_SIZE + HMAC256_OUTPUT_SIZE ] = { 0 };
    int            prev_len = 0;
    uint8_t*       encrypted_data = NULL;
    uint8_t*       decrypted_data = NULL;
    uint8_t        hash_output[ 32 ] __attribute__((aligned(CRYPTO_OPTIMIZED_ALIGNMENT)));
    wicedfs_file_t fp;
    int            remaining;
    int            filesize;
    int            result;

    result      = wicedfs_fopen( &resource_fs_handle, &fp, file_name );
    ERROR_CHECK( result == 0, "wicedfs_open ");

    wicedfs_fseek( &fp, 0, SEEK_END );
    ERROR_CHECK( result == 0, "wicedfs_fseek ");

    filesize    = wicedfs_ftell( &fp );
    ERROR_CHECK( result == 0, "wicedfs_ftell ");

    wicedfs_fseek( &fp, 0, SEEK_SET );
    ERROR_CHECK( result == 0, "wicedfs_fseek ");

    /* Buffer to hold Encrypted data read from File */
    encrypted_data = (uint8_t*) memalign( CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT, MAX_CHUNK_SIZE );
    ERROR_CHECK( NULL != encrypted_data, " Failed to malloc  ");

    /* Buffer to hold Decrypted data */
    decrypted_data = (uint8_t*) memalign( CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT, MAX_CHUNK_SIZE );
    ERROR_CHECK( NULL != encrypted_data, " Failed to malloc  ");

    /* HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
     * Calculate (key XOR ipad) and (key XOR opad) as described above
     */
    cryptotest_calculate_input_output_keypad( keyhmacsha256, hmacsha256_keysize, i_key_pad, o_key_pad );

    /* Last (SHA256_HASH_SIZE) bits are signature */
    if ( filesize < ( MAX_CHUNK_SIZE - SHA256_HASH_SIZE ) )
    {
        result = wicedfs_fread( encrypted_data, 1, ( filesize - SHA256_HASH_SIZE ), &fp ) != ( filesize - SHA256_HASH_SIZE );
        ERROR_CHECK( result == 0, "wicedfs_fread ");

        platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );
        platform_hwcrypto_sha256_hash( decrypted_data, ( filesize - SHA256_HASH_SIZE ), store_buffer, hash_output);
    }
    else
    {
        /* Perform SHA256 HASH in 3 steps
         * INIT     1 call              (i_keypad (64 bytes) + input_data ( MAX_CHUNK_SIZE - 64 ) bytes
         * UPDT     (0 or more calls)   ( MAX_CHUNK_SIZE) bytes
         * FINAL    1 call              ( remaining input data size after the above operations)
         */
        result = wicedfs_fread( encrypted_data, 1, ( MAX_CHUNK_SIZE - HMAC256_INNERHASHCONTEXT_SIZE ), &fp );
        ERROR_CHECK( result == ( MAX_CHUNK_SIZE - HMAC256_INNERHASHCONTEXT_SIZE ), "wicedfs_fread ");

        platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );

        platform_hwcrypto_sha256_hash( decrypted_data, ( filesize - SHA256_HASH_SIZE ), store_buffer, hash_output);

        remaining   = ( filesize - SHA256_HASH_SIZE );

        /* INIT has already hashed ( MAX_CHUNK_SIZE - HMAC256_INNERHASHCONTEXT_SIZE ) bytes of input data */
        remaining   = (int) ( remaining - ( MAX_CHUNK_SIZE - HMAC256_INNERHASHCONTEXT_SIZE ) );

        /* Length of data hashed till now in 64K blocks */
        prev_len    = MAX_CHUNK_SIZE / BD_PREVLEN_BLOCKSZ;
        while ( remaining > MAX_CHUNK_SIZE )
        {

            result  = wicedfs_fread( encrypted_data, 1, MAX_CHUNK_SIZE, &fp );
            ERROR_CHECK( result == MAX_CHUNK_SIZE, "wicedfs_fread ");

            platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );

            platform_hwcrypto_sha256_hash( decrypted_data, ( filesize - SHA256_HASH_SIZE ), store_buffer, hash_output);

            remaining   -= MAX_CHUNK_SIZE;
            prev_len    += MAX_CHUNK_SIZE / BD_PREVLEN_BLOCKSZ;
        }

        result = wicedfs_fread( encrypted_data, 1, remaining, &fp );
        ERROR_CHECK( result == remaining, "wicedfs_fread ");

        platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );

        platform_hwcrypto_sha256_hash( decrypted_data, ( filesize - SHA256_HASH_SIZE ), store_buffer, hash_output);
    }

    /* Compute HMAC = HASH(key XOR opad || innerHash) */
    memcpy( &store_buffer[ 0 ], o_key_pad, HMAC256_OUTERHASHCONTEXT_SIZE );
    memcpy( &store_buffer[ HMAC256_OUTERHASHCONTEXT_SIZE ], hash_output, HMAC256_OUTPUT_SIZE );


    platform_hwcrypto_sha256_hash( decrypted_data, ( filesize - SHA256_HASH_SIZE ), store_buffer, hash_output);

    result = wicedfs_fread( encrypted_data, 1, SHA256_HASH_SIZE, &fp );
    ERROR_CHECK( result == SHA256_HASH_SIZE, "wicedfs_fread ");

    platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );

    if ( memcmp( decrypted_data, (uint32_t*) hash_output, SHA256_HASH_SIZE ) )
    {
        uint32_t *expected, *result;
        int i;

        expected    = (uint32_t*) decrypted_data;
        result      = (uint32_t*) hash_output;
        for ( i = 0; i < (signed) ( SHA256_HASH_SIZE / 4 ); i++ )
        {
            printf( "Expected %lx Got %lx\n", expected[ i ], result[ i ] );
        }

        printf( " SHA256 HMAC Signature does not match , TEST FAIL \n" );
    }
    else
    {
        printf( " SHA256 HMAC Signature matches , TEST PASS\n" );
    }

    wicedfs_fclose( &fp );
    free( encrypted_data );
    free( decrypted_data );

    return WICED_SUCCESS;
}

/* For incremental HMAC operations calculate the InnerHashContext (key XOR ipad)
 * and OuterHashContext (key XOR opad)
 */
static void cryptotest_calculate_input_output_keypad( uint8_t* key, uint32_t keylen, uint8_t i_key_pad[ ], uint8_t o_key_pad[ ] )
{
    int i = 0;

    memcpy( i_key_pad, key, keylen );
    memcpy( o_key_pad, key, keylen );

    for ( i = 0; i < HMAC_BLOCK_SIZE; i++ )
    {
        i_key_pad[ i ] ^= HMAC_IPAD;
        o_key_pad[ i ] ^= HMAC_OPAD;
    }
}

/* Verify SHA-256 HMAC signature of a File (Signature is appended to the file) */
wiced_result_t sha256_hmac_signature_verify( char *file_name, uint8_t* hmacsha256_key, uint32_t hmacsha256_keylen )
{
    uint8_t*       output;
    wicedfs_file_t infp;
    uint8_t*       encbuffer = NULL;
    uint32         filesize;
    uint8_t        hash_output[ 32 ] __attribute__((aligned(CRYPTO_OPTIMIZED_ALIGNMENT)));
    uint8_t*       expected_signature;
    int            result;

    result      = wicedfs_fopen( &resource_fs_handle, &infp, file_name );
    ERROR_CHECK( result==0, "wicedfs_fopen fails");

    result      = wicedfs_fseek( &infp, 0, SEEK_END );
    ERROR_CHECK( result==0, "wicedfs_fseek fails");

    filesize    = wicedfs_ftell( &infp );
    ERROR_CHECK( filesize > 0, "wicedfs_ftell fails");

    result      = wicedfs_fseek( &infp, 0, SEEK_SET );
    ERROR_CHECK( result==0, "wicedfs_fseek fails");

    encbuffer   = malloc( filesize );
    ERROR_CHECK( encbuffer != NULL, "malloc fails ");

    result      = wicedfs_fread( encbuffer, 1, filesize, &infp );
    ERROR_CHECK (result == filesize, "File read error");

    output      = (uint8_t*) ( hash_output );

    /* Expected Signature is appended to the file */
    expected_signature = encbuffer + ( filesize - SHA256_HASH_SIZE );

    //platform_hwcrypto_verify_sha256hmac( encbuffer, ( filesize - SHA256_HASH_SIZE ), hmacsha256_key, hmacsha256_keylen, output );
    ///////////////////////////////////

    if ( memcmp( expected_signature, (uint32_t*) output, SHA256_HASH_SIZE ) )
    {
        uint32_t *expected, *result;
        int i;

        expected    = (uint32_t*) expected_signature;
        result      = (uint32_t*) output;
        for ( i = 0; i < (signed) ( SHA256_HASH_SIZE / 4 ); i++ )
        {
            printf( "Expected %lx Got %lx\n", expected[ i ], result[ i ] );
        }

        printf( " SHA256 HMAC Signature does not match , TEST FAIL  \n" );
    }
    else
    {
        printf( " SHA256 HMAC Signature matches , TEST PASS\n" );
    }

    if ( encbuffer != NULL )
    {
        free( encbuffer );
    }
    wicedfs_fclose( &infp );
    return WICED_SUCCESS;
}

/* ASE128 CBC Decrypt the enc_file_name and verify the decrypted data with
 * dec_file_name
 */
wiced_result_t aes128_decrypt_and_verify( char *enc_file_name, char* dec_file_name, uint8_t *aes128_key, uint8_t *aes128_iv )
{
    wicedfs_file_t encfp;
    wicedfs_file_t decfp;
    uint8_t*       encrypted_data = NULL;
    uint8_t*       decrypted_data = NULL;
    uint32_t       filesize;
    int            data_size;
    int            remaining_size;
    int            result = 0;
    uint8_t*       crypto_output = NULL;

    result      = wicedfs_fopen( &resource_fs_handle, &encfp, enc_file_name );
    ERROR_CHECK( result == 0, "wicedfs_open");

    result      = wicedfs_fseek( &encfp, 0, SEEK_END );
    ERROR_CHECK( result == 0, "wicedfs_fseek ");

    filesize    = wicedfs_ftell( &encfp );
    ERROR_CHECK( filesize > 0, "wicedfs_fseek ");

    wicedfs_fseek( &encfp, 0, SEEK_SET );

    result      = wicedfs_fopen( &resource_fs_handle, &decfp, dec_file_name );
    ERROR_CHECK( result == 0, "wicedfs_open");

    encrypted_data      = malloc( MAX_CHUNK_SIZE );
    ERROR_CHECK( NULL != encrypted_data, " Failed to malloc  ");

    decrypted_data      = malloc( MAX_CHUNK_SIZE );
    ERROR_CHECK( NULL != encrypted_data, " Failed to malloc  ");

    crypto_output       = (uint8_t*) memalign( CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT, MAX_CHUNK_SIZE );
    ERROR_CHECK( NULL != crypto_output, " Failed to malloc  ");

    remaining_size = filesize;

    while ( remaining_size > 0 )
    {
        data_size = MIN(remaining_size, MAX_CHUNK_SIZE);

        /* Decrypt data */
        result = wicedfs_fread( encrypted_data, 1, data_size, &encfp );
        ERROR_CHECK( result == data_size, "wicedfs read");

        platform_hwcrypto_aes128cbc_decrypt( aes128_key, sizeof(aes128_key), aes128_iv, (filesize - SHA256_HASH_SIZE), encrypted_data, decrypted_data );

        /* Compare with Expected values */
        result = wicedfs_fread( decrypted_data, 1, data_size, &decfp );
        ERROR_CHECK( result == data_size, "wicedfs read");

        result = memcmp( crypto_output, decrypted_data, data_size );
        if ( 0 != result )
            break;

        remaining_size -= data_size;
    }

    if ( 0 != result )
    {
        printf( "AES128 CBC Decryption of %s Fails\n", enc_file_name );
    }
    else
    {
        printf( "AES128 CBC Decryption of %s is successful\n", enc_file_name );
    }

    wicedfs_fclose( &decfp );
    wicedfs_fclose( &encfp );
    free( encrypted_data );
    free( decrypted_data );
    free( crypto_output );

    return WICED_SUCCESS;
}
