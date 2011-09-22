
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
    _printMessage(num, "Rally Raid error", msg, ap);
    exit(1);
}

extern "C" void PrintMessage(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    _printMessage(num, "Rally Raid message", msg, ap);
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
#include "TheGame.h"
#include "MenuManager.h"

class ModalEventReceiver : public irr::IEventReceiver
{
public:
    ModalEventReceiver()
        : closed(false)
    {
    }

    virtual ~ModalEventReceiver()
    {
    }

private:
    virtual bool OnEvent(const irr::SEvent &event)
    {
        //printf("empty event: %u (%u)\n", event.EventType, event.GUIEvent.EventType);
        if (event.EventType == irr::EET_GUI_EVENT && event.GUIEvent.EventType == irr::gui::EGET_MESSAGEBOX_OK)
        {
            closed = true;
        }
        return false;
    }

public:
    volatile bool closed;
};

static bool printToWindow = true;

/* extern "C" */ void switchPrintDestination()
{
    printToWindow = false;
}


/* extern "C" */ void PrintError(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    char s[1000], title[100];
    _snprintf_s(title, sizeof(title), "Rally Raid - code: %d", num);
    _vsnprintf_s(s, sizeof(s), msg, ap);
    s[sizeof(s)-1] = 0;
    _printMessage(num, "Rally Raid error", msg, ap);
    if (printToWindow)
    {
#ifdef _NODEF// _MSC_VER
        irr::core::stringw ws = L" ";
        irr::core::stringw wtitle = L" ";
        ws = s;
        wtitle = title;
        MessageBox(0, ws.c_str(), wtitle.c_str(), MB_OK | MB_ICONSTOP);
#else
        MessageBox(0, s, title, MB_OK | MB_ICONSTOP);
#endif
    }
    else
    {
        irr::core::stringw ws = L" ";
        irr::core::stringw wtitle = L" ";
        ws = s;
        wtitle = title;
        TheGame::getInstance()->getEnv()->addMessageBox(wtitle.c_str(), ws.c_str());

        irr::IEventReceiver* savedER = MenuManager::getInstance()->getCurrentEventReceiver();

        ModalEventReceiver* mer = new ModalEventReceiver();
        TheGame::getInstance()->getEnv()->setUserEventReceiver(mer);

        irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
        while (!mer->closed)
        {
            irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
            device->run();
            device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
            device->getGUIEnvironment()->drawAll();
            device->getVideoDriver()->endScene();
            device->sleep(50);
        }

        TheGame::getInstance()->getEnv()->setUserEventReceiver(savedER);
        delete mer;
    }
    exit(1);
}

/* extern "C" */ void PrintMessage(int num, const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    char s[1000], title[100];
    _snprintf_s(title, sizeof(title), "Rally Raid message - code: %d", num);
    _vsnprintf_s(s, sizeof(s), msg, ap);
    s[sizeof(s)-1] = 0;
    _printMessage(num, "Rally Raid message", msg, ap);
    if (printToWindow)
    {
#ifdef NODEF //_MSC_VER
        irr::core::stringw ws = L" ";
        irr::core::stringw wtitle = L" ";
        ws = s;
        wtitle = title;
        MessageBox(0, ws.c_str(), wtitle.c_str(), MB_OK | MB_ICONWARNING);
#else
        MessageBox(0, s, title, MB_OK | MB_ICONWARNING);
#endif
    }
    else
    {
        irr::core::stringw ws = L" ";
        irr::core::stringw wtitle = L" ";
        ws = s;
        wtitle = title;
        TheGame::getInstance()->getEnv()->addMessageBox(wtitle.c_str(), ws.c_str());

        irr::IEventReceiver* savedER = MenuManager::getInstance()->getCurrentEventReceiver();

        ModalEventReceiver* mer = new ModalEventReceiver();
        TheGame::getInstance()->getEnv()->setUserEventReceiver(mer);

        irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
        while (!mer->closed)
        {
            irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
            device->run();
            device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
            device->getGUIEnvironment()->drawAll();
            device->getVideoDriver()->endScene();
            device->sleep(50);
        }

        TheGame::getInstance()->getEnv()->setUserEventReceiver(savedER);
        delete mer;
    }
//    printMessage (num,"ODE Message",msg,ap);
}

#endif // WIN32
