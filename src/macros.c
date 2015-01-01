/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Macros.
  */

#include "macros.h"

size_t zzstrlen(const char *str)
{
	size_t len = 0;
	if(str) {
		for(; str[0] != 0 || str[1] != 0; str++, len++);
	}
	return len;
}
