#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# Makefile for gcc (or cygwin gcc)

CC = mingw32-gcc.exe
RM = del
# Uncomment below to compile in Linux
# CC = gcc
# RM = rm -f

CFLAGS = -g -DDEBUG

SRCBASE = ../../../

VPATH = $(SRCBASE)/WICED/security/BESL/crypto_open

INC = -I$(SRCBASE)/WICED/security/BESL/crypto_open/
INC += -I$(SRCBASE)/WICED/security/BESL/crypto/

TARGET = aes_cbc_128

OBJS = aes_cbc_128.o aes.o

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

.c.o :
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) $(TARGET) *.o *.exe core
