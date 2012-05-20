
#include "Race.h"
#include "Day.h"
#include "Stage.h"
#include "ConfigFile.h"
#include "stdafx.h"
#include "StringConverter.h"
#include "TheGame.h"


Stage::Stage(Day* parent, const std::string& raceName, const std::string& dayName, const std::string& stageName, bool& ret)
    : parent(parent),
      raceName(raceName),
      dayName(dayName),
      stageName(stageName),
      stageLongName(),
      dssAssName(),
      shortDescription(),
      imageName(STAGE_DIR(raceName, dayName, stageName)+"/"+DEFAULTSTAGE_IMG),
      globalObjectList(),
      itinerPointList(),
      wayPointList(),
      AIPointList(),
      active(false),
      roadMap(),
      stageTime(10),
      image(0),
      groundFriction(0.8f),
      heightModifierList(),
      loaded(false),
      preLoaded(false),
      editorHeightModifier()
{
    ret = readHeader();
}

Stage::~Stage()
{
    deactivate();
    RaceManager::clearGlobalObjects(globalObjectList);
    RoadManager::clearRoadMap(roadMap);
    ItinerManager::clearItinerPointList(itinerPointList);
    WayPointManager::clearWayPointList(wayPointList);
    AIPoint::clearAIPointList(AIPointList);
    heightModifierList.clear();
}

bool Stage::readHeader()
{
    bool ret = readCfg();
    if (ret)
    {
        readShortDescription();
    }
    return ret;
}

bool Stage::readPreData()
{
    if (preLoaded) return true;
    
    preLoaded = true;
    
    readItinerPointList();
    readWayPointList();
    readAIPointList();
    
    return true;
}

bool Stage::readData()
{
    if (loaded) return true;
    
    loaded = true;


    readGlobalObjects();
    //readItinerPointList();
    //readWayPointList();
    //readAIPointList();
    readHeightModifierList();
    RoadManager::readRoads(STAGE_ROADS(raceName, dayName, stageName), roadMap, false, true);

    return true;
}

bool Stage::readCfg()
{
    ConfigFile cf;
    cf.load(STAGE_DIR(raceName, dayName, stageName)+"/"+STAGE_CFG);

    dprintf(MY_DEBUG_NOTE, "Read stage cfg file: %s / %s / %s\n", raceName.c_str(), dayName.c_str(), stageName.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tSection s: %s\n", secName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valName = i->second;
            dprintf(MY_DEBUG_NOTE, "\t\tkey: %s, value: %s\n", keyName.c_str(), valName.c_str());

            if (keyName == "long_name")
            {
                stageLongName = valName;
            } else if (keyName == "stage_time")
            {
                stageTime = StringConverter::parseUnsignedInt(valName, 10);
            } else if (keyName == "image")
            {
                imageName = valName;
            } else if (keyName == "dss_ass")
            {
                dssAssName = valName;
            } else if (keyName == "ground_friction")
            {
                groundFriction = StringConverter::parseFloat(valName, 0.8f);
            }
        }
    }
    image = TheGame::getInstance()->getDriver()->getTexture(imageName.c_str());
    return true;
}

void Stage::readShortDescription()
{
    RaceManager::readShortDescription(STAGE_DIR(raceName, dayName, stageName) + "/" + DESCRIPTION_TXT, shortDescription);
}

void Stage::readGlobalObjects()
{
    RaceManager::readGlobalObjects(STAGE_DIR(raceName, dayName, stageName) + "/" + OBJECTS_CFG, globalObjectList);
}

void Stage::readItinerPointList()
{
    ItinerManager::readItinerPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + ITINER_CFG, itinerPointList);
}

void Stage::readWayPointList()
{
    WayPointManager::readWayPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + WAYPOINT_CFG, wayPointList);
}

void Stage::readAIPointList()
{
    AIPoint::readAIPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + AIPOINTS_CFG, AIPointList);
    if (!AIPointList.empty() && AIPointList.back()->getTime() != 0) stageTime = AIPointList.back()->getTime();
}

void Stage::readHeightModifierList()
{
    RaceManager::readHeightModifierList(STAGE_DIR(raceName, dayName, stageName) + "/" + HEIGHTMODIFIERS_CFG, heightModifierList);
}

bool Stage::write()
{
    if (!preLoaded) readPreData();
    if (!loaded) readData();

    if (!AIPointList.empty() && AIPointList.back()->getTime() != 0) stageTime = AIPointList.back()->getTime();

    bool ret = writeCfg();
    ret &= writeShortDescription();
    ret &= writeGlobalObjects();
    ret &= writeItinerPointList();
    ret &= writeWayPointList();
    ret &= writeAIPointList();
    ret &= writeHeightModifierList();
    return ret;
}

bool Stage::updateStageTimeAndWriteAIPoints()
{
    assert(preLoaded);

    if (AIPointList.empty() || AIPointList.back()->getTime() == 0) return false;

    stageTime = AIPointList.back()->getTime();

    bool ret = writeCfg();
    ret &= writeAIPointList();
    return ret;
}

bool Stage::writeCfg()
{
    FILE* f;
    int ret = 0;
    std::string fileName = STAGE_DIR(raceName, dayName, stageName)+"/"+STAGE_CFG;
    errno_t error = fopen_s(&f, fileName.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", fileName.c_str());
        return false;
    }

    ret = fprintf(f, "long_name=%s\n", stageLongName.c_str());
    ret = fprintf(f, "stage_time=%u\n", stageTime);
    ret = fprintf(f, "image=%s\n", imageName.c_str());
    ret = fprintf(f, "dss_ass=%s\n", dssAssName.c_str());
    ret = fprintf(f, "ground_friction=%f\n", groundFriction);

    fclose(f);
    return true;
}

bool Stage::writeShortDescription()
{
    return RaceManager::writeShortDescription(STAGE_DIR(raceName, dayName, stageName) + "/" + DESCRIPTION_TXT, shortDescription);
}

bool Stage::writeGlobalObjects()
{
    return RaceManager::writeGlobalObjects(STAGE_DIR(raceName, dayName, stageName) + "/" + OBJECTS_CFG, globalObjectList);
}

bool Stage::writeItinerPointList()
{
    return ItinerManager::writeItinerPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + ITINER_CFG, itinerPointList);
}

bool Stage::writeWayPointList()
{
    return WayPointManager::writeWayPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + WAYPOINT_CFG, wayPointList);
}

bool Stage::writeAIPointList()
{
    return AIPoint::writeAIPointList(STAGE_DIR(raceName, dayName, stageName) + "/" + AIPOINTS_CFG, AIPointList);
}

bool Stage::writeHeightModifierList()
{
    return RaceManager::writeHeightModifierList(STAGE_DIR(raceName, dayName, stageName) + "/" + HEIGHTMODIFIERS_CFG, heightModifierList);
}

void Stage::activate()
{
    if (!preLoaded) readPreData();
    if (!loaded) readData();

    RoadManager::getInstance()->addStageRoad(roadMap);
    
    if (active) return;

    RaceManager::addGlobalObjectsToObjectWire(globalObjectList);
    ItinerManager::addItinerPointListToObjectWire(itinerPointList);
    WayPointManager::addWayPointListToObjectWire(wayPointList);
    active = true;
}

void Stage::deactivate()
{
    if (!active) return;

    RaceManager::removeGlobalObjectsFromObjectWire(globalObjectList);
    ItinerManager::removeItinerPointListFromObjectWire(itinerPointList);
    WayPointManager::removeWayPointListFromObjectWire(wayPointList);
    active = false;
}
