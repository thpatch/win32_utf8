/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * dsound.dll functions.
  */

#pragma once

WRAPPER_DEC(HRESULT WINAPI, DirectSoundCaptureEnumerate,
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
);
#undef DirectSoundCaptureEnumerate
#define DirectSoundCaptureEnumerate DirectSoundCaptureEnumerateU

WRAPPER_DEC(HRESULT WINAPI, DirectSoundEnumerate,
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
);
#undef DirectSoundEnumerate
#define DirectSoundEnumerate DirectSoundEnumerateU
