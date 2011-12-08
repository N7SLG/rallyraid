
#include "TheGame.h"
#include "stdafx.h"
#include "EventReceiver.h"
#include "Settings.h"
#include "ShadersSM20.h"
#include "OffsetManager.h"
#include "hk.h"
#include "ObjectPoolManager.h"
#include "TheEarth.h"
#include "VehicleTypeManager.h"
#include "VehicleManager.h"
#include "Vehicle.h"
#include "MySound.h"
#include "ObjectWire.h"
#include "Player.h"
#include "RaceManager.h"
#include "MenuManager.h"
#include "MenuPageEditor.h"
#include "MenuPageStage.h"
#include "MenuPageMain.h"
#include "Hud.h"
#include "RoadManager.h"
#include "RoadTypeManager.h"
#include "ItinerManager.h"
#include "GamePlay.h"
#include "Tick.h"
#include "WayPointManager.h"
#include "FontManager.h"
#include "MessageManager.h"
#include "Terrain_defs.h"
#include "Terrain.h"
#include "Stage.h"
#include "ItinerPoint.h"
#include "LoadingThread.h"
#include "ShadowRenderer.h"
#include <CSceneNodeAnimatorCameraFPS.h>
#include "error.h"

void renderVehicleImages();

// static stuff
TheGame* TheGame::theGame = 0;

void TheGame::initialize()
{
    dprintf(MY_DEBUG_INFO, "TheGame::initialize(): theGame: %p\n", theGame);
    if (theGame == 0)
    {
        theGame = new TheGame();
    }
}

void TheGame::finalize()
{
    dprintf(MY_DEBUG_IMPINFO, "TheGame::finalize(): theGame: %p\n", theGame);
    //assert(0);
    if (theGame)
    {
        delete theGame;
        theGame = 0;
    }
}


