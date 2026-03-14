#pragma once

#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <tchar.h>
#include <thumbcache.h>
#include <windows.h>

STDAPI_(ULONG) DllAddRef();
STDAPI_(ULONG) DllRelease();
STDAPI_(HINSTANCE) DllInstance();

// {4BBBEAB5-BE00-41f4-A209-FE838660B9B1}
#define szCLSID_SampleThumbnailProvider L"{4BBBEAB5-BE00-41f4-A209-FE838660B9B1}"
DEFINE_GUID(
    CLSID_SampleThumbnailProvider,
    0x4bbbeab5,
    0xbe00,
    0x41f4,
    0xa2,
    0x9,
    0xfe,
    0x83,
    0x86,
    0x60,
    0xb9,
    0xb1
);
