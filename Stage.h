
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


class Stage
{
public:
    Stage(Day* parent, const std::string& raceName, const std::string& dayName, const std::string& stageName, bool& ret);
    ~Stage();
    
private:
    bool read();
    bool readCfg();
    void readShortDescription();
    void readGlobalObjects();
    void readItinerPointList();
    void readWayPointList();
    void readAIPointList();
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
    const std::string& getShortDescription() const; // inline
    const RoadManager::roadMap_t& getRoadMap() const; // inline
    const ItinerManager::itinerPointList_t& getItinerPointList() const; // inline
    const WayPointManager::wayPointList_t& getWayPointList() const; // inline
    const AIPoint::AIPointList_t& getAIPointList() const; // inline
    Day* getParent() const; // inline
    unsigned int getStageTime(); // inline
    irr::video::ITexture* getImage(); // inline
    const heightModifierList_t& getHeightModifierList(); // inline

    void activate();
    void deactivate();

private:
    Day*                            parent;
    std::string                     raceName;
    std::string                     dayName;
    std::string                     stageName;
    std::string                     stageLongName;
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
    heightModifierList_t            heightModifierList;
    
    
    HeightModifier                  editorHeightModifier;
    

    friend class RaceManager;
    friend class MenuPageEditor;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
};

inline const std::string& Stage::getName() const
{
    return stageName;
}

inline const std::string& Stage::getLongName() const
{
    return stageLongName;
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

inline unsigned int Stage::getStageTime()
{
    return stageTime;
}

inline irr::video::ITexture* Stage::getImage()
{
    return image;
}

inline const heightModifierList_t& Stage::getHeightModifierList()
{
    return heightModifierList;
}

#endif // STAGE_H
