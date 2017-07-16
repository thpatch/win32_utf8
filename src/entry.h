/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Entry point wrapper, converting the command-line parameters to UTF-8.
  */

// The entry points themselves only need this header, and could be #included
// from C++ source files, so we need to do this here again.
#ifdef __cplusplus
extern "C" {
#endif

// User-defined, subsystem-independent main function. Compile or #include one
// of the entry_*.c files from the top directory, depending on which entry
// point you want to use.
int __cdecl win32_utf8_main(int argc, const char *argv[]);
#define main win32_utf8_main

typedef int __cdecl main_t(int argc, const char *argv[]);

// Performs the conversion and calls [user_main] with an UTF-8 argv.
int win32_utf8_entry(main_t *user_main);

#ifdef __cplusplus
}
#endif
