/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Entry point wrapper, converting the command-line parameters to UTF-8.
  */

int win32_utf8_entry(main_t *user_main)
{
	int ret;
	int argc_w = 0;
	char **argv_u = CommandLineToArgvU(GetCommandLineW(), &argc_w);
	if(argv_u) {
		assert(user_main);

		win32_utf8_init();
		ret = user_main(argc_w, (const char**)argv_u);
		win32_utf8_exit();
		LocalFree(argv_u);
	} else {
		ret = GetLastError();
		fprintf(stderr, "win32_utf8: Error converting the command line!?\n");
	}
	return ret;
}
