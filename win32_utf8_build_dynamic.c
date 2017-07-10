/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Main compilation unit for dynamic builds.
  */

#include "src/build.c"

// Dynamic builds call these in DllMain().
#define win32_utf8_init()
#define win32_utf8_exit()

#include "src/entry.c"
