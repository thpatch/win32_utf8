/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Main include file.
  */

#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <psapi.h>
#include <ShlObj.h>
#include <stddef.h>
#include <stdio.h>
#include <WinInet.h>

#include "macros.h"
#include "utf.h"

#include "gdi32_dll.h"
#include "kernel32_dll.h"
#include "msvcrt_dll.h"
#include "psapi_dll.h"
#include "shell32_dll.h"
#include "shlwapi_dll.h"
#include "user32_dll.h"
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
const w32u8_dll_t* w32u8_get_wrapped_functions();

// Sets a custom codepage for wide char conversion, which is used if the input
// to a *U function is not valid UTF-8.
// Useful for console applications (which use CP_OEMCP by default) or patching
// applications where the application's native codepage isn't ASCII.
void w32u8_set_fallback_codepage(UINT codepage);
