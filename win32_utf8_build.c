/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Main compilation unit.
  */

#define PSAPI_VERSION 1
#define CINTERFACE
#define COBJMACROS

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "wininet.lib")

// Headers
#include "src/win32_utf8.h"
#include "src/wrappers.h"
#include <stddef.h>
#include <assert.h>

// Helper functions
#include "src/macros.c"
#include "src/utf.c"
#include "src/win32_utf8.c"
#include "src/wrappers.c"

// Wrappers
#include "src/comdlg32_dll.c"
#include "src/dsound_dll.c"
#include "src/gdi32_dll.c"
#include "src/kernel32_dll.c"
#include "src/msvcrt_dll.c"
#include "src/psapi_dll.c"
#include "src/shell32_dll.c"
#include "src/shlwapi_dll.c"
#include "src/user32_dll.c"
#include "src/version_dll.c"
#include "src/wininet_dll.c"