// non-static stuff    
TheGame::TheGame()
    : device(0), driver(0), smgr(0), camera(0), fix_camera(0), fps_camera(0),
      env(0),
      eventReceiver(0),
      shaders(0),
      offsetManager(0),
      earth(0),
      vehicleTypeManager(0),
      vehicleManager(0),
      soundEngine(0),
      objectWire(0),
      player(0),
      raceManager(0),
      menuManager(0),
      hud(0),
      roadManager(0),
      roadTypeManager(0),
      itinerManager(0),
      gamePlay(0),
      wayPointManager(0),
      fontManager(0),
      messageManager(0),
      loadingThread(0),
      terminate(true),
      windowId(0),
      lastScreenSize(),
      failed_render(0),
      tick(0),
      lastPhysTick(0),
      lastSlowTick(0),
      physicsOngoing(true),
      cameraOffsetObject(0),
      dynCamDist(2.f),
      cameraDirection(),
      cameraAngle(0.0f),
      inGame(true),
      editorSeconds(0),
      editorLastTick(0),
      skydome(0)
{
    dprintf(MY_DEBUG_INFO, "TheGame::TheGame(): this: %p\n", this);
    theGame = this;

    irr::SIrrlichtCreationParameters params;
    params.HighPrecisionFPU = true;
    //params.DriverType = irr::video::EDT_DIRECT3D9 /*irr::video::EDT_OPENGL*/;
    //params.WindowSize = irr::core::dimension2du(1280, 800);

    readSettings(params);

    device = irr::createDeviceEx(params);

    windowId = (size_t)params.WindowId;

    if (device==0)
    {
         dprintf(MY_DEBUG_ERROR, "unable to create device\n");
         PrintMessage(1, "Unable to create device!\nTry to edit the data/settings.cfg file, or write mail to balazs.tuska@gmail.com to get help.");
         terminate = true;
    }
    else
    {
        terminate = false;
        
        switchPrintDestination();

        device->setWindowCaption(L"Rally Raid 1.0");

        dprintf(MY_DEBUG_NOTE, "Initialize Irrlicht members\n");
        driver = device->getVideoDriver();
        smgr = device->getSceneManager();
        env = device->getGUIEnvironment();
        {
            for (unsigned int i = 0; i < irr::gui::EGDC_COUNT ; ++i)
            {
                irr::video::SColor col = env->getSkin()->getColor(/*irr::gui::EGDC_WINDOW*/(irr::gui::EGUI_DEFAULT_COLOR)i);
                col.setAlpha(255);
                env->getSkin()->setColor(/*irr::gui::EGDC_WINDOW*/(irr::gui::EGUI_DEFAULT_COLOR)i, col);
            }
        }
        fix_camera = smgr->addCameraSceneNode();
        fps_camera = smgr->addCameraSceneNodeFPS(0, 100.f, Settings::getInstance()->fpsStep);
        fix_camera->setFarValue(28000.f);
        fix_camera->setNearValue(0.5f);
        fps_camera->setFarValue(28000.f);
        fps_camera->setNearValue(0.5f);
        camera = fix_camera;
        smgr->setActiveCamera(camera);
        lastScreenSize = getScreenSize();

        dprintf(MY_DEBUG_NOTE, "Initialize shaders\n");
        Shaders::initialize();
        shaders = Shaders::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize font manager\n");
        FontManager::initialize();
        fontManager = FontManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize message manager\n");
        MessageManager::initialize();
        messageManager = MessageManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize loading thread\n");
        LoadingThread::initialize();
        loadingThread = LoadingThread::getInstance();

        loadingThread->startLoading();

        loadingThread->setLargeSteps(0, 6);
        dprintf(MY_DEBUG_NOTE, "Initialize event receiver\n");
        eventReceiver = new EventReceiver();
        loadingThread->setLargeSteps(7, 13);
        dprintf(MY_DEBUG_NOTE, "Initialize Havok\n");
        hk::initialize();
        loadingThread->setLargeSteps(14, 20);
        dprintf(MY_DEBUG_NOTE, "Initialize offset manager\n");
        OffsetManager::initialize();
        offsetManager = OffsetManager::getInstance();
        loadingThread->setLargeSteps(21, 27);
        dprintf(MY_DEBUG_NOTE, "Initialize object pool manager\n");
        ObjectPoolManager::initialize();
        loadingThread->setLargeSteps(28, 34);
        dprintf(MY_DEBUG_NOTE, "Initialize shadow renderer\n");
        ShadowRenderer::initialize();
        loadingThread->setLargeSteps(35, 41);
        dprintf(MY_DEBUG_NOTE, "Initialize road type manager\n");
        RoadTypeManager::initialize();
        roadTypeManager = RoadTypeManager::getInstance();
        loadingThread->setLargeSteps(42, 48);
        dprintf(MY_DEBUG_NOTE, "Initialize road manager\n");
        RoadManager::initialize();
        roadManager = RoadManager::getInstance();
        loadingThread->setLargeSteps(49, 55);
        dprintf(MY_DEBUG_NOTE, "Initialize earth\n");
        Terrain::initialize();
        TheEarth::initialize();
        earth = TheEarth::getInstance();
        loadingThread->setLargeSteps(56, 62);
        dprintf(MY_DEBUG_NOTE, "Initialize VehicleTypeManager\n");
        VehicleTypeManager::initialize();
        vehicleTypeManager = VehicleTypeManager::getInstance();
        loadingThread->setLargeSteps(63, 69);
        dprintf(MY_DEBUG_NOTE, "Initialize VehicleManager\n");
        VehicleManager::initialize();
        vehicleManager = VehicleManager::getInstance();
        loadingThread->setLargeSteps(70, 76);
        dprintf(MY_DEBUG_NOTE, "Initialize SoundEngine\n");
        MySoundEngine::initialize();
        soundEngine = MySoundEngine::getInstance();
        loadingThread->setLargeSteps(77, 83);
        dprintf(MY_DEBUG_NOTE, "Initialize object wire\n");
        ObjectWire::initialize();
        objectWire = ObjectWire::getInstance();
        loadingThread->setLargeSteps(84, 90);
        dprintf(MY_DEBUG_NOTE, "Initialize player\n");
        Player::initialize();
        player = Player::getInstance();
        loadingThread->setLargeSteps(91, 92);
        dprintf(MY_DEBUG_NOTE, "Initialize itiner manager\n");
        ItinerManager::initialize();
        itinerManager = ItinerManager::getInstance();
        loadingThread->setLargeSteps(93, 94);
        dprintf(MY_DEBUG_NOTE, "Initialize waypoint manager\n");
        WayPointManager::initialize();
        wayPointManager = WayPointManager::getInstance();
        loadingThread->setLargeSteps(95, 100);
        dprintf(MY_DEBUG_NOTE, "Initialize race manager\n");
        RaceManager::initialize();
        raceManager = RaceManager::getInstance();

        loadingThread->endLoading();

        dprintf(MY_DEBUG_NOTE, "Initialize menu manager\n");
        MenuManager::initialize();
        menuManager = MenuManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize game play\n");
        GamePlay::initialize();
        gamePlay = GamePlay::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize hud\n");
        Hud::initialize();
        hud = Hud::getInstance();

        //renderVehicleImages();

        skydome = smgr->addSkyDomeSceneNode(driver->getTexture("data/skys/fairday.jpg"),16,8,0.95f,2.0f);
        /*if (useShaders && useCgShaders)
        {
            skydome->setMaterialType((video::E_MATERIAL_TYPE)myMaterialType_sky);
            skydome->setMaterialTexture(1, driver->getTexture("data/skys/skystars.jpg"));
        }*/
        skydome->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }
}

