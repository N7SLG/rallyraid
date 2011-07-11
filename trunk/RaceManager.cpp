
#include "RaceManager.h"
#include "Race.h"
#include "Day.h"
#include "Stage.h"
#include "stdafx.h"
#include "ConfigDirectory.h"
#include "ConfigFile.h"
#include "ObjectWireGlobalObject.h"
#include "ObjectPool.h"
#include "ObjectPoolManager.h"
#include "ObjectWire.h"
#include "StringConverter.h"
#include "RoadManager.h"


RaceManager* RaceManager::raceManager = 0;
heightModifierList_t RaceManager::emptyHeightModifierList;

void RaceManager::initialize()
{
    if (raceManager == 0)
    {
        raceManager = new RaceManager();
    }
}

void RaceManager::finalize()
{
    if (raceManager)
    {
        delete raceManager;
        raceManager = 0;
    }
}


RaceManager::RaceManager()
    : raceMap(),
      currentRaceName(),
      currentDayName(),
      currentStageName(),
      currentRace(0),
      currentDay(0),
      currentStage(0),
      editorRace(0),
      editorDay(0),
      editorStage(0)
{
    read();
}

RaceManager::~RaceManager()
{
    for (raceMap_t::const_iterator rit = raceMap.begin();
         rit != raceMap.end();
         rit++)
    {
        delete rit->second;
    }
    raceMap.clear();
}

void RaceManager::read()
{
    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read races directory:\n");

    bool ret = ConfigDirectory::load(RACES_DIR.c_str(), RACE_CFG.c_str(), fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read races directory\n");
        return;
    }
    
    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string raceName = it->c_str();
        Race* race = new Race(raceName, ret);
        if (!ret)
        {
            delete race;
        }
        else
        {
            raceMap[raceName] = race;
        }
    }
}

void RaceManager::activateStage(Stage* stage, bool forceReload)
{
    if (currentStage == stage && !forceReload) return;

    RoadManager::getInstance()->clearStageRoads();

    Day* newDay = stage->parent;
    Race* newRace = stage->parent->parent;

    if (currentStage && (stage != currentStage || forceReload))
    {
        currentStage->deactivate();
    }
    currentStage = stage;
    currentStageName = currentStage->getName();

    if (currentDay && (currentDay != newDay || forceReload))
    {
        currentDay->deactivate();
    }
    currentDay = newDay;
    currentDayName = currentDay->getName();

    if (currentRace && (currentRace != newRace || forceReload))
    {
        currentRace->deactivate();
    }
    currentRace = newRace;
    currentRaceName = currentRace->getName();

    currentRace->activate();
    currentDay->activate();
    currentStage->activate();
}

const heightModifierList_t& RaceManager::getCurrentHeightModifierList()
{
    return currentStage ? currentStage->getHeightModifierList() : emptyHeightModifierList;
}

/* static */ void RaceManager::readShortDescription(const std::string& fileName, std::string& shortDescription)
{
    FILE* f;
    const unsigned int sdSize = 4096;
    char* tmp = new char[sdSize];
    memset(tmp, 0, sdSize);
    
    errno_t error = fopen_s(&f, fileName.c_str(), "rb");
    if (error)
    {
        printf("unable to open file for read %s\n", fileName.c_str());
        delete tmp;
        return;
    }

    size_t ret = fread(tmp, sizeof(char), sdSize - 1, f);
    if (ret)
    {
        shortDescription = tmp;
    }
    fclose(f);
    delete tmp;
}

/* static */ bool RaceManager::writeShortDescription(const std::string& fileName, const std::string& shortDescription)
{
    FILE* f;
    
    errno_t error = fopen_s(&f, fileName.c_str(), "wb");
    if (error)
    {
        printf("unable to open file for write %s\n", fileName.c_str());
        return false;
    }

    /*size_t ret =*/ fwrite(shortDescription.data(), sizeof(char), shortDescription.size(), f);
    /*if (ret)
    {
        shortDescription = tmp;
    }*/
    fclose(f);
    return true;
}

/* static */ void RaceManager::readGlobalObjects(const std::string& fileName, RaceManager::globalObjectList_t& globalObjectList)
{
    ConfigFile cf;
    cf.load(fileName);

    dprintf(MY_DEBUG_NOTE, "Read global objects cfg file: %s\n", fileName.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        std::string objectPoolName;
        irr::core::vector3df apos;
        irr::core::vector3df rot;
        irr::core::vector3df scale(1.0f, 1.0f, 1.0f);

        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tSection: %s\n", secName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valName = i->second;
            dprintf(MY_DEBUG_NOTE, "\t\tkey: %s, value: %s\n", keyName.c_str(), valName.c_str());

            if (keyName == "type")
            {
                objectPoolName = valName;
            } else if (keyName == "pos")
            {
                StringConverter::parseFloat3(valName, apos.X, apos.Y, apos.Z);
            } else if (keyName == "rot")
            {
                StringConverter::parseFloat3(valName, rot.X, rot.Y, rot.Z);
            } else if (keyName == "scale")
            {
                StringConverter::parseFloat3(valName, scale.X, scale.Y, scale.Z);
            }
        }

        ObjectPool* objectPool = 0;
        ObjectPoolManager::objectPoolMap_t::const_iterator poolIt = ObjectPoolManager::getInstance()->getObjectPoolMap().find(objectPoolName);
        if (poolIt != ObjectPoolManager::getInstance()->getObjectPoolMap().end())
        {
            objectPool = poolIt->second;
        }

        if (objectPool)
        {
            ObjectWireGlobalObject* globalObject = new ObjectWireGlobalObject(objectPool, apos, rot, scale);
            globalObjectList.push_back(globalObject);
        }
    }
}

