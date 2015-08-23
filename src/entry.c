/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Wrappers for certain entry points.
  */

#ifndef WIN32_UTF8_MAIN_UNIT
#include "win32_utf8.h"
#endif

#define main win32_utf8_main

#undef main

// This reference is also why we can't just compile this file for dynamic
// builds as well, as those can't possibly resolve it.
int __cdecl win32_utf8_main(int argc, const char *argv[]);

int __cdecl main(int argc, const char *argv[])
{
	int ret;
	int argc_w = 0;
	char **argv_u = CommandLineToArgvU(GetCommandLineW(), &argc_w);
	if(argv_u) {
		ret = win32_utf8_main(argc_w, (const char**)argv_u);
		LocalFree(argv_u);
	} else {
		ret = GetLastError();
		fprintf(stderr, "win32_utf8: Error converting the command line!?\n");
	}
	return ret;
}
