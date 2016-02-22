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
 * AES128-CBC Encrypt/Decrypt input file
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <aes.h>

#define BCM_KEY_SIZE        (16)
#define AES_BLOCK_SZ        (16)
#define AES_KEY_BITLEN(x)   (x*8)
#define MIN(x,y) ((x) < (y) ? (x) : (y))

int readfile( const char *infile, char *buf, int maxlen )
{
    FILE *infp;
    int filelen = 0;

    infp = fopen( infile, "rb" );
    if ( !infp )
    {
        printf( "* Failed to open input file '%s' - %s\n", infile, strerror( errno ) );
        return -1;
    }

    filelen = fread( buf, 1, maxlen, infp );
    if ( filelen < 0 )
    {
        printf( "* Failed to read data from file '%s'\n", infile );
    }

    fclose( infp );

    return filelen;
}

int aes_cbc_128_test( void )
{
    return 0;
}

int aes_cbc_128_enc( const char *keyfile, const char *infile, const char *outfile, uint32_t blocksize )
{
    uint8_t key[ BCM_KEY_SIZE ];
    uint8_t iv[ BCM_KEY_SIZE ];
    aes_context_t aes_ctx;
    uint8_t *filebuffer = NULL;
    uint8_t *encbuffer = NULL;
    uint32_t filesize, padded_filesize;
    int32_t encsize;
    uint32_t keysize;
    FILE *infp = NULL;
    FILE *outfp = NULL;
    int32_t success = 1;
    int32_t remaining;
    char zero_char = '\0';

    keysize = readfile( keyfile, key, sizeof( key ) );

    if ( keysize != BCM_KEY_SIZE )
    {
        success = 0;
        goto finalize;
    }

    infp = fopen( infile, "rb" );
    if ( !infp )
    {
        printf( "* Failed to open input file '%s' - %s\n", infile, strerror( errno ) );
        success = 0;
        goto finalize;
    }

    fseek( infp, 0L, SEEK_END );
    filesize = ftell( infp );
    fclose( infp );
    padded_filesize = ( ( ( filesize - 1 ) >> 4 ) + 1 ) << 4;

    if ( padded_filesize != filesize )
    {
        uint32_t padding = padded_filesize - filesize;

        printf( "[aes_cbc_128] padding %s [%d -> %d Bytes] to make it's size AES_BLOCK_SIZE (16) aligned\n", infile, filesize, padded_filesize );
        infp = fopen( infile, "ab" );
        if ( !infp )
        {
            printf( "* Failed to open input file '%s' - %s\n", infile, strerror( errno ) );
            success = 0;
            goto finalize;
        }

        while ( padding != 0 )
        {
            fwrite( &zero_char, sizeof(char), 1, infp );
            padding-- ;
        }

        fclose( infp );
    }
    infp = fopen( infile, "rb" );
    if ( !infp )
    {
        printf( "* Failed to open input file '%s' - %s\n", infile, strerror( errno ) );
        success = 0;
        goto finalize;
    }

    fseek( infp, 0L, SEEK_SET );

    if ( blocksize == 0 )
        blocksize = padded_filesize;

    filebuffer = malloc( blocksize );
    if ( filebuffer == NULL )
    {
        printf( "* Failed to allocation filebuffer of file '%s'\n", infile );
        success = 0;
        goto finalize;
    }
    encbuffer = malloc( blocksize );
    if ( encbuffer == NULL )
    {
        printf( "* Failed to allocation encrypt buffer of file '%s'\n", infile );
        success = 0;
        goto finalize;
    }

    outfp = fopen( outfile, "wb" );
    remaining = padded_filesize;

    while ( remaining > 0 )
    {

        uint32_t read_size;

        memset( filebuffer, 0, blocksize );
        memset( encbuffer, 0, blocksize );
        memset( iv, 0, AES_BLOCK_SZ );
        aes_setkey_enc( &aes_ctx, key, AES_KEY_BITLEN(keysize) );
        read_size = MIN(blocksize, remaining);

        if ( fread( filebuffer, 1, read_size, infp ) != read_size )
        {
            printf( "* File '%s' read error\n", infile );
            success = 0;
            goto finalize;
        }

        aes_crypt_cbc( &aes_ctx, AES_ENCRYPT, read_size, iv, filebuffer, encbuffer );

        if ( fwrite( encbuffer, 1, read_size, outfp ) != read_size )
        {
            success = 0;
            printf( "* Failed to write output file '%s' - %s\n", outfile, strerror( errno ) );
            goto finalize;
        }

        remaining -= read_size;
    }

    finalize:

    if ( infp )
        fclose( infp );
    if ( outfp )
        fclose( outfp );
    if ( filebuffer )
        free( filebuffer );
    if ( encbuffer )
        free( encbuffer );
    if ( !success )
        remove( outfile );

    return success;
}

