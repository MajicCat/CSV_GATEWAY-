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
# Input args  : sign, rsa_priave_key, inputfile, outputfile
# Assumptions : openssl is installed and path to openssl is in Windows PATH environment variable
#             : rsa_private_key.n is present in the RSA keys folder
# Calculate the RSA Signature of the inputfile
# outputfile = inputfile + RSA signature + RSA public key

my ($sign, $rsa_private_key, $infile, $outfile) = @ARGV;

# For Debugging
# foreach $key (keys(%ENV)) {
# printf("%-10.10s: $ENV{$key}\n", $key);
# }

local $ENV{PATH} = "$ENV{SAVED_PATH}";

#Open infile to read
open INFILE, '<', $infile or die $!;
binmode INFILE;

my $tmp_file = 'img_temp';
open TMPFILE, '>', $tmp_file or die $!;
binmode TMPFILE;

#Copy input file to img_temp
while(<INFILE>)
{
    print TMPFILE $_;
}

close TMPFILE;

my $filesize = -s  "img_temp";
# Pad tmp_file to make its size % 16
$residue = (16 - ($filesize % 16)) & 0xf;
open TMPFILE, '>>', $tmp_file or die $!;
binmode TMPFILE;

while($residue > 0)
{
    print TMPFILE "\0";
    $residue -= 1;
}

close TMPFILE;
close INFILE;
#Calculate sha256 hash of input and sign the hash using rsa_private_key
system("openssl dgst -sha256 -sign $rsa_private_key -out sig_tmp  $tmp_file");

#Outfile = tmp_file + RSA signature + RSA public key
open TMPFILE, '<', $tmp_file or die $!;
binmode TMPFILE;
open OUTFILE, '>', $outfile or die $!;
binmode OUTFILE;
while(<TMPFILE>)
{
    print OUTFILE $_;
}
close TMPFILE;

open SIGFILE, '<', "sig_tmp" or die $!;
binmode SIGFILE;
while(<SIGFILE>)
{
    print OUTFILE $_;
}

open PUB_KEY_HASH, "<", "$rsa_private_key.n" or die $!;
binmode PUB_KEY_HASH;
while(<PUB_KEY_HASH>)
{
    print OUTFILE $_;
}

close PUB_KEY_HASH;
close SIGFILE;
close OUTFILE;
unlink sig_tmp;
unlink img_temp;