TheGame::~TheGame()
{
    dprintf(MY_DEBUG_IMPINFO, "TheGame::~TheGame(): this: %p\n", this);

    earth->terminateAndWait();

    if (eventReceiver)
    {
        delete eventReceiver;
        eventReceiver = 0;
    }

    shaders = 0;
    windowId = 0;
    smgr = 0;
    fps_camera = fix_camera = camera = 0;
    if (cameraOffsetObject)
    {
        cameraOffsetObject->removeFromManager();
        delete cameraOffsetObject;
        cameraOffsetObject = 0;
    }
    loadingThread = 0;
    messageManager = 0;
    fontManager = 0;
    offsetManager = 0;
    earth = 0;
    vehicleManager = 0;
    vehicleTypeManager = 0;
    soundEngine = 0;
    objectWire = 0;
    player = 0;
    raceManager = 0;
    itinerManager = 0;
    wayPointManager = 0;
    roadManager = 0;
    roadTypeManager = 0;
    menuManager = 0;
    gamePlay = 0;
    hud = 0;

    dprintf(MY_DEBUG_NOTE, "Finalize game play\n");
    GamePlay::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize menu manager\n");
    MenuManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize hud\n");
    Hud::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize race manager\n");
    RaceManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize waypoint manager\n");
    WayPointManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize itiner manager\n");
    ItinerManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize player\n");
    Player::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize object wire\n");
    ObjectWire::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize soundEngine\n");
    MySoundEngine::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize vehicleManager\n");
    VehicleManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize vehicleTypeManager\n");
    VehicleTypeManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize earth\n");
    TheEarth::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize road manager\n");
    RoadManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize road type manager\n");
    RoadTypeManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize shadow renderer\n");
    ShadowRenderer::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize offsetManager\n");
    OffsetManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize objectPoolManager\n");
    ObjectPoolManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize loading thread\n");
    LoadingThread::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize message manager\n");
    MessageManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize font manager\n");
    FontManager::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize shaders\n");
    Shaders::finalize();
    dprintf(MY_DEBUG_NOTE, "Finalize settings\n");
    Settings::finalize();
    if (driver)
    {
        driver->removeAllTextures();
        driver->removeAllHardwareBuffers();
        driver = 0;
    }
    if (device)
    {
        device->drop();
        device = 0;
    }
    hk::finalize();
}

