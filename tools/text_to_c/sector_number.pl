#!/usr/bin/perl

#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
#use File:stat;

if (! $ARGV == 2)
{
    print "Usage ./sector_number.pl  Current_Sector BLOCK_SIZE";
    exit;
}

my $numerator = $ARGV[0];
my $denominator = $ARGV[1];

if (( index($ARGV[0], 'x') != -1 ) || (index($ARGV[0], 'X') != -1 ))
{
    $numerator = hex($ARGV[0]);
}
if (( index($ARGV[1], 'x') != -1 ) || (index($ARGV[1], 'X') != -1 ))
{
    $denominator = hex($ARGV[1]);
}

my $SECTOR = $numerator / $denominator;

print ("$SECTOR\r\n");