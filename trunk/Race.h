
#ifndef RACE_H
#define RACE_H

#include <string>
#include <map>
#include <irrlicht.h>
#include "RaceManager.h"
#include "RoadManager.h"


class Day;
class Competitor;

class Race
{
public:
    Race(const std::string& raceName, bool& ret);
    ~Race();
    
    typedef std::map<std::string, Day*> dayMap_t;
    typedef std::map<unsigned int, Competitor*> competitorMap_t;

private:
    bool readHeader();
    bool readPreData();
    bool readData();
    
    bool readCfg();
    bool readDays();
    void readShortDescription();
    
    bool readCompetitors();
    void readGlobalObjects();
    
    bool write();
    bool writeCfg();
    bool writeShortDescription();
    bool writeGlobalObjects();

public:
    Day* getDay(const std::string& dayName) const; // inline
    const dayMap_t& getDayMap() const; // inline
    const std::string& getName() const; // inline
    const std::string& getLongName() const; // inline
    const std::string& getShortDescription() const; // inline
    const competitorMap_t& getCompetitorMap() const; // inline
    Competitor* getCompetitor(unsigned int compNum) const; // inline
    const RoadManager::roadMap_t& getRoadMap() const; // inline
    irr::video::ITexture* getImage(); // inline

    void activate();
    void deactivate();

private:
    std::string                     raceName;
    std::string                     raceLongName;
    std::string                     shortDescription;
    std::string                     imageName;
    dayMap_t                        dayMap;
    competitorMap_t                 competitorMap;
    RaceManager::globalObjectList_t globalObjectList;
    bool                            active;
    RoadManager::roadMap_t          roadMap;
    bool                            loaded;
    bool                            preLoaded;
    irr::video::ITexture*           image;

    
    friend class RaceManager;
    friend class GamePlay;
    friend class MenuPageEditor;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
    friend class MenuPageStage;
};


inline Day* Race::getDay(const std::string& dayName) const
{
    dayMap_t::const_iterator dit = dayMap.find(dayName);
    if (dit == dayMap.end())
    {
        return 0;
    }
    return dit->second;
}

inline const Race::dayMap_t& Race::getDayMap() const
{
    return dayMap;
}

inline const std::string& Race::getName() const
{
    return raceName;
}

inline const std::string& Race::getLongName() const
{
    return raceLongName;
}

inline const std::string& Race::getShortDescription() const
{
    return shortDescription;
}

inline const Race::competitorMap_t& Race::getCompetitorMap() const
{
    return competitorMap;
}

inline Competitor* Race::getCompetitor(unsigned int compNum) const
{
    competitorMap_t::const_iterator it = competitorMap.find(compNum);
    if (it == competitorMap.end())
    {
        return 0;
    }
    return it->second;
}

inline const RoadManager::roadMap_t& Race::getRoadMap() const
{
    return roadMap;
}

inline irr::video::ITexture* Race::getImage()
{
    return image;
}

#endif // RACE_H