void TheGame::readSettings(irr::SIrrlichtCreationParameters& params)
{
    Settings::initialize();

    if (Settings::getInstance()->driverType == "d3d9")
    {
        params.DriverType = irr::video::EDT_DIRECT3D9 /*irr::video::EDT_OPENGL*/;
    }
    else
    {
        params.DriverType = irr::video::EDT_OPENGL;
    }
    if (Settings::getInstance()->desktopResolution && Settings::getInstance()->fullScreen)
    {
        irr::IrrlichtDevice* nullDev = irr::createDevice(irr::video::EDT_NULL);
        dprintf(MY_DEBUG_NOTE, "Use full screen with auto-detection\n");
        if (nullDev)
        {
            dprintf(MY_DEBUG_NOTE, "null device found auto-detection is possible.\n");
            params.WindowSize = nullDev->getVideoModeList()->getDesktopResolution();
            params.Bits = nullDev->getVideoModeList()->getDesktopDepth();
            nullDev->drop();
            dprintf(MY_DEBUG_INFO, "null device found auto-detection is possible. detected: %dx%dx%d\n",
                params.WindowSize.Width, params.WindowSize.Height, params.Bits);
        }
        else
        {
            dprintf(MY_DEBUG_INFO, "null device not found auto-detection is not possible!!!\n");
            PrintMessage(2, "Unable to detect desktop resolution!\nUse resolution from the settings: %ux%u!",
                Settings::getInstance()->resolutionX, Settings::getInstance()->resolutionY);
        }
    }
    else
    {
        params.WindowSize = irr::core::dimension2du(Settings::getInstance()->resolutionX, Settings::getInstance()->resolutionY);
        params.Bits = Settings::getInstance()->displayBits;
    }
    params.Fullscreen = Settings::getInstance()->fullScreen;
    params.Vsync = Settings::getInstance()->vsync;
    //assert(0);
    if (Settings::getInstance()->editorMode == false && (params.WindowSize.Width < 1280 || params.WindowSize.Height < 768))
    {
         PrintMessage(3, "Your current resolution (%ux%u) is lower then 1280x768!\nThe game was not tested on this resolution!",
             Settings::getInstance()->resolutionX, Settings::getInstance()->resolutionY);
    }
}

