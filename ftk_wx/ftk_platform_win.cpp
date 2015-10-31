
#include <windows.h>
#include <tchar.h>

#include "ftk_resource.h"

int FTK_Platform_Init()
{
	// Make our custom Inconsolata.otf font available to load. In Windows
	// we can pull the binary .otf file out of a resource, map it and 
	// add it to the system where only our process can "see" it.
	HRSRC hrc = FindResource(0, MAKEINTRESOURCE(IDR_PROGFONT), (RT_FONT));
	if (!hrc)
		return -1;

	HGLOBAL fhandle = LoadResource(0, hrc);
	if (!fhandle)
		return -2;


	DWORD font_size = SizeofResource(0, hrc);
	VOID* font_data = LockResource(fhandle);
	if (!font_data)
		return -3;

	DWORD  num_installed = 0;
	HANDLE h_font = AddFontMemResourceEx(font_data, font_size, 0, &num_installed);
	if (!num_installed)
		return -4;

	// TODO : we are leaking these handles right now

	return 0;
}
