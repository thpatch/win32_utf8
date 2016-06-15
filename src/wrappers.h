/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Generic call wrappers to cut down redundancy.
  */

#pragma once

// Wrapper for functions that take only a single string parameter.
typedef size_t WINAPI Wrap1PFunc_t(
	LPCWSTR lpsz
);
size_t WINAPI Wrap1P(
	Wrap1PFunc_t *func,
	LPCSTR lpsz
);

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