void TheGame::loop()
{
    dprintf(MY_DEBUG_INFO, "TheGame::loop() begin, terminate: %s\n", terminate?"true":"false");

    if (terminate)
    {
        dprintf(MY_DEBUG_INFO, "TheGame::loop() end: not initialized\n");
        return;
    }

    smgr->setAmbientLight(irr::video::SColorf(0.3f, 0.3f, 0.3f));
    smgr->setShadowColor(irr::video::SColor(255, 50, 50, 50));

    /*
    Ogre::Light* dLight = smgr->createLight("dLight");
    dLight->setType(Ogre::Light::LT_DIRECTIONAL);
    dLight->setDiffuseColour(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
    dLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));
    dLight->setDirection(0, -1, 1);
    */
    const unsigned int step_ms = 1000 / Settings::getInstance()->targetFps;
    const unsigned int slowStep_ms = 100;
    const float step_sec = 1.f / (float)Settings::getInstance()->targetFps;
    unsigned int physUpdate;
    unsigned int eventReceiverTick = 0;
    unsigned int lastEventReceiverTick = 0;
    const unsigned int eventReceiverCheckRate = 1000 / 30;

    /*ScreenQuad testQuad(driver, irr::core::position2di(10, 10), irr::core::dimension2du(400, 300));
    // ScreenQuad testQuad(driver, irr::core::position2di(0, 0), driver->getScreenSize());

    testQuad.getMaterial().setTexture(0, driver->getTexture("data/bg/dakar_bg1.jpg"));
    testQuad.getMaterial().MaterialType = shaders->materialMap["quad2d"];
    //testQuad.getMaterial().setTexture(0, driver->getTexture("data/menu_textures/bg_frame_main_1280.png"));
    testQuad.rotate(30.0f, irr::core::position2di(150, 150));*/

    //OffsetObject* car = ObjectPoolManager::getInstance()->getObject("vw3", initialPos+initialDir);
    //printf("car off: %f, %f (%f, %f)\n", offsetManager->getOffset().X, offsetManager->getOffset().Z,
    //    car->getNode()->getPosition().X, car->getNode()->getPosition().Z);
    //Vehicle* car = new Vehicle("vw3", initialPos+initialDir, irr::core::vector3df());

    MenuManager::getInstance()->open(MenuManager::MP_MAIN);
    //gamePlay->startGame(0);

    while (device->run() && !terminate)
    {
        if (device->isWindowActive() || Settings::getInstance()->AIPlayer)
        {
            tick = device->getTimer()->getTime();
            eventReceiverTick = tick / eventReceiverCheckRate;
            //bool ou = false;
            if (inGame)
            {
                // -------------------------------
                //         update events
                // -------------------------------
                if (eventReceiver && eventReceiverTick != lastEventReceiverTick)
                {
                    eventReceiver->checkEvents();
                    lastEventReceiverTick = eventReceiverTick;
                }
                /*
                if (eventReceiver)
                {
                    eventReceiver->updateWindow(width, height);
                }
                */

                // -------------------------------
                //         update physics
                // -------------------------------
                //camera->setTarget(camera->getPosition()+initialDir);
                //printf("off: %f, %f (%f, %f)\n", offsetManager->getOffset().X, offsetManager->getOffset().Z,
                //    camera->getPosition().X, camera->getPosition().Z);
                /*bool ou = */offsetManager->update(offsetManager->getOffset()+camera->getPosition());
                objectWire->update(offsetManager->getOffset()+camera->getPosition());
                //printf("off: %f, %f (%f, %f)\n", offsetManager->getOffset().X, offsetManager->getOffset().Z,
                //    camera->getPosition().X, camera->getPosition().Z);
                //assert(0);
                //car->getBody()->activate();

                //car->setHandbrake(0.0f);
                //car->setTorque(-0.2f);
                //car->setSteer(-0.4f);
                bool physUpdateDone = false;
                physUpdate = (tick - lastPhysTick) / step_ms;
                if (physUpdate > 10)
                {
                    physUpdate = 10;
                    lastPhysTick = tick - (10*step_ms);
                }
                while (/*lastPhysTick < tick && */physUpdate)
                {
                    if (physicsOngoing && player->getFirstPressed())
                    {
                        hk::step(step_sec);
                        physUpdateDone = true;
                    }
                    lastPhysTick += step_ms;
                    physUpdate--;
                }
                /*
                update dynamic object position
                */
                if (physUpdateDone/* || ou*/)
                {
                    OffsetObject::updateDynamicToPhys();
                    handleUpdatePos(true); // update the camera to the player position
                }

                if (tick - lastSlowTick > slowStep_ms /*Tick::less(lastSlowTick, tick) lastSlowTick < tick || tick < slowStep_ms*/)
                {
                    //lastSlowTick += slowStep_ms;
                    lastSlowTick = tick;// + slowStep_ms;
                    
                    if (Settings::getInstance()->editorMode)
                    {
                        unsigned int mtick = tick / 1000;
                        if (mtick != editorLastTick)
                        {
                            editorLastTick = mtick;
                            editorSeconds++;
                        }
                        
                        irr::core::stringw str = L"Secs: ";
                        str += editorSeconds;
                        str += L",     Pos: ";
                        str += (int)camera->getPosition().X;
                        str += L", ";
                        str += (int)camera->getPosition().Y;
                        str += L", ";
                        str += (int)camera->getPosition().Z;
                        str += L" - Offset: ";
                        str += (int)offsetManager->getOffset().X;
                        str += L", ";
                        str += (int)offsetManager->getOffset().Z;
                        str += ",     Tile pos: ";
                        str += ((int)offsetManager->getOffset().X+(int)camera->getPosition().X)/TILE_SIZE;
                        str += L", ";
                        str += abs((int)offsetManager->getOffset().Z+(int)camera->getPosition().Z)/TILE_SIZE;
                        str += L"        Polygon count: ";
                        str += driver->getPrimitiveCountDrawn();
                        if (TheEarth::getInstance()->threadIsRunning())
                        {
                            str += L"       thread is running";
                        }
                        
                        str += L"\nDetail pos: ";
                        str += (int)((offsetManager->getOffset().X+camera->getPosition().X)/TILE_DETAIL_SCALE_F);
                        str += L", ";
                        str += (int)((offsetManager->getOffset().Z+camera->getPosition().Z)/TILE_DETAIL_SCALE_F);
                        if (!RaceManager::getInstance()->getCurrentHeightModifierList().empty())
                        {
                            str += L", last: ";
                            str += (int)(RaceManager::getInstance()->getCurrentHeightModifierList().back().pos.X/TILE_DETAIL_SCALE_F);
                            str += L", ";
                            str += (int)(RaceManager::getInstance()->getCurrentHeightModifierList().back().pos.Z/TILE_DETAIL_SCALE_F);
                            str += L", diff: ";
                            str += (int)((offsetManager->getOffset().X+camera->getPosition().X)/TILE_DETAIL_SCALE_F) -
                                (int)(RaceManager::getInstance()->getCurrentHeightModifierList().back().pos.X/TILE_DETAIL_SCALE_F);
                            str += L", ";
                            str += (int)((offsetManager->getOffset().Z+camera->getPosition().Z)/TILE_DETAIL_SCALE_F) -
                                (int)(RaceManager::getInstance()->getCurrentHeightModifierList().back().pos.Z/TILE_DETAIL_SCALE_F);
                        }
                        str += L",     Distance to last itinerary point: ";
                        if (RaceManager::getInstance()->getCurrentStage() && !RaceManager::getInstance()->getCurrentStage()->getItinerPointList().empty())
                        {
                            str += (int)(RaceManager::getInstance()->getCurrentStage()->getItinerPointList().back()->getPos().getDistanceFrom(offsetManager->getOffset()+camera->getPosition()));
                        }
                        else
                        {
                            str += L"0";
                        }
                        str += L"     FPS speed: ";
                        str += (int)(getFPSSpeed()*10.f);
                        
                        hud->getEditorText()->setText(str.c_str());
                    }

                    earth->update(offsetManager->getOffset()+camera->getPosition(), cameraDirection);
                }
                gamePlay->update(tick, offsetManager->getOffset()+camera->getPosition());
            }
            else
            {
                if (eventReceiver)
                {
                    eventReceiver->checkEventsMenu();
                }

                lastPhysTick = lastSlowTick = tick;
                editorLastTick = tick / 1000;
            }
            // -------------------------------
            //         update graphics
            // -------------------------------
            if (!driver->beginScene(true, true, irr::video::SColor(0,192,192,192)))
            {
                driver->endScene();
                driver->OnResize(lastScreenSize);
                failed_render++;
                if (failed_render > 5)
                {
                    dprintf(MY_DEBUG_ERROR, "failed to render 5 times, quit\n");
                    terminate = true;
                }
                continue;
            }
            failed_render = 0;

            ShadowRenderer::getInstance()->renderShadowNodes();

            driver->setRenderTarget(0, true, true, irr::video::SColor(0, 0, 0, 255));
            //printf("prerender\n");
            //earth->registerVisual();
            hud->preRender(cameraAngle);
            //printf("scene mgr drawAll\n");
            smgr->drawAll();
            //printf("menu and env render\n");
            messageManager->updateText(tick);
            if (Settings::getInstance()->editorMode)
            {
                MenuPageEditor::render();
            }
            if (inGame)
            {
                hud->render();
            }
            else
            {
                if (MenuPageStage::menuPageStage->isVisible())
                {
                    MenuPageStage::menuPageStage->render();
                }
                if (MenuPageMain::menuPageMain->isVisible())
                {
                    MenuPageMain::menuPageMain->render();
                }
            }
            
            env->drawAll();
            
            //printf("hud render\n");
            //testQuad.render();
            driver->endScene();
            //assert(!ou);
            //printf("5\n");

            //int fps = driver->getFPS();

            //last_tick = tick;
        } // if (window->active)
        else
        {
            device->sleep(100);
            lastPhysTick = lastSlowTick = device->getTimer()->getTime();
            editorLastTick = lastPhysTick / 1000;
        } // if (!window->active)
    } // wile (run && !terminate)

    dprintf(MY_DEBUG_INFO, "TheGame::loop() end\n");
}

