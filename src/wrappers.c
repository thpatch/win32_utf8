/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Generic call wrappers to cut down redundancy.
  */

DWORD WINAPI WrapGetString(
	WrapGetStringFunc_t *func,
	DWORD nBufferLength,
	LPSTR lpBuffer
)
{
	DWORD ret;
	VLA(wchar_t, lpBuffer_w, nBufferLength);

	if(!lpBuffer) {
		VLA_FREE(lpBuffer_w);
	}
	ret = func(nBufferLength, lpBuffer_w);
	if(lpBuffer) {
		StringToUTF8(lpBuffer, lpBuffer_w, nBufferLength);
	} else {
		// Hey, let's be nice and return the _actual_ length.
		VLA(wchar_t, lpBufferReal_w, ret);
		func(ret, lpBufferReal_w);
		ret = StringToUTF8(NULL, lpBufferReal_w, 0) + 1;
		VLA_FREE(lpBufferReal_w);
	}
	VLA_FREE(lpBuffer_w);
	return ret;
}
