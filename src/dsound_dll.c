/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * dsound.dll functions.
  */

const w32u8_pair_t dsound_pairs[] = {
	{"DirectSoundCaptureEnumerateA", DirectSoundCaptureEnumerateU},
	{"DirectSoundEnumerateA", DirectSoundEnumerateU},
	NULL
};

typedef struct {
	LPDSENUMCALLBACKA lpOrigProc;
	LPVOID lOrigParam;
} DSEnumParam;

BOOL CALLBACK DSEnumCallbackWrap(
	LPGUID lpGuid,
	LPCWSTR lpcstrDescription,
	LPCWSTR lpcstrModule,
	DSEnumParam *wrap_param
)
{
	BOOL ret;
	UTF8_DEC(lpcstrDescription);
	UTF8_DEC(lpcstrModule);
	UTF8_CONV(lpcstrDescription);
	UTF8_CONV(lpcstrModule);
	ret = wrap_param->lpOrigProc(
		lpGuid, lpcstrDescription_utf8, lpcstrModule_utf8, wrap_param->lOrigParam
	);
	UTF8_FREE(lpcstrDescription);
	UTF8_FREE(lpcstrModule);
	return ret;
}

HRESULT WINAPI DirectSoundCaptureEnumerateU(
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
)
{
	DSEnumParam wrap_param = {pDSEnumCallback, pContext};
	return DirectSoundCaptureEnumerateW(
		(LPDSENUMCALLBACKW)DSEnumCallbackWrap, &wrap_param
	);
}

HRESULT WINAPI DirectSoundEnumerateU(
	LPDSENUMCALLBACKA pDSEnumCallback,
	LPVOID pContext
)
{
	DSEnumParam wrap_param = {pDSEnumCallback, pContext};
	return DirectSoundEnumerateW(
		(LPDSENUMCALLBACKW)DSEnumCallbackWrap, &wrap_param
	);
}