void TheGame::reset(const irr::core::vector3df& apos, const irr::core::vector3df& dir)
{
    if (cameraOffsetObject)
    {
        cameraOffsetObject->removeFromManager();
        delete cameraOffsetObject;
        cameraOffsetObject = 0;
    }
    if (camera == fps_camera) switchCamera();
    camera->setPosition(apos);
    camera->setTarget(camera->getPosition()+dir);

    cameraOffsetObject = new OffsetObject(camera/*, true*/);
    cameraOffsetObject->addToManagerBegin();
    offsetManager->update(camera->getPosition());
    cameraOffsetObject->setUpdateCB(this);
}

void TheGame::resetTick()
{
    device->run();
    lastPhysTick = lastSlowTick = tick = device->getTimer()->getTime();
    lastPhysTick--;
    editorLastTick = tick / 1000;
    editorSeconds = 0;
}

void TheGame::doFewSteps(unsigned int stepCnt)
{
    const float step_sec = 1.f / (float)Settings::getInstance()->targetFps;
    unsigned int gpu = 0;
    unsigned int falseTick = 1000;
    for (;stepCnt > 0; stepCnt--, gpu++)
    {
        offsetManager->update(offsetManager->getOffset()+camera->getPosition());
        objectWire->update(offsetManager->getOffset()+camera->getPosition());
        hk::step(step_sec);
        OffsetObject::updateDynamicToPhys();
        if (gpu < 2)
        {
            gamePlay->update(falseTick, offsetManager->getOffset()+camera->getPosition(), true);
            falseTick+=100;
        }
        if (stepCnt == 1)
        {
            inGame = false; // turn off inGame -> not to update itiner stuff
            handleUpdatePos(true); // update the camera to the player position
            inGame = true;
        }
    }
}

