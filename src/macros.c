/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Random macros and utility functions.
  */

size_t zzstrlen(const char *str)
{
	size_t len = 0;
	if(str) {
		for(; str[0] != 0 || str[1] != 0; str++, len++);
	}
	return len;
}

// Returns whether the numeric specifier at [p] consumes an argument or not,
// and advances [p] past it.
static int printf_num_parse(const char **p)
{
	assert(p && *p);
	if(**p == '*') {
		// width specified in next variable argument
		(*p)++;
		return 1;
	} else while(isdigit(**p)) {
		(*p)++;
	}
	return 0;
}

// Fills [fmt] with information about the printf format specification
// starting at [p]. [p] is assumed to start after the initial % character,
// and is returned after the final character denoting the type.
// Based on Wine's implementation for msvcrt.dll (dlls/msvcrt/printf.h).
const char* printf_format_parse(printf_format_t *fmt, const char *p)
{
	int flag_double = 0;
	
	assert(fmt);
	assert(p && (*(p - 1) == '%' || *(p - 1) == '!'));

	memset(fmt, 0, sizeof(printf_format_t));

	if(!(*p)) {
		return p;
	}

	// Skip flags. From left to right:
	// prefix sign, prefix space, left-align, zero padding, alternate
	while(strchr("+ -0#", *p)) {
		p++;
	}

	// Width
	fmt->argc_before_type += printf_num_parse(&p);

	// Precision
	if(*p == '.') {
		p++;
		fmt->argc_before_type += printf_num_parse(&p);
	}

	// Argument size modifier
	while(*p) {
		if(*p=='l' && *(p+1)=='l') {
			flag_double = 1;
			p += 2;
		} else if(*p=='h' || *p=='l' || *p=='L') {
			p++;
		} else if(*p == 'I') {
			if(*(p+1)=='6' && *(p+2)=='4') {
				flag_double = 1;
				p += 3;
			} else if(*(p+1)=='3' && *(p+2)=='2') {
				p += 3;
			} else if(isdigit(*(p+1)) || !*(p+1)) {
				break;
			} else {
				p++;
			}
		} else if(*p == 'w' || *p == 'F') {
			p++;
		} else {
			break;
		}
	}
	fmt->type = *(p++);
	if(strchr("aeEfgG", fmt->type)) {
		// Two surprises here:
		// • Yes, floats are always coerced to doubles.
		// • Yes, flag_double is ignored here because Microsoft has no support
		//   for printing long doubles in either of their printf implementations.
		fmt->type_size_in_ints = sizeof(double) / sizeof(int);
	} else if(strchr("sScCpndiouxX", fmt->type)) {
		fmt->type_size_in_ints = flag_double + 1;
	}
	return p;
}

const char* windows_version(void)
{
	static char version[64];
	if(version[0] != '\0') {
		return version;
	}

	char *p = version;
	size_t rem = sizeof(version) / sizeof(version[0]);

#define snprintf_cat(fmt, ...) \
	if(rem > 0) { \
		int chars_printed = wnsprintfA(p, rem, fmt, __VA_ARGS__); \
		if(chars_printed < 0) { \
			chars_printed = rem; \
		} \
		rem -= chars_printed; \
		p += chars_printed; \
	}

	// Don't need to depend on the entire Driver Development Kit just for
	// ntddk.h.
	typedef struct _OSVERSIONINFOW {
		ULONG dwOSVersionInfoSize;
		ULONG dwMajorVersion;
		ULONG dwMinorVersion;
		ULONG dwBuildNumber;
		ULONG dwPlatformId;
		WCHAR szCSDVersion[128];
		USHORT wServicePackMajor;
		USHORT wServicePackMinor;
		USHORT wSuiteMask;
		UCHAR wProductType;
		UCHAR wReserved;
	} RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;

	// Or ntoskrnl.lib.
	typedef LONG WINAPI RtlGetVersion_type(RTL_OSVERSIONINFOEXW *lpVersionInformation);
	typedef const char *wine_get_version_type(void);

	RtlGetVersion_type* RtlGetVersion;
	wine_get_version_type *wine_get_version;

	HMODULE hNTDLL = GetModuleHandleA("ntdll.dll");
	if(!hNTDLL) {
		snprintf_cat("%s", "unknown operating system");
		return version;
	}

	// Wine
	// ----
	wine_get_version = (wine_get_version_type*)GetProcAddress(hNTDLL, "wine_get_version");
	if(wine_get_version) {
		const char* wine_ver = wine_get_version();
		if(!wine_ver) {
			wine_ver = "<unknown>";
		}
		snprintf_cat("Wine/%s", wine_ver);
		return version;
	}
	// ----

	RtlGetVersion = (RtlGetVersion_type*)GetProcAddress(hNTDLL, "RtlGetVersion");
	RTL_OSVERSIONINFOEXW ver_info = { sizeof(ver_info) };
	RtlGetVersion(&ver_info);

	const char *winver = NULL;
	ULONG major = ver_info.dwMajorVersion;
	ULONG minor = ver_info.dwMinorVersion;
	UCHAR product = ver_info.wProductType;
	USHORT suite = ver_info.wSuiteMask;

	// As per https://msdn.microsoft.com/en-us/library/windows/hardware/ff563620(v=vs.85).aspx
	if(major == 10) {
		winver = "10";
	} else if(major == 6 && minor == 3) {
		winver = "8.1";
	} else if(major == 6 && minor == 2 && product == VER_NT_WORKSTATION) {
		winver = "8";
	} else if(major == 6 && minor == 2 && product != VER_NT_WORKSTATION) {
		winver = "Server 2012";
	} else if(major == 6 && minor == 1 && product == VER_NT_WORKSTATION) {
		winver = "7";
	} else if(major == 6 && minor == 1 && product != VER_NT_WORKSTATION) {
		winver = "Server 2008 R2";
	} else if(major == 6 && minor == 0 && product == VER_NT_WORKSTATION) {
		winver = "Vista";
	} else if(major == 6 && minor == 0 && product != VER_NT_WORKSTATION) {
		winver = "Server 2008";
	} else if(major == 5 && minor == 2 && suite == VER_SUITE_WH_SERVER) {
		winver = "Home Server";
	} else if(major == 5 && minor == 2) {
		winver = "Server 2003";
	} else if(major == 5 && minor == 1) {
		winver = "XP";
	} else if(major == 5 && minor == 0) {
		winver = "2000";
	}

	if(winver) {
		snprintf_cat("Windows %s", winver);
	} else {
		snprintf_cat("Windows %u.%u", major, minor);
	}

	// szCSDVersion can be localized, see
	// https://channel9.msdn.com/forums/Coffeehouse/542106-The-Service-Pack-string/
	// So...
	if(ver_info.wServicePackMajor != 0) {
		snprintf_cat(", Service Pack %hu", ver_info.wServicePackMajor);
		if(ver_info.wServicePackMinor != 0) {
			snprintf_cat(".%hu", ver_info.wServicePackMinor);
		}
	}

	// If Windows 10 really will be the "last Windows", we better add this too.
	if(ver_info.dwBuildNumber != 0) {
		snprintf_cat(", Build %u", ver_info.dwBuildNumber);
	}
#undef snprintf_cat
	return version;
}