/* static */ bool RaceManager::writeGlobalObjects(const std::string& fileName, const RaceManager::globalObjectList_t& globalObjectList)
{
    FILE* f;
    errno_t error = fopen_s(&f, fileName.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", fileName.c_str());
        return false;
    }

    unsigned int id = 1;
    char fillZero[5] = {'0', '0', '0', '0', 0};

    assert(globalObjectList.size() < 100000);

    for (globalObjectList_t::const_iterator it = globalObjectList.begin();
         it != globalObjectList.end();
         it++)
    {
        if (id != 1)
        {
            fprintf_s(f, "\n");
        }
        if (id < 10)
        {
            fillZero[4] = 0;
        }
        else if (id < 100)
        {
            fillZero[3] = 0;
        }
        else if (id < 1000)
        {
            fillZero[2] = 0;
        }
        else if (id < 10000)
        {
            fillZero[1] = 0;
        }
        else
        {
            fillZero[0] = 0;
        }
        fprintf_s(f, "[%s%u]\n", fillZero, id);

        fprintf_s(f, "type=%s\n", (*it)->getObjectPool()->getName().c_str());
        fprintf_s(f, "pos=%f %f %f\n", (*it)->getPos().X, (*it)->getPos().Y, (*it)->getPos().Z);
        fprintf_s(f, "rot=%f %f %f\n", (*it)->getRot().X, (*it)->getRot().Y, (*it)->getRot().Z);
        fprintf_s(f, "scale=%f %f %f\n", (*it)->getScale().X, (*it)->getScale().Y, (*it)->getScale().Z);

        id++;
    }

    fclose(f);
    return true;
}

/* static */ void RaceManager::clearGlobalObjects(RaceManager::globalObjectList_t& globalObjectList)
{
    for (globalObjectList_t::iterator it = globalObjectList.begin();
         it != globalObjectList.end();
         it++)
    {
        delete (*it);
    }
    globalObjectList.clear();
}

/* static */ void RaceManager::addGlobalObjectsToObjectWire(const RaceManager::globalObjectList_t& globalObjectList)
{
    for (globalObjectList_t::const_iterator it = globalObjectList.begin();
         it != globalObjectList.end();
         it++)
    {
        ObjectWire::getInstance()->addGlobalObject(*it);
    }
}

/* static */ void RaceManager::removeGlobalObjectsFromObjectWire(const RaceManager::globalObjectList_t& globalObjectList)
{
    for (globalObjectList_t::const_iterator it = globalObjectList.begin();
         it != globalObjectList.end();
         it++)
    {
        ObjectWire::getInstance()->removeGlobalObject(*it, false);
    }
}

/* static */ void RaceManager::editorRenderObjects(const RaceManager::globalObjectList_t& globalObjectList)
{
    for (globalObjectList_t::const_iterator it = globalObjectList.begin();
         it != globalObjectList.end();
         it++)
    {
        (*it)->editorRender(*it == globalObjectList.back());
    }
}

/* static */ void RaceManager::readHeightModifierList(const std::string& fileName, heightModifierList_t& heightModifierList)
{
    FILE* f;
    int ret = 0;
    float f1, f2, f3;
    float radius;

    if (!heightModifierList.empty())
    {
        dprintf(MY_DEBUG_WARNING, "height modifiers already exists for %s\n", fileName.c_str());
        return;
    }

    errno_t error = fopen_s(&f, fileName.c_str(), "r");
    if (error)
    {
        printf("height modifiers file unable to open: %s\n", fileName.c_str());
        return;
    }
    
    while (true)
    {
        ret = fscanf_s(f, "%f %f %f %f\n", &f1, &f2, &f3, &radius);
        if (ret < 4)
        {
            break;
        }
        HeightModifier hm;
        hm.pos = irr::core::vector3df(f1, f2, f3);
        hm.radius = radius;
        heightModifierList.push_back(hm);
    }
    fclose(f);
}

/* static */ bool RaceManager::writeHeightModifierList(const std::string& fileName, const heightModifierList_t& heightModifierList)
{
    FILE* f;
    int ret = 0;
    errno_t error = fopen_s(&f, fileName.c_str(), "w");
    if (error)
    {
        printf("unable to open height modifiers file for write %s\n", fileName.c_str());
        return false;
    }

    for (heightModifierList_t::const_iterator it = heightModifierList.begin(); 
         it != heightModifierList.end(); it++)
    {
        ret = fprintf(f, "%f %f %f %f\n", it->pos.X, it->pos.Y, it->pos.Z, it->radius);
    }

    fclose(f);
    return true;
}

/* static */ Stage* RaceManager::getNextStage(Race* race, Stage* stage)
{
    if (stage==0 || race == 0) return 0;
    
    bool found = false;
    Race::dayMap_t::const_iterator dit = race->getDayMap().begin();
    Day::stageMap_t::const_iterator sit;
    for (; dit != race->getDayMap().end(); dit++)
    {
        for (sit = dit->second->getStageMap().begin(); sit != dit->second->getStageMap().end(); sit++)
        {
            if (sit->second == stage)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }
    
    if (found)
    {
        sit++;
        if (sit != dit->second->getStageMap().end())
        {
            return sit->second;
        }
        // stage end check the next day
        dit++;
        if (dit != race->getDayMap().end())
        {
            if (!dit->second->getStageMap().empty())
            {
                return dit->second->getStageMap().begin()->second;
            }
        }
    }
    return 0;
}