void TheGame::switchCamera()
{
    irr::core::vector3df pos = camera->getPosition();
    irr::core::vector3df tar = camera->getTarget();
    camera = ((camera == fix_camera) ? fps_camera : fix_camera);
    resetMouseCursor();
    smgr->setActiveCamera(camera);
    camera->setPosition(pos);
    camera->setTarget(tar);
    if (cameraOffsetObject) cameraOffsetObject->setNode(camera);
}

float TheGame::getFPSSpeed()
{
    float ret = 0.0f;
    if (!fps_camera->getAnimators().empty())
    {
        ret = ((irr::scene::CSceneNodeAnimatorCameraFPS*)(*(fps_camera->getAnimators().begin())))->getMoveSpeed();
    }
    return ret;
}

void TheGame::setFPSSpeed(float speed)
{
    if (!fps_camera->getAnimators().empty())
    {
        ((irr::scene::CSceneNodeAnimatorCameraFPS*)(*(fps_camera->getAnimators().begin())))->setMoveSpeed(speed);
    }
}

void TheGame::incFPSSpeed()
{
    float fpsSpeed = getFPSSpeed();
    fpsSpeed += Settings::getInstance()->fpsStep;
    if (fpsSpeed > 2.0f)
    {
        fpsSpeed = 2.0f;
    }
    setFPSSpeed(fpsSpeed);
}

void TheGame::decFPSSpeed()
{
    float fpsSpeed = getFPSSpeed();
    fpsSpeed -= Settings::getInstance()->fpsStep;
    if (fpsSpeed < Settings::getInstance()->fpsStep)
    {
        fpsSpeed = Settings::getInstance()->fpsStep;
    }
    setFPSSpeed(fpsSpeed);
}


void TheGame::resetMouseCursor(bool onlyFPS)
{
    if (!onlyFPS || (onlyFPS && camera == fps_camera))
    {
        device->getCursorControl()->setPosition((int)(driver->getScreenSize().Width / 2), (int)(driver->getScreenSize().Height / 2));
    }
}

