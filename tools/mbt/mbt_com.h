/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include <Windows.h>
#include <string>
using namespace std;

//**************************************************************************************************
//*** Definitions for BTW Serial Bus
//**************************************************************************************************

// Helper class to print debug messages to Debug Console
class DebugHelper
{
public:
    void DebugOut() { OutputDebugStringA(m_Buffer); }
    void DebugOut(LPCSTR v) { OutputDebugStringA(v); }
    void DebugOut(LPCSTR fmt, LPCSTR v);
    void DebugOut(LPCSTR fmt, DWORD v1);
    void DebugOut(LPCSTR fmt, DWORD v1, DWORD v2);
    void PrintCommProp(COMMPROP& commProp);
    void PrintCommState(DCB& serial_config);

    static char m_Buffer[1024];
};

//
// Serial Bus class, use this class to read/write from/to the serial bus device
//
class ComHelper
{
public:
    ComHelper();
    virtual ~ComHelper();

    // oopen serialbus driver to access device
    BOOL OpenPort(char* argv);

    // read data from device
    DWORD Read(LPBYTE b, DWORD dwLen);

    // write data to device
    DWORD Write(LPBYTE b, DWORD dwLen);
private:

    // overlap IO for Read and Write
    OVERLAPPED m_OverlapRead;
    OVERLAPPED m_OverlapWrite;
    HANDLE m_handle;
};

