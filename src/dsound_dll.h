/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * dsound.dll functions.
  */

#pragma once

HRESULT WINAPI DirectSoundCaptureEnumerateU(
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
);
#undef DirectSoundCaptureEnumerate
#define DirectSoundCaptureEnumerate DirectSoundCaptureEnumerateU

HRESULT WINAPI DirectSoundEnumerateU(
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
);
#undef DirectSoundEnumerate
#define DirectSoundEnumerate DirectSoundEnumerateU
