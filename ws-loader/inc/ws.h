#ifndef __WS_H
#define __WS_H

#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include <stdint.h>

#define HOME L"192.168.153.133"
#define PORT 50000

// Uncomment the line below for squid proxy
//#define PROXY

HINTERNET get_websocket();

BOOL ws_load(const wchar_t *home, int port);

#endif
