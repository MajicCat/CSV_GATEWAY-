#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_HTTPS_Server

$(NAME)_SOURCES    := https_server.c

$(NAME)_COMPONENTS := daemons/HTTP_server

$(NAME)_RESOURCES  := apps/https_server/https_server_top.html \
                      images/brcmlogo.png \
                      images/brcmlogo_line.png \
                      images/favicon.ico


CERTIFICATE := $(SOURCE_ROOT)resources/certificates/brcm_demo_server_cert.cer
PRIVATE_KEY := $(SOURCE_ROOT)resources/certificates/brcm_demo_server_cert_key.key