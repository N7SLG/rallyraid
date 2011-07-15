#ifndef THEGAME_H
#define THEGAME_H

#include "stdafx.h"
#include "OffsetObject.h"

class EventReceiver;
class Shaders;
class OffsetManager;
class TheEarth;
class VehicleTypeManager;
class VehicleManager;
class MySoundEngine;
class ObjectWire;
class Player;
class RaceManager;
class MenuManager;
class Hud;
class RoadManager;
class RoadTypeManager;
class ItinerManager;
class GamePlay;
class WayPointManager;
class FontManager;
class MessageManager;
class LoadingThread;

class TheGame : public OffsetObjectUpdateCB
{
public:
    static void initialize();
    static void finalize();
    static TheGame* getInstance() {return theGame;}

public:
    void loop();

    // called by GamePlay::startGame()
    void reset(const irr::core::vector3df& apos, const irr::core::vector3df& dir);
    void resetTick();
    void doFewSteps(unsigned int stepCnt);

    irr::IrrlichtDevice*            getDevice();
    irr::video::IVideoDriver*       getDriver();
    irr::scene::ISceneManager*      getSmgr();
    irr::scene::ICameraSceneNode*   getCamera();
    size_t                          getWindowId();
    irr::gui::IGUIEnvironment*      getEnv();
    EventReceiver*                  getEventReceiver(); // inline
    const irr::core::dimension2du&  getScreenSize();
    unsigned int                    getTick();
    void                            switchCamera();
    bool                            isFpsCamera();
    //Shaders*                        getShaders();

    bool                            getPhysicsOngoing() {return physicsOngoing;}
    void                            setPhysicsOngoing(bool newValue) {physicsOngoing = newValue;}
    
    bool                            getInGame(); // inline
    void                            setInGame(bool val); // inline

    bool                            getEditorMode(); // inline
    void                            setEditorMode(bool val); // inline

    float                           getFPSSpeed();
    void                            setFPSSpeed(float speed);
    void                            incFPSSpeed();
    void                            decFPSSpeed();

private:
    TheGame();
    virtual ~TheGame();

protected:

private:
    static TheGame*                 theGame;

    void readSettings(irr::SIrrlichtCreationParameters& params);
    virtual void handleUpdatePos(bool phys);

private:
    // members
    irr::IrrlichtDevice*            device;
    irr::video::IVideoDriver*       driver;
    irr::scene::ISceneManager*      smgr;
    irr::scene::ICameraSceneNode*   camera;
    irr::scene::ICameraSceneNode*   fix_camera;
    irr::scene::ICameraSceneNode*   fps_camera;
    irr::gui::IGUIEnvironment*      env;

    EventReceiver*                  eventReceiver;
    Shaders*                        shaders;
    OffsetManager*                  offsetManager;
    TheEarth*                       earth;
    VehicleTypeManager*             vehicleTypeManager;
    VehicleManager*                 vehicleManager;
    MySoundEngine*                  soundEngine;
    ObjectWire*                     objectWire;
    Player*                         player;
    RaceManager*                    raceManager;
    MenuManager*                    menuManager;
    Hud*                            hud;
    RoadManager*                    roadManager;
    RoadTypeManager*                roadTypeManager;
    ItinerManager*                  itinerManager;
    GamePlay*                       gamePlay;
    WayPointManager*                wayPointManager;
    FontManager*                    fontManager;
    MessageManager*                 messageManager;
    LoadingThread*                  loadingThread;

    bool                            terminate;
    size_t                          windowId;
    irr::core::dimension2du         lastScreenSize;
    unsigned int                    failed_render;
    unsigned int                    tick;
    unsigned int                    lastPhysTick;
    unsigned int                    lastSlowTick;
    bool                            physicsOngoing;
    OffsetObject*                   cameraOffsetObject;
    float                           dynCamDist;
    irr::core::vector3df            cameraDirection;
    float                           cameraAngle;
    bool                            inGame;
    bool                            editorMode;
    unsigned int                    editorSeconds;
    unsigned int                    editorLastTick;

    irr::scene::ISceneNode*         skydome;
};

inline irr::IrrlichtDevice* TheGame::getDevice()
{
    return device;
}

inline irr::video::IVideoDriver* TheGame::getDriver()
{
    return driver;
}

inline irr::scene::ISceneManager* TheGame::getSmgr()
{
    return smgr;
}

inline irr::scene::ICameraSceneNode* TheGame::getCamera()
{
    return camera;
}

inline size_t TheGame::getWindowId()
{
    return windowId;
}

inline irr::gui::IGUIEnvironment* TheGame::getEnv()
{
    return env;
}

inline EventReceiver* TheGame::getEventReceiver()
{
    return eventReceiver;
}

inline const irr::core::dimension2du& TheGame::getScreenSize()
{
    return driver->getScreenSize();
}

inline unsigned int TheGame::getTick()
{
    return tick;
}

inline bool TheGame::isFpsCamera()
{
    return camera == fps_camera;
}

//inline Shaders* TheGame::getShaders()
//{
//    return shaders;
//}

inline bool TheGame::getInGame()
{
    return inGame;
}

inline void TheGame::setInGame(bool val)
{
    inGame = val;
}

inline bool TheGame::getEditorMode()
{
    return editorMode;
}

inline void TheGame::setEditorMode(bool val)
{
    editorMode = val;
}

#endif // THEGAME_H
