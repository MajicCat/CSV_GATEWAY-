#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Secure_MQTT

$(NAME)_SOURCES := secure_mqtt.c \

$(NAME)_COMPONENTS := protocols/MQTT

$(NAME)_RESOURCES  := apps/secure_mqtt/secure_mqtt_root_cacert.cer