
#ifndef DAY_H
#define DAY_H

#include <string>
#include <map>
#include <irrlicht.h>
#include "RaceManager.h"
#include "RoadManager.h"


class Stage;

class Day
{
public:
    Day(Race* parent, const std::string& raceName, const std::string& dayName, bool& ret);
    ~Day();
    
    typedef std::map<std::string, Stage*> stageMap_t;

private:
    bool readHeader();
    bool readData();
    
    bool readCfg();
    bool readStages();
    void readShortDescription();
    
    void readGlobalObjects();

    bool write();
    bool writeCfg();
    bool writeShortDescription();
    bool writeGlobalObjects();

public:
    Stage* getStage(const std::string& stageName) const; // inline
    const stageMap_t& getStageMap() const; // inline
    const std::string& getName() const; // inline
    const std::string& getLongName() const; // inline
    const std::string& getShortDescription() const; // inline
    const RoadManager::roadMap_t& getRoadMap() const; // inline
    Race* getParent() const; // inline

    void activate();
    void deactivate();

private:
    Race*                           parent;
    std::string                     raceName;
    std::string                     dayName;
    std::string                     dayLongName;
    std::string                     shortDescription;
    stageMap_t                      stageMap;
    RaceManager::globalObjectList_t globalObjectList;
    bool                            active;
    RoadManager::roadMap_t          roadMap;
    bool                            loaded;
    

    friend class RaceManager;
    friend class MenuPageEditor;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
};


inline Stage* Day::getStage(const std::string& stageName) const
{
    stageMap_t::const_iterator sit = stageMap.find(stageName);
    if (sit == stageMap.end())
    {
        return 0;
    }
    return sit->second;
}

inline const Day::stageMap_t& Day::getStageMap() const
{
    return stageMap;
}

inline const std::string& Day::getName() const
{
    return dayName;
}

inline const std::string& Day::getLongName() const
{
    return dayLongName;
}

inline const std::string& Day::getShortDescription() const
{
    return shortDescription;
}

inline const RoadManager::roadMap_t& Day::getRoadMap() const
{
    return roadMap;
}

inline Race* Day::getParent() const
{
    return parent;
}

#endif // DAY_H
