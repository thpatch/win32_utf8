/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Generic call wrappers to cut down redundancy.
  */

#pragma once


// Wrapper for functions that write a string into a buffer, and return its
// necessary size when passing 0 for [nBufferLength].
typedef DWORD WINAPI WrapGetStringFunc_t(
	DWORD nBufferLength,
	LPWSTR lpBuffer
);
DWORD WINAPI WrapGetString(
	WrapGetStringFunc_t *func,
	DWORD nBufferLength,
	LPSTR lpBuffer
);
