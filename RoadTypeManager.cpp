
#include "RoadTypeManager.h"
#include "RoadType.h"
#include "stdafx.h"


RoadTypeManager* RoadTypeManager::roadTypeManager = 0;

void RoadTypeManager::initialize()
{
    if (roadTypeManager == 0)
    {
        roadTypeManager = new RoadTypeManager();
    }
}

void RoadTypeManager::finalize()
{
    if (roadTypeManager)
    {
        delete roadTypeManager;
        roadTypeManager = 0;
    }
}


RoadTypeManager::RoadTypeManager()
    : roadTypeMap(),
      editorRoadType(0)
{
    read();
}

RoadTypeManager::~RoadTypeManager()
{
    for (roadTypeMap_t::const_iterator rit = roadTypeMap.begin();
         rit != roadTypeMap.end();
         rit++)
    {
        delete rit->second;
    }
    roadTypeMap.clear();
}

void RoadTypeManager::read()
{
    FILE* f;
    int ret = 0;
    int numOfRoads = 0;

    dprintf(MY_DEBUG_INFO, "Read road types:\n");

    errno_t error = fopen_s(&f, "data/roads/types/road_types.txt", "r");
    if (error)
    {
        printf("road types file unable to open\n");
        return;
    }
    
    while (true)
    {
        bool ret = false;
        RoadType* roadType = new RoadType(f, ret);
        if (ret)
        {
            roadTypeMap[roadType->getName()] = roadType;
        }
        else
        {
            delete roadType;
            fclose(f);
            return;
        }
    }
    
    fclose(f);
}
