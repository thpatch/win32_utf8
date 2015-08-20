Win32 UTF-8 wrapper
-------------------

### Why a wrapper? ###

This library evolved from the need of the [Touhou Community Reliant Automatic Patcher] (https://github.com/thpatch/thcrap) to hack Unicode functionality for the Win32 API into games using the ANSI functions.

By simply including `win32_utf8.h` and linking to this library, you automatically have Unicode compatibility in applications using the native Win32 APIs, usually without requiring changes to existing code using char strings.

### Extended functionality ###

In addition, this library also adds new useful functionality to some original Windows functions.

###### kernel32.dll ######

* `CreateDirectoryU()` works recursively - the function creates all necessary directories to form the given path.
* `GetModuleFileNameU()` returns the necessary length of a buffer to hold the module file name if NULL is passed for `nSize` or `lpFilename`, similar to what `GetCurrentDirectory()` can do by default.

###### shell32.dll ######

* `SHBrowseForFolderU()` always displays an edit box and shows a resizable window if the active thread's COM settings allow it.

###### shlwapi.dll ######

* `PathRemoveFileSpecU()` correctly works as intended for paths containing forward slashes

### OS compatibility
win32_utf8 it meant to require at least Windows XP - that is, it statically references only Windows functions that were available on XP. Wrappers for functions that were introduced in later Windows versions load their original functions dynamically using GetProcAddress().

As a result, these wrappers themselves are not tied to the minimum required OS of the function they wrap. This means that applications which call these wrappers will actually *start* on Windows XP and *not* abort with the classic *"The procedure entry point X could not be located in the dynamic link library Y.DLL."* error on startup. (Unless, of course, if your compiler would target a newer Windows version anyway.) However, the wrappers will show this message box on every call:

![OS compatibility message box](os_compat.png)

This is arguably preferable over the three other options for dealing with this issue (just crashing by calling a NULL pointer, silently doing nothing, or aborting on startup with the aforementioned error). It should still run the majority of the program, and it provides a helpful message during testing - which can even be left in shipping builds as a sort of nag screen for end users that still use old Windows versions.

The following functions are wrapped in this way:

 * version.dll
	 * GetFileVersionInfoSizeEx()
	 * GetFileVersionInfoEx()

### Building ###
For static linking into your existing native Win32 code, simply replace all inclusions of `windows.h` with `win32_utf8.h`, and make sure that `win32_utf8_build.c` is compiled as part of your sources.

For dynamic linking or other more special use cases, a project file for Visual C++ is provided. The default configuration requires the Visual Studio 2013 platform toolset with Windows XP targeting support, but the project should generally build under every version since Visual C++ 2010 Express after changing the platform toolset (*Project → Properties → General → Platform Toolset*) to a supported option.

#### Compiler support ####
**Visual C++** and **MinGW** compile the code just fine. Cygwin is not supported, as [it lacks Unicode versions of certain C runtime functions because they're not part of the POSIX standard](https://www.cygwin.com/ml/cygwin/2006-03/msg00539.html). (Of course, using MinGW's `gcc` through Cygwin works just fine.)
