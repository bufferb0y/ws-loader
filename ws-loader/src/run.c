#include "../inc/run.h"

BOOL execute(LPVOID shellcode, size_t size) {

	// https://github.com/aahmad097/AlternativeShellcodeExec/blob/master/EnumPwrSchemes/EnumPwrSchemes.cpp

	if (!EnumPwrSchemes((PWRSCHEMESENUMPROC)shellcode, 0)) {
		return FALSE;
	}

	return TRUE;
}