int aes_cbc_128_dec( const char *keyfile, const char *infile, const char *outfile, uint32_t blocksize )
{
    uint8_t key[ BCM_KEY_SIZE ];
    uint8_t iv[ BCM_KEY_SIZE ];
    aes_context_t aes_ctx;
    uint8_t *filebuffer = NULL;
    uint8_t *decbuffer = NULL;
    uint32_t filesize, padded_filesize;
    int32_t decsize;
    uint32_t keysize;
    FILE *infp = NULL;
    FILE *outfp = NULL;
    int32_t success = 1;
    int32_t remaining;

    keysize = readfile( keyfile, key, sizeof( key ) );

    if ( keysize != BCM_KEY_SIZE )
    {
        success = 0;
        goto finalize;
    }

    infp = fopen( infile, "rb" );
    if ( !infp )
    {
        printf( "* Failed to open input file '%s' - %s\n", infile, strerror( errno ) );
        success = 0;
        goto finalize;
    }

    fseek( infp, 0L, SEEK_END );
    filesize = ftell( infp );
    padded_filesize = ( ( ( filesize - 1 ) >> 4 ) + 1 ) << 4;
    fseek( infp, 0L, SEEK_SET );

    if ( blocksize == 0 )
        blocksize = padded_filesize;

    remaining = filesize;
    filebuffer = malloc( blocksize );
    if ( filebuffer == NULL )
    {
        printf( "* Failed to allocation filebuffer of file '%s'\n", infile );
        success = 0;
        goto finalize;
    }
    decbuffer = malloc( blocksize );
    if ( decbuffer == NULL )
    {
        printf( "* Failed to allocation decrypt buffer of file '%s'\n", infile );
        success = 0;
        goto finalize;
    }

    outfp = fopen( outfile, "wb" );

    while ( remaining > 0 )
    {
        uint32_t read_size;

        read_size = MIN(blocksize, remaining);
        memset( filebuffer, 0, blocksize );
        memset( decbuffer, 0, blocksize );
        memset( iv, 0, AES_BLOCK_SZ );
        aes_setkey_dec( &aes_ctx, key, AES_KEY_BITLEN( keysize) );

        if ( ( success = fread( filebuffer, 1, read_size, infp ) ) != read_size )
        {
            printf( "* File '%s' read error\n", infile );
            printf( "expected = %d, got = %d\n", read_size, success );
            success = 0;
            goto finalize;
        }

        aes_crypt_cbc( &aes_ctx, AES_DECRYPT, read_size, iv, filebuffer, decbuffer );

        if ( fwrite( decbuffer, 1, read_size, outfp ) != read_size )
        {
            success = 0;
            printf( "* Failed to write output file '%s' - %s\n", outfile, strerror( errno ) );
            goto finalize;
        }

        remaining -= read_size;
    }
    finalize:

    if ( infp )
        fclose( infp );
    if ( outfp )
        fclose( outfp );
    if ( filebuffer )
        free( filebuffer );
    if ( decbuffer )
        free( decbuffer );
    if ( !success )
        remove( outfile );

    return success;
}

void usage( void )
{
    printf( "Usage: aes_cbc_128 <mode>\n"
        "where <mode> is one of:\n"
        "  test                                              Run the self test\n"
        "  enc <keyfile> <infile> <encryptedfile> <blocksz>  Encrypt the supplied file\n"
        "  dec <keyfile> <infile> <decryptedfile> <blocksz>  Decrypt the supplied file\n" );
    exit( 0 );
}

int main( int argc, const char **argv )
{
    int argn;
    const char *mode;
    uint32_t blocksize = 0;
    for ( argn = 1; argn < argc; argn++ )
    {
        const char *arg = argv[ argn ];
        if ( arg[ 0 ] != '-' )
            break;

        printf( "* Unknown option '%s'\n", arg );
        usage( );
    }

    if ( argn == argc )
        usage( );

    mode = argv[ argn++ ];

    if ( strcmp( mode, "test" ) == 0 )
    {
        return aes_cbc_128_test( );
    }
    else if ( strcmp( mode, "enc" ) == 0 )
    {
        if ( ( argn + 3 ) == argc )
        {
            blocksize = 0;
        }
        else if ( ( argn + 4 ) == argc )
        {
            blocksize = atoi( argv[ argn + 3 ] );
        }
        else
        {
            printf( "* Incorrect parameters for sign\n" );
            usage( );
        }

        if ( aes_cbc_128_enc( argv[ argn ], argv[ argn + 1 ], argv[ argn + 2 ], blocksize ) == 0 )
        {
            return -1;
        }
    }
    else if ( strcmp( mode, "dec" ) == 0 )
    {
        if ( ( argn + 3 ) == argc )
        {
            blocksize = 0;
        }
        else if ( ( argn + 4 ) == argc )
        {
            blocksize = atoi( argv[ argn + 3 ] );
        }
        else
        {
            printf( "* Incorrect parameters for sign\n" );
            usage( );
        }

        if ( aes_cbc_128_dec( argv[ argn ], argv[ argn + 1 ], argv[ argn + 2 ], blocksize ) == 0 )
        {
            return -1;
        }
    }
    else
    {
        printf( "* Unknown mode '%s'\n", mode );
        usage( );
    }

    return 0;
}
