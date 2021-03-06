/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Initialization stubs.
  */

#ifndef WIN32_UTF8_NO_API
const w32u8_dll_t* w32u8_get_wrapped_functions()
{
	// Yes, this is the prettiest way I came up with.
	extern const w32u8_pair_t comdlg32_pairs[];
	extern const w32u8_pair_t dsound_pairs[];
	extern const w32u8_pair_t gdi32_pairs[];
	extern const w32u8_pair_t kernel32_pairs[];
	extern const w32u8_pair_t msvcrt_pairs[];
	extern const w32u8_pair_t psapi_pairs[];
	extern const w32u8_pair_t shell32_pairs[];
	extern const w32u8_pair_t shlwapi_pairs[];
	extern const w32u8_pair_t user32_pairs[];
	extern const w32u8_pair_t version_pairs[];
	extern const w32u8_pair_t wininet_pairs[];

	static const w32u8_dll_t dlls[] = {
		{"comdlg32.dll", comdlg32_pairs},
		{"dsound.dll", dsound_pairs},
		{"gdi32.dll", gdi32_pairs},
		{"kernel32.dll", kernel32_pairs},
		{"msvcrt.dll", msvcrt_pairs},
		{"psapi.dll", psapi_pairs},
		{"shell32.dll", shell32_pairs},
		{"shlwapi.dll", shlwapi_pairs},
		{"user32.dll", user32_pairs},
		{"version.dll", version_pairs},
		{"wininet.dll", wininet_pairs},
		{ NULL }
	};
	return dlls;
}
#endif

UINT fallback_codepage = CP_ACP;

void w32u8_set_fallback_codepage(UINT codepage)
{
	fallback_codepage = codepage;
}

void win32_utf8_init(void)
{
	version_init();
}

void win32_utf8_exit(void)
{
	kernel32_exit();
}

// Yes, this _has_ to be included in every project.
// Visual C++ won't use it when imported from a library
// and just defaults to msvcrt's one in this case.
BOOL APIENTRY DllMain(HMODULE hDll, DWORD ulReasonForCall, LPVOID lpReserved)
{
	(void)hDll;
	(void)lpReserved;

	switch(ulReasonForCall) {
		case DLL_PROCESS_ATTACH:
			win32_utf8_init();
			break;
		case DLL_PROCESS_DETACH:
			win32_utf8_exit();
			break;
	}
	return TRUE;
}
