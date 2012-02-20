
#ifndef STAGE_H
#define STAGE_H

#include <string>
#include <map>
#include <irrlicht.h>
#include "RaceManager.h"
#include "RoadManager.h"
#include "ItinerManager.h"
#include "AIPoint.h"
#include "WayPointManager.h"
#include "Settings.h"

// use in RaceEngine.cpp when calculate AI speed
#define DIFFICULTY_SPEED_STEP_DIVIDER   24.f

// use calculate stage time
//#define DIFFICULTY_SPEED_STEP           7   // 180 km/h / DIFFICULTY_SPEED_STEP_DIVIDER

class Stage
{
public:
    Stage(Day* parent, const std::string& raceName, const std::string& dayName, const std::string& stageName, bool& ret);
    ~Stage();
    
private:
    // called in the constructior
    bool readHeader();
    
    // EXTERNAL, called in the stage menu, need some additional data
    bool readPreData();
    
    // called in the activate
    bool readData();

    // called by readHeader()
    bool readCfg();
    void readShortDescription();

    // called by readPreData()
    void readItinerPointList();
    void readWayPointList();
    void readAIPointList();

    // called by readData()
    void readGlobalObjects();
    void readHeightModifierList();

    bool write();
    bool writeCfg();
    bool writeShortDescription();
    bool writeGlobalObjects();
    bool writeItinerPointList();
    bool writeWayPointList();
    bool writeAIPointList();
    bool writeHeightModifierList();

public:
    const std::string& getName() const; // inline
    const std::string& getLongName() const; // inline
    const std::string& getDssAss() const; // inline
    const std::string& getShortDescription() const; // inline
    const RoadManager::roadMap_t& getRoadMap() const; // inline
    const ItinerManager::itinerPointList_t& getItinerPointList() const; // inline
    const WayPointManager::wayPointList_t& getWayPointList() const; // inline
    const AIPoint::AIPointList_t& getAIPointList() const; // inline
    Day* getParent() const; // inline
    unsigned int getStageTime() const; // inline
    irr::video::ITexture* getImage(); // inline
    const heightModifierList_t& getHeightModifierList() const; // inline
    float getGroundFriction() const; // inline

    void activate();
    void deactivate();

    bool updateStageTimeAndWriteAIPoints();

private:
    Day*                            parent;
    std::string                     raceName;
    std::string                     dayName;
    std::string                     stageName;
    std::string                     stageLongName;
    std::string                     dssAssName;
    std::string                     shortDescription;
    std::string                     imageName;
    RaceManager::globalObjectList_t globalObjectList;
    ItinerManager::itinerPointList_t itinerPointList;
    WayPointManager::wayPointList_t wayPointList;
    AIPoint::AIPointList_t          AIPointList;
    bool                            active;
    RoadManager::roadMap_t          roadMap;
    unsigned int                    stageTime;
    irr::video::ITexture*           image;
    float                           groundFriction;
    heightModifierList_t            heightModifierList;
    bool                            loaded;
    bool                            preLoaded;
    
    
    HeightModifier                  editorHeightModifier;
    

    friend class RaceManager;
    friend class GamePlay;
    friend class MenuPageEditor;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
    friend class MenuPageStage;
};

inline const std::string& Stage::getName() const
{
    return stageName;
}

inline const std::string& Stage::getLongName() const
{
    return stageLongName;
}

inline const std::string& Stage::getDssAss() const
{
    return dssAssName;
}

inline const std::string& Stage::getShortDescription() const
{
    return shortDescription;
}

inline const RoadManager::roadMap_t& Stage::getRoadMap() const
{
    return roadMap;
}

inline const ItinerManager::itinerPointList_t& Stage::getItinerPointList() const
{
    return itinerPointList;
}

inline const WayPointManager::wayPointList_t& Stage::getWayPointList() const
{
    return wayPointList;
}

inline const AIPoint::AIPointList_t& Stage::getAIPointList() const
{
    return AIPointList;
}

inline Day* Stage::getParent() const
{
    return parent;
}

inline unsigned int Stage::getStageTime() const
{
    if (!AIPointList.empty() && stageTime)
    {
        const float stageSpeed = (AIPointList.back()->getGlobalDistance()/(float)stageTime);
        const float stageSpeedStep = stageSpeed / DIFFICULTY_SPEED_STEP_DIVIDER;
        const float desiredSpeed = stageSpeed + stageSpeedStep - (stageSpeedStep*(float)Settings::getInstance()->difficulty);
        if (desiredSpeed < 0.0001f) return stageTime;
        return (unsigned int)(AIPointList.back()->getGlobalDistance()/desiredSpeed);
    }
    return stageTime;
}

inline irr::video::ITexture* Stage::getImage()
{
    return image;
}

inline const heightModifierList_t& Stage::getHeightModifierList() const
{
    return heightModifierList;
}

inline float Stage::getGroundFriction() const
{
    return groundFriction;
}

#endif // STAGE_H
