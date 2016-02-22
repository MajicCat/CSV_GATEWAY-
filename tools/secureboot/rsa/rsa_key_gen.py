#!/usr/bin/python
#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# Needs : openssl, ssh-keygen
# Generate rsa key pair and hash of rsa public exponent

import os, sys, base64, struct

try:
    total = len(sys.argv)

    if total < 1:
        print 'Usage: ' + sys.argv[0] + ' <rsa key name>'
        sys.exit()

    # generate RSA key with e=65537
    os.system('openssl genrsa -f4 -out ' + sys.argv[1] + ' 2048')
    # make key secure
    os.system('chmod 600 ' + sys.argv[1])
    # extract RSA public key
    os.system('openssl rsa -in ' + sys.argv[1] + ' -pubout -out ' + sys.argv[1] + '.pub.rsa')
    # make ssh-rsa key
    os.system('ssh-keygen -y -f ' + sys.argv[1] + ' > ' + sys.argv[1] + '.pub')

    # extract public modulus n
    keydata = base64.b64decode(open(sys.argv[1] + '.pub').read().split(None)[1])
    parts = []
    while keydata :
        # read the length of the data
        dlen = struct.unpack('>I', keydata[:4])[0]
        # read in <length> bytes
        data, keydata = keydata[4:dlen+4], keydata[4+dlen:]
        parts.append(data)

    if len(parts[2]) > 256 :
        parts[2] = parts[2][len(parts[2])-256:]

    open(sys.argv[1] + '.n', 'wb').write(parts[2])

    os.system('openssl dgst -sha256 -binary -out ' + sys.argv[1] + '.n.hash ' + sys.argv[1] + '.n')

except IOError:
    print ("Error!")
    sys.exit()
