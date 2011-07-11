
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
#include "Stage.h"
#include "ItinerPoint.h"
#include "LoadingThread.h"


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
      editorMode(true),
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
         terminate = true;
    }
    else
    {
        terminate = false;

        device->setWindowCaption(L"Rally Raid 2012");

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
        fps_camera = smgr->addCameraSceneNodeFPS(0, 100.f, 0.1f);
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

        dprintf(MY_DEBUG_NOTE, "Initialize event receiver\n");
        eventReceiver = new EventReceiver();
        dprintf(MY_DEBUG_NOTE, "Initialize Havok\n");
        hk::initialize();
        dprintf(MY_DEBUG_NOTE, "Initialize offset manager\n");
        OffsetManager::initialize();
        offsetManager = OffsetManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize object pool manager\n");
        ObjectPoolManager::initialize();
        dprintf(MY_DEBUG_NOTE, "Initialize road type manager\n");
        RoadTypeManager::initialize();
        roadTypeManager = RoadTypeManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize road manager\n");
        RoadManager::initialize();
        roadManager = RoadManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize earth\n");
        TheEarth::initialize();
        earth = TheEarth::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize VehicleTypeManager\n");
        VehicleTypeManager::initialize();
        vehicleTypeManager = VehicleTypeManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize VehicleManager\n");
        VehicleManager::initialize();
        vehicleManager = VehicleManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize SoundEngine\n");
        MySoundEngine::initialize();
        soundEngine = MySoundEngine::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize object wire\n");
        ObjectWire::initialize();
        objectWire = ObjectWire::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize player\n");
        Player::initialize();
        player = Player::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize itiner manager\n");
        ItinerManager::initialize();
        itinerManager = ItinerManager::getInstance();
        dprintf(MY_DEBUG_NOTE, "Initialize waypoint manager\n");
        WayPointManager::initialize();
        wayPointManager = WayPointManager::getInstance();
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
    params.WindowSize = irr::core::dimension2du(Settings::getInstance()->resolutionX, Settings::getInstance()->resolutionY);
    params.Fullscreen = Settings::getInstance()->fullScreen;
    params.Bits = Settings::getInstance()->displayBits;
    params.Vsync = Settings::getInstance()->vsync;
    //assert(0);
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
        if (device->isWindowActive())
        {
            tick = device->getTimer()->getTime();

            if (inGame)
            {
                // -------------------------------
                //         update events
                // -------------------------------
                if (eventReceiver)
                {
                    eventReceiver->checkEvents();
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
                offsetManager->update(offsetManager->getOffset()+camera->getPosition());
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
                if (physUpdateDone)
                {
                    OffsetObject::updateDynamicToPhys();
                    handleUpdatePos(true); // update the camera to the player position
                }

                if (tick - lastSlowTick > slowStep_ms /*Tick::less(lastSlowTick, tick) lastSlowTick < tick || tick < slowStep_ms*/)
                {
                    //lastSlowTick += slowStep_ms;
                    lastSlowTick = tick;// + slowStep_ms;
                    
                    if (editorMode)
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

            driver->setRenderTarget(0, true, true, irr::video::SColor(0, 0, 0, 255));
            //printf("prerender\n");
            earth->registerVisual();
            hud->preRender(cameraAngle);
            //printf("scene mgr drawAll\n");
            smgr->drawAll();
            //printf("menu and env render\n");
            messageManager->updateText(tick);
            if (editorMode)
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
            }
            
            env->drawAll();
            
            //printf("hud render\n");
            //testQuad.render();
            driver->endScene();
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
    cameraOffsetObject->addToManager();
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
    for (;stepCnt > 0; stepCnt--)
    {
        offsetManager->update(offsetManager->getOffset()+camera->getPosition());
        objectWire->update(offsetManager->getOffset()+camera->getPosition());
        hk::step(step_sec);
        OffsetObject::updateDynamicToPhys();
        if (stepCnt == 1) handleUpdatePos(true); // update the camera to the player position
    }
}

void TheGame::switchCamera()
{
    irr::core::vector3df pos = camera->getPosition();
    irr::core::vector3df tar = camera->getTarget();
    camera = ((camera == fix_camera) ? fps_camera : fix_camera);
    smgr->setActiveCamera(camera);
    camera->setPosition(pos);
    camera->setTarget(tar);
    if (cameraOffsetObject) cameraOffsetObject->setNode(camera);
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
        carrot = player->getVehicleMatrix().getRotationDegrees();

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

    if (phys && inGame)
    {
        wayPointManager->update(offsetManager->getOffset()+camera->getPosition());
        itinerManager->update(offsetManager->getOffset()+camera->getPosition());
        player->update();
    }
    
    cameraDirection = camera->getTarget()-camera->getPosition();
    cameraDirection.normalize();
    // calculate cameraAngle
    cameraAngle = (float)(irr::core::vector2df(cameraDirection.X, cameraDirection.Z)).getAngle();
    
    irr::core::vector3df velocity;
    if (camera != fps_camera && player->getVehicle() != 0) velocity = player->getVehicle()->getLinearVelocity();
    soundEngine->setListenerPosition(camera->getPosition(), cameraDirection, camera->getUpVector(), velocity);
}
