
#include "Race.h"
#include "Day.h"
#include "Competitor.h"
#include "ConfigDirectory.h"
#include "ConfigFile.h"
#include "stdafx.h"
#include "StringConverter.h"

Race::Race(const std::string& raceName, bool& ret)
    : raceName(raceName),
      raceLongName(),
      shortDescription(),
      dayMap(),
      competitorMap(),
      globalObjectList(),
      active(false),
      roadMap(),
      loaded(false),
      preLoaded(false)
{
    ret = readHeader();
}

Race::~Race()
{
    for (dayMap_t::const_iterator dit = dayMap.begin();
         dit != dayMap.end();
         dit++)
    {
        delete dit->second;
    }
    dayMap.clear();

    for (competitorMap_t::const_iterator cit = competitorMap.begin();
         cit != competitorMap.end();
         cit++)
    {
        delete cit->second;
    }
    competitorMap.clear();

    deactivate();
    RaceManager::clearGlobalObjects(globalObjectList);
    RoadManager::clearRoadMap(roadMap);
}
    
bool Race::readHeader()
{
    bool ret = readCfg();
    if (ret)
    {
        ret = readDays();
        if (ret)
        {
            readShortDescription();
        }
    }
    return ret;
}

bool Race::readPreData()
{
    if (preLoaded) return true;
    
    preLoaded = true;
    
    return readCompetitors();
}

bool Race::readData()
{
    if (loaded) return true;
    
    loaded = true;
    
    readGlobalObjects();
    RoadManager::readRoads(RACE_ROADS(raceName), roadMap, false, true);

    return true;
}

bool Race::readCfg()
{
    ConfigFile cf;
    cf.load(RACE_DIR(raceName)+"/"+RACE_CFG);

    dprintf(MY_DEBUG_NOTE, "Read race cfg file: %s\n", raceName.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tSection r: %s\n", secName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valName = i->second;
            dprintf(MY_DEBUG_NOTE, "\t\tkey: %s, value: %s\n", keyName.c_str(), valName.c_str());

            if (keyName == "long_name")
            {
                raceLongName = valName;
            }/* else if (keyName == "cache_objects")
            {
                cacheObjects = StringConverter::parseBool(valName, true);
            }*/
        }
    }
    return true;
}

bool Race::readDays()
{
    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read race (%s) directory:\n", raceName.c_str());

    bool ret = ConfigDirectory::load(RACE_DIR(raceName).c_str(), DAY_CFG.c_str(), fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read race (%s) directory\n", raceName.c_str());
        return false;
    }
    
    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string dayName = it->c_str();
        Day* day = new Day(this, raceName, dayName, ret);
        if (!ret)
        {
            delete day;
        }
        else
        {
            dayMap[dayName] = day;
        }
    }
    return true;
}

void Race::readShortDescription()
{
    RaceManager::readShortDescription(RACE_DIR(raceName) + "/" + DESCRIPTION_TXT, shortDescription);
}

bool Race::readCompetitors()
{
    ConfigFile cf;
    cf.load(RACE_DIR(raceName)+"/"+COMPETITORS_CFG);

    dprintf(MY_DEBUG_NOTE, "Read race-competitors cfg file: %s\n", raceName.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        unsigned int num = 0;
        std::string name;
        std::string coName;
        std::string teamName;
        std::string vehicleTypeName;
        unsigned int strength = 0;

        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tSection c: %s\n", secName.c_str());

        num = StringConverter::parseUnsignedInt(secName, 0);

        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;

        if (num > 0)
        {
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                keyName = i->first;
                valName = i->second;
                dprintf(MY_DEBUG_NOTE, "\t\tkey: %s, value: %s\n", keyName.c_str(), valName.c_str());

                if (keyName == "name")
                {
                    name = valName;
                } else if (keyName == "co_name")
                {
                    coName = valName;
                } else if (keyName == "team_name")
                {
                    teamName = valName;
                } else if (keyName == "vehicle_type")
                {
                    vehicleTypeName = valName;
                } else if (keyName == "strength")
                {
                    strength = StringConverter::parseUnsignedInt(valName, 0);
                }
            }
            if (strength > 0)
            {
                Competitor* c = new Competitor(num, name, coName, teamName, vehicleTypeName, strength);
                competitorMap[num] = c;
            }
        }
    }
    return true;
}

void Race::readGlobalObjects()
{
    RaceManager::readGlobalObjects(RACE_DIR(raceName) + "/" + OBJECTS_CFG, globalObjectList);
}

bool Race::write()
{
    if (!preLoaded) readPreData();
    if (!loaded) readData();
    
    bool ret = writeCfg();
    ret &= writeShortDescription();
    ret &= writeGlobalObjects();
    return ret;
}

bool Race::writeCfg()
{
    FILE* f;
    int ret = 0;
    std::string fileName = RACE_DIR(raceName)+"/"+RACE_CFG;
    errno_t error = fopen_s(&f, fileName.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", fileName.c_str());
        return false;
    }

    ret = fprintf(f, "long_name=%s\n", raceLongName.c_str());

    fclose(f);
    return true;
}

bool Race::writeShortDescription()
{
    return RaceManager::writeShortDescription(RACE_DIR(raceName) + "/" + DESCRIPTION_TXT, shortDescription);
}

bool Race::writeGlobalObjects()
{
    return RaceManager::writeGlobalObjects(RACE_DIR(raceName) + "/" + OBJECTS_CFG, globalObjectList);
}


void Race::activate()
{
    if (!preLoaded) readPreData();
    if (!loaded) readData();

    RoadManager::getInstance()->addStageRoad(roadMap);
    
    if (active) return;

    RaceManager::addGlobalObjectsToObjectWire(globalObjectList);
    active = true;
}

void Race::deactivate()
{
    if (!active) return;

    RaceManager::removeGlobalObjectsFromObjectWire(globalObjectList);
    active = false;
}
