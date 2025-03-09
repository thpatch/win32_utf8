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
	if (ofn_a) {
		size_t lpstrFilter_len = ofn_a->lpstrFilter ? zzstrlen(ofn_a->lpstrFilter) + 2 : 0;
		size_t lpstrInitialDir_len = ofn_a->lpstrInitialDir ? strlen(ofn_a->lpstrInitialDir) + 1 : 0;
		size_t lpstrTitle_len = ofn_a->lpstrTitle ? strlen(ofn_a->lpstrTitle) + 1 : 0;
		size_t lpstrDefExt_len = ofn_a->lpstrDefExt ? strlen(ofn_a->lpstrDefExt) + 1 : 0;
		size_t lpTemplateName_len = RESID_IS_STR(ofn_a->lpTemplateName) ? strlen(ofn_a->lpTemplateName) + 1 : 0;
		size_t lpstrCustomFilter_len = ofn_a->lpstrCustomFilter ? ofn_a->nMaxCustFilter : 0;
		size_t lpstrFileTitle_len = ofn_a->lpstrFileTitle ? ofn_a->nMaxFileTitle : 0;

		size_t total_len = lpstrFilter_len + lpstrCustomFilter_len + ofn_a->nMaxFile + lpstrFileTitle_len + lpstrInitialDir_len + lpstrTitle_len + lpstrDefExt_len + lpTemplateName_len;
		VLA(wchar_t, param_buffers, total_len);
		wchar_t* param_buffer_write = param_buffers;

		VLA(BYTE, ofn_w_raw, ofn_a->lStructSize);
		OPENFILENAMEW* ofn_w = (OPENFILENAMEW*)ofn_w_raw;
		memcpy(ofn_w, ofn_a, ofn_a->lStructSize);

		if (ofn_a->lpstrFilter) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrFilter, lpstrFilter_len);
			ofn_w->lpstrFilter = param_buffer_write;
			param_buffer_write += written;
		}
		if (ofn_a->lpstrCustomFilter) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrCustomFilter, ofn_a->nMaxCustFilter);
			ofn_w->lpstrCustomFilter = param_buffer_write;
			param_buffer_write += written;
		}
		if (ofn_a->lpstrFileTitle) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrFileTitle, ofn_a->nMaxFileTitle);
			ofn_w->lpstrFileTitle = param_buffer_write;
			param_buffer_write += written;
		}
		if (ofn_a->lpstrInitialDir) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrInitialDir, lpstrInitialDir_len);
			ofn_w->lpstrInitialDir = param_buffer_write;
			param_buffer_write += written;
		}
		if (ofn_a->lpstrTitle) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrTitle, lpstrTitle_len);
			ofn_w->lpstrTitle = param_buffer_write;
			param_buffer_write += written;
		}
		if (ofn_a->lpstrDefExt) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpstrDefExt, lpstrDefExt_len);
			ofn_w->lpstrDefExt = param_buffer_write;
			param_buffer_write += written;
		}
		if (RESID_IS_STR(ofn_a->lpTemplateName)) {
			size_t written = StringToUTF16(param_buffer_write, ofn_a->lpTemplateName, lpTemplateName_len);
			ofn_w->lpstrDefExt = param_buffer_write;
			param_buffer_write += written;
		}

		StringToUTF16(param_buffer_write, ofn_a->lpstrFile, ofn_a->nMaxFile);
		ofn_w->lpstrFile = param_buffer_write;

		BOOL ret = func(ofn_w);

		if (ret) {
			StringToUTF8(ofn_a->lpstrFile, ofn_w->lpstrFile, ofn_a->nMaxFile);
			ofn_a->nFileOffset = WideCharToMultiByte(
				CP_UTF8, 0, ofn_w->lpstrFile, ofn_w->nFileOffset, NULL, 0, NULL, NULL
			);
			ofn_a->nFileExtension = ofn_w->nFileExtension ? WideCharToMultiByte(
				CP_UTF8, 0, ofn_w->lpstrFile, ofn_w->nFileExtension, NULL, 0, NULL, NULL
			) : 0;
		} else {
			*(WORD*)ofn_a->lpstrFile = *(WORD*)ofn_w->lpstrFile;
		}
		if (ofn_a->lpstrCustomFilter) {
			StringToUTF8(ofn_a->lpstrCustomFilter, ofn_w->lpstrCustomFilter, ofn_a->nMaxCustFilter);
		}
		if (ofn_a->lpstrFileTitle) {
			StringToUTF8(ofn_a->lpstrFileTitle, ofn_w->lpstrFileTitle, ofn_a->nMaxFileTitle);
		}

		VLA_FREE(ofn_w_raw);
		VLA_FREE(param_buffers);
		return ret;
	}
	return func(NULL);
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
