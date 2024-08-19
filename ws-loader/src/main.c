#include <windows.h>
#include <stdio.h>

#include "../inc/ws.h"

int main() {

	if (!ws_load(HOME, PORT)) {
	
		printf("[!] ws-load failed.\n");
		return -1;
	
	}

	printf("[+] ws-load completed.\n");

	return 0;
}
