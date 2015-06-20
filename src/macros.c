/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Random macros and utility functions.
  */

#include "win32_utf8.h"
#include <assert.h>

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