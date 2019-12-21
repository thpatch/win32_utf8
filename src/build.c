/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Code shared between static and dynamic builds.
  */

#pragma warning(error: 4028)

// We're targeting older C runtime versions, too.
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#define PSAPI_VERSION 1
#define CINTERFACE
#define COBJMACROS

#define WIN32_UTF8_MAIN_UNIT

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "wininet.lib")

// Headers
#include "../win32_utf8.h"
#include "wrappers.h"
#include <stddef.h>

// Helper functions
#include "macros.c"
#include "utf.c"
#include "win32_utf8.c"
#include "wrappers.c"

// Wrappers
#include "comdlg32_dll.c"
#include "dsound_dll.c"
#include "gdi32_dll.c"
#include "kernel32_dll.c"
#include "msvcrt_dll.c"
#include "psapi_dll.c"
#include "shell32_dll.c"
#include "shlwapi_dll.c"
#include "user32_dll.c"
#include "version_dll.c"
#include "wininet_dll.c"
