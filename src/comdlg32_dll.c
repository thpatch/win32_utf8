/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * comdlg32.dll functions.
  */

const w32u8_pair_t comdlg32_pairs[] = {
	{"GetOpenFileNameA", GetOpenFileNameU},
	{"GetSaveFileNameA", GetSaveFileNameU},
	{ NULL }
};

/// Wrappers
/// --------
typedef BOOL WINAPI WrapOFNFunc_t(
	LPOPENFILENAMEW ofn_w
);

static BOOL WrapOFN(
	WrapOFNFunc_t *func,
	LPOPENFILENAMEA ofn_a
)
{
	if(!ofn_a) {
		return func(NULL);
	} else {
		BOOL ret;
		VLA(BYTE, ofn_w_raw, ofn_a->lStructSize);
		OPENFILENAMEW *ofn_w = (OPENFILENAMEW *)ofn_w_raw;

		size_t lpstrFilter_len = zzstrlen(ofn_a->lpstrFilter) + 2;
		size_t lpstrInitialDir_len = strlen(ofn_a->lpstrInitialDir) + 1;
		size_t lpstrTitle_len = strlen(ofn_a->lpstrTitle) + 1;
		size_t lpstrDefExt_len = strlen(ofn_a->lpstrDefExt) + 1;
		VLA(wchar_t, lpstrFilter_w, lpstrFilter_len);
		VLA(wchar_t, lpstrCustomFilter_w, ofn_a->nMaxCustFilter);
		VLA(wchar_t, lpstrFile_w, ofn_a->nMaxFile);
		VLA(wchar_t, lpstrFileTitle_w, ofn_a->nMaxFileTitle);
		VLA(wchar_t, lpstrInitialDir_w, lpstrInitialDir_len);
		VLA(wchar_t, lpstrTitle_w, lpstrTitle_len);
		VLA(wchar_t, lpstrDefExt_w, lpstrDefExt_len);
		RESID_DEC(lpTemplateName);

		memcpy(ofn_w, ofn_a, ofn_a->lStructSize);
		ofn_w->lpstrFilter = StringToUTF16_VLA(lpstrFilter_w, ofn_a->lpstrFilter, lpstrFilter_len);
		ofn_w->lpstrCustomFilter = StringToUTF16_VLA(lpstrCustomFilter_w, ofn_a->lpstrCustomFilter, ofn_a->nMaxCustFilter);
		ofn_w->lpstrFile = StringToUTF16_VLA(lpstrFile_w, ofn_a->lpstrFile, ofn_a->nMaxFile);
		ofn_w->lpstrFileTitle = StringToUTF16_VLA(lpstrFileTitle_w, ofn_a->lpstrFileTitle, ofn_a->nMaxFileTitle);
		ofn_w->lpstrInitialDir = StringToUTF16_VLA(lpstrInitialDir_w, ofn_a->lpstrInitialDir, lpstrInitialDir_len);
		ofn_w->lpstrTitle = StringToUTF16_VLA(lpstrTitle_w, ofn_a->lpstrTitle, lpstrTitle_len);
		ofn_w->lpstrDefExt = StringToUTF16_VLA(lpstrDefExt_w, ofn_a->lpstrDefExt, lpstrDefExt_len);
		RESID_CONV(lpTemplateName, ofn_a->lpTemplateName);
		ofn_w->lpTemplateName = lpTemplateName_w;

		ret = func(ofn_w);

		if(ofn_a->lpstrFile) {
			StringToUTF8(ofn_a->lpstrFile, ofn_w->lpstrFile, ofn_a->nMaxFile);
			ofn_a->nFileOffset = WideCharToMultiByte(
				CP_UTF8, 0, ofn_w->lpstrFile, ofn_w->nFileOffset, NULL, 0, NULL, NULL
			);
			ofn_a->nFileExtension = WideCharToMultiByte(
				CP_UTF8, 0, ofn_w->lpstrFile, ofn_w->nFileExtension, NULL, 0, NULL, NULL
			);
		}
		if(ofn_a->lpstrFileTitle) {
			StringToUTF8(ofn_a->lpstrFileTitle, ofn_w->lpstrFileTitle, ofn_a->nMaxFileTitle);
		}
		if(ofn_a->lpstrCustomFilter) {
			StringToUTF8(ofn_a->lpstrCustomFilter, ofn_w->lpstrCustomFilter, ofn_a->nMaxCustFilter);
		}

		RESID_FREE(lpTemplateName, ofn_a->lpTemplateName);
		VLA_FREE(lpstrDefExt_w);
		VLA_FREE(lpstrTitle_w);
		VLA_FREE(lpstrInitialDir_w);
		VLA_FREE(lpstrFileTitle_w);
		VLA_FREE(lpstrFile_w);
		VLA_FREE(lpstrCustomFilter_w);
		VLA_FREE(lpstrFilter_w);
		VLA_FREE(ofn_w_raw);
		return ret;
	}
}
/// --------

BOOL WINAPI GetOpenFileNameU(
	LPOPENFILENAMEA ofn_a
)
{
	return WrapOFN(GetOpenFileNameW, ofn_a);
}

BOOL WINAPI GetSaveFileNameU(
	LPOPENFILENAMEA ofn_a
)
{
	return WrapOFN(GetSaveFileNameW, ofn_a);
}
