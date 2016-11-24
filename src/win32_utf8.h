/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Main include file.
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Windows.h>
#include <CommDlg.h>
#include <MMSystem.h>
#include <dsound.h>
#include <psapi.h>
#include <ShellAPI.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <stdio.h>
#include <WinInet.h>

#include "entry.h"
#include "macros.h"
#include "message_enum.h"
#include "utf.h"

#include "comdlg32_dll.h"
#include "dsound_dll.h"
#include "gdi32_dll.h"
#include "kernel32_dll.h"
#include "msvcrt_dll.h"
#include "psapi_dll.h"
#include "shell32_dll.h"
#include "shlwapi_dll.h"
#include "user32_dll.h"
#include "version_dll.h"
#include "wininet_dll.h"

typedef struct {
	// Name of the original ANSI function (e.g. "CreateFileA")
	const char *ansi_name;
	// Pointer to our UTF-8 version (e.g. CreateFileU)
	const void *utf8_ptr;
} w32u8_pair_t;

typedef struct {
	// DLL name (e.g. "kernel32.dll")
	const char *name;
	// List of functions we wrap in this DLL
	const w32u8_pair_t *funcs;
} w32u8_dll_t;

// Returns a complete list of function wrappers provided by win32_utf8,
// categorized by the DLL of the original function.
// Both the function pair lists per DLL and the DLL list itself are terminated
// by a zero-filled entry.
#ifndef WIN32_UTF8_NO_API
const w32u8_dll_t* w32u8_get_wrapped_functions();
#else
#define w32u8_get_wrapped_functions ERROR_win32_utf8_was_configured_without_API!
#endif

// Sets a custom codepage for wide char conversion, which is used if the input
// to a *U function is not valid UTF-8.
// Useful for console applications (which use CP_OEMCP by default) or patching
// applications where the application's native codepage isn't ASCII.
void w32u8_set_fallback_codepage(UINT codepage);

#ifdef __cplusplus
}
#endif
