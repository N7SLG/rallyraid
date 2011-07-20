
#include "LoadingThread.h"
#include "TheGame.h"
#include "ScreenQuad.h"
#include "Shaders.h"
#include "FontManager.h"


// static stuff
LoadingThread* LoadingThread::loadingThread = 0;

void LoadingThread::initialize()
{
    dprintf(MY_DEBUG_INFO, "LoadingThread::initialize(): loadingThread: %p\n", loadingThread);
    if (loadingThread == 0)
    {
        loadingThread = new LoadingThread();
    }
}

void LoadingThread::finalize()
{
    dprintf(MY_DEBUG_IMPINFO, "LoadingThread::finalize(): loadingThread: %p\n", loadingThread);
    //assert(0);
    if (loadingThread)
    {
        delete loadingThread;
        loadingThread = 0;
    }
}


LoadingThread::LoadingThread()
    : loading(false),
      running(false),
      bgQuad(0),
      staticTextLoading(0)
{
    bgQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(0, 0),
        irr::core::dimension2du(TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        false);
    bgQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    bgQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));
    bgQuad->setVisible(false);

    staticTextLoading = TheGame::getInstance()->getEnv()->addStaticText(L"Loading...",
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width/2 - 400,54,TheGame::getInstance()->getScreenSize().Height/2 + 400,88),
        false, false, 0, -1, false);
    staticTextLoading->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL18));
    staticTextLoading->setOverrideColor(irr::video::SColor(255, 255, 255, 255));
    staticTextLoading->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);
    staticTextLoading->setVisible(false);
}

LoadingThread::~LoadingThread()
{
    endLoading();
    terminateAndWait();
    if (bgQuad)
    {
        delete bgQuad;
        bgQuad = 0;
    }
}

void LoadingThread::startLoading()
{
    dprintf(MY_DEBUG_NOTE, "startLoading() called, loading: %u, running: %u\n", loading, running);
    if (loading) return;
    
    loading = true;
    bgQuad->setVisible(true);
    staticTextLoading->setVisible(true);

    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();

    device->run();
    device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
    bgQuad->render();
    device->getGUIEnvironment()->drawAll();
    device->getVideoDriver()->endScene();

    //execute();
}

void LoadingThread::endLoading()
{
    dprintf(MY_DEBUG_NOTE, "endLoading() called, loading: %u, running: %u\n", loading, running);
    if (!loading) return;
    
    staticTextLoading->setVisible(false);
    bgQuad->setVisible(false);
    loading = false;
    //unsigned int i = 1;
    while (running)
    {
        //dprintf(MY_DEBUG_NOTE, "wait loading thread end: %u\n", running);
        //TheGame::getInstance()->getDevice()->sleep(1000*i);
        //i++;
    }
    dprintf(MY_DEBUG_NOTE, "endLoading() finished, loading: %u, running: %u\n", loading, running);
}

void LoadingThread::run()
{
    dprintf(MY_DEBUG_NOTE, "start loading thread render\n");
    running = true;
    while (loading)
    {
        render();
        TheGame::getInstance()->getDevice()->sleep(250);
    }
    dprintf(MY_DEBUG_NOTE, "end loading thread render\n");
    running = false;
}

void LoadingThread::refresh()
{
    dprintf(MY_DEBUG_NOTE, "start refresh: loading: %u, running: %u\n", loading, running);
    if (loading && !running)
    {
        render();
    }
    dprintf(MY_DEBUG_NOTE, "end refresh\n");
}

void LoadingThread::render()
{
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
    dprintf(MY_DEBUG_NOTE, "loading thread render step\n");
    device->run();
    device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
    bgQuad->render();
    device->getGUIEnvironment()->drawAll();
    device->getVideoDriver()->endScene();
}