void TheGame::handleUpdatePos(bool phys)
{
    if (phys && camera != fps_camera)
    {
        // update the camera pos to the player pos
        const irr::core::matrix4& viewPos = player->getViewPos();
        const irr::core::matrix4& viewDest = player->getViewDest();
        irr::core::vector3df campos;
        irr::core::vector3df camtar;
        irr::core::vector3df centar;
        irr::core::vector3df camup = irr::core::vector3df(0.f, 1.f, 0.f);
        irr::core::vector3df carrot;
        irr::core::matrix4 tcentar;
        tcentar.setTranslation(irr::core::vector3df(viewDest[12], viewPos[13], viewDest[14]));

        campos = (player->getVehicleMatrix() * viewPos).getTranslation();
        camtar = (player->getVehicleMatrix() * viewDest).getTranslation();
        centar = (player->getVehicleMatrix() * tcentar).getTranslation();
        carrot = player->getVehicle()->getRotationDegrees();// getVehicleMatrix().getRotationDegrees();

        camera->setTarget(camtar);
        if (player->getViewNum() == VIEW_0)
        {
            camera->setUpVector(camup);
        }
        else
        {
            camera->setUpVector(carrot.rotationToDirection(camup));
        }
        if ((player->getViewNum()+player->getViewMask())!=CALC_VIEW(VIEW_0, VIEW_CENTER) /*|| !useDynCam*/ || player->getRecenterView())
        {
            camera->setPosition(campos);
            dynCamDist = fabsf((centar - campos).getLength());
            player->clearRecenterView();
        }
        else
        {
            irr::core::vector3df dir = (camera->getPosition() - centar).normalize();
                   
            float fact = fabsf(player->getVehicle()->getSpeed())*0.01f;
                   
            dir = dir * (dynCamDist + fact * fact) ;
                   
            if (dir.Y < 0.05f) dir.Y = 0.05f;
            camera->setPosition(centar + dir);
        }
    }
    //camera->updateAbsolutePosition();

    if (phys && inGame)
    {
        wayPointManager->update(offsetManager->getOffset()+camera->getPosition());
        itinerManager->update(offsetManager->getOffset()+camera->getPosition());
        player->update();
    }
    
    cameraDirection = camera->getTarget()-camera->getPosition();
    cameraDirection.normalize();
    
    irr::core::vector3df velocity;
    if (camera != fps_camera && player->getVehicle() != 0)
    {
        velocity = player->getVehicle()->getLinearVelocity();
        cameraAngle = player->getVehicle()->getAngle();
    }
    else
    {
        // calculate cameraAngle
        cameraAngle = (float)(irr::core::vector2df(cameraDirection.X, cameraDirection.Z)).getAngle();
    }
    soundEngine->setListenerPosition(camera->getPosition(), cameraDirection, camera->getUpVector(), velocity);
}


void renderVehicleImages()
{
    printf("renderVehicleImages(): BEGIN\n");

    bool tempTexFlagMipMaps = TheGame::getInstance()->getDriver()->getTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS);
    bool tempTexFlag32 = TheGame::getInstance()->getDriver()->getTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT);
    irr::video::ITexture* car_selector_rtt = TheGame::getInstance()->getDriver()->addRenderTargetTexture(irr::core::dimension2du(1024, 1024));
    TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, tempTexFlagMipMaps);
    TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT, tempTexFlag32);

    irr::scene::ICameraSceneNode* car_selector_camera = TheGame::getInstance()->getSmgr()->addCameraSceneNode();
    car_selector_camera->setPosition(irr::core::vector3df(3.4f, 1.2f, 1.7f));
    car_selector_camera->setTarget(irr::core::vector3df(0.f,-0.5f,0.f));
    car_selector_camera->setAspectRatio(1.f);
    //car_selector_camera->setFarValue(DEFAULT_FAR_VALUE);
    TheGame::getInstance()->getSmgr()->setActiveCamera(car_selector_camera);

    for (VehicleTypeManager::vehicleTypeMap_t::const_iterator it = VehicleTypeManager::getInstance()->getVehicleTypeMap().begin();
         it != VehicleTypeManager::getInstance()->getVehicleTypeMap().end();
         it++)
    {
        printf("Create vehicle, type: %s\n", it->first.c_str());
        Vehicle* vehicle = new Vehicle(it->first, irr::core::vector3df(), irr::core::vector3df());
        
        printf("Render start\n");
        TheGame::getInstance()->getDriver()->beginScene(true, true, irr::video::SColor(0,255,0,0));
        TheGame::getInstance()->getDriver()->setRenderTarget(car_selector_rtt, true, true, irr::video::SColor(0, 255, 0, 255));
        TheGame::getInstance()->getSmgr()->drawAll();
        TheGame::getInstance()->getDriver()->endScene();
        printf("Render end\n");
        
        delete vehicle;

        std::string filename = it->first + "_preview.png";

        irr::video::IImage* outImg = TheGame::getInstance()->getDriver()->createImage(car_selector_rtt, irr::core::position2d< irr::s32 >(), car_selector_rtt->getOriginalSize());
        irr::io::IWriteFile* wf = TheGame::getInstance()->getDevice()->getFileSystem()->createAndWriteFile(filename.c_str());
        TheGame::getInstance()->getDriver()->writeImageToFile(outImg, wf);
        wf->drop();
        outImg->drop();
    }

    TheGame::getInstance()->getSmgr()->setActiveCamera(TheGame::getInstance()->getCamera());

    printf("renderVehicleImages(): END\n");
}
