
#include "error.h"
#include <stdio.h>
#include <stdarg.h>

static void _printMessage(int num, const char *msg1, const char *msg2, va_list ap)
{
    fflush(stderr);
    fflush(stdout);
    if (num)
        fprintf(stderr, "\n%s - code: %d: ", msg1, num);
    else
        fprintf(stderr, "\n%s: ", msg1);
    vfprintf(stderr, msg2, ap);
    fprintf(stderr, "\n");
    fflush(stderr);
}

//****************************************************************************
// linux

#ifndef WIN32

extern "C" void PrintError(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap,msg);
    _printMessage(num, "Rally Raid 2012 error", msg, ap);
    exit(1);
}

extern "C" void PrintMessage(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    _printMessage(num, "Rally Raid 2012 message", msg, ap);
}

#endif

//****************************************************************************
// windows

#ifdef WIN32

// isn't cygwin annoying!
#ifdef CYGWIN
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#endif


#include "windows.h"
#include <irrlicht.h>


extern "C" void PrintError(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    char s[1000], title[100];
    _snprintf(title, sizeof(title), "Rally Raid 2012 - code: %d", num);
    _vsnprintf(s, sizeof(s), msg, ap);
    s[sizeof(s)-1] = 0;
    _printMessage(num, "Rally Raid 2012 error", msg, ap);
#ifdef _NODEF// _MSC_VER
    irr::core::stringw ws = L" ";
    irr::core::stringw wtitle = L" ";
    ws = s;
    wtitle = title;
    MessageBox(0, ws.c_str(), wtitle.c_str(), MB_OK | MB_ICONSTOP);
#else
    MessageBox(0, s, title, MB_OK | MB_ICONSTOP);
#endif
    exit(1);
}

extern "C" void PrintMessage(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    char s[1000], title[100];
    _snprintf(title, sizeof(title), "Rally Raid 2012 message - code: %d", num);
    _vsnprintf(s, sizeof(s), msg, ap);
    s[sizeof(s)-1] = 0;
    _printMessage(num, "Rally Raid 2012 message", msg, ap);
#ifdef NODEF //_MSC_VER
    irr::core::stringw ws = L" ";
    irr::core::stringw wtitle = L" ";
    ws = s;
    wtitle = title;
    MessageBox(0, ws.c_str(), wtitle.c_str(), MB_OK | MB_ICONWARNING);
#else
    MessageBox(0, s, title, MB_OK | MB_ICONWARNING);
#endif
//    printMessage (num,"ODE Message",msg,ap);
}

#endif // WIN32
