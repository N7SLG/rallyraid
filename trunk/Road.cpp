
#include "Road.h"
#include "ConfigFile.h"
#include "RoadType.h"
#include "RoadTypeManager.h"
#include "RoadManager.h"

#include "OffsetManager.h"
#include "TheEarth.h"
#include "TheGame.h"

#include "stdafx.h"
#include <assert.h>


Road::Road(const std::string& roadFilename, bool& ret, bool global, bool read)
    : roadFilename(roadFilename),
      roadName(),
      roadDataFilename(),
      roadType(0),
      loaded(false),
      global(global),
      roadPointVector(),
      firstSaved(-1),
      lastSaved(-1)
{
    ret = readHeader();
    if (ret && read)
    {
        readData();
    }
}

Road::Road(const std::string&   roadFilename,
           const std::string&   roadName,
           const std::string&   roadDataFilename,
           RoadType*            roadType,
           bool                 global)
    : roadFilename(roadFilename),
      roadName(roadName),
      roadDataFilename(roadDataFilename),
      roadType(roadType),
      global(global),
      roadPointVector(),
      firstSaved(-1),
      lastSaved(-1)
{
}

Road::~Road()
{
}

bool Road::readHeader()
{
    ConfigFile cf;
    cf.load(roadFilename);

    dprintf(MY_DEBUG_NOTE, "Read road cfg file: %s\n", roadFilename.c_str());
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

            if (keyName == "name")
            {
                roadName = valName;
            } else if (keyName == "type")
            {
                roadType = RoadTypeManager::getInstance()->getRoadType(valName);
            } else if (keyName == "data")
            {
                roadDataFilename = valName;
            }
        }
    }
    return roadType != 0 && roadName.size() > 0;
}

bool Road::writeHeader()
{
    if (roadType == 0)
    {
        printf("unable to save road cfg, no road type is given");
        return false;
    }

    FILE* f;
    int ret = 0;
    errno_t error = fopen_s(&f, roadFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open road cfg file for write %s\n", roadFilename.c_str());
        return false;
    }

    ret = fprintf(f, "name=%s\n", roadName.c_str());
    ret = fprintf(f, "type=%s\n", roadType->getName().c_str());
    ret = fprintf(f, "data=%s\n", roadDataFilename.c_str());

    fclose(f);
    return true;
}

bool Road::readData()
{
    FILE* f;
    int ret = 0;
    double f1, f2, f3;
    int radius;
    unsigned int color;
    unsigned long pointNum = 0;

    loaded = true;

    if (!roadPointVector.empty())
    {
        dprintf(MY_DEBUG_WARNING, "road data already exists for road %s\n", roadName.c_str());
        return true;
    }

    errno_t error = fopen_s(&f, roadDataFilename.c_str(), "r");
    if (error)
    {
        printf("road data file unable to open: %s\n", roadDataFilename.c_str());
        return false;
    }
    
    ret = fscanf_s(f, "%lu\n", &pointNum);
    if (ret < 1)
    {
        printf("road data unable to read number of points: %s\n", roadDataFilename.c_str());
        fclose(f);
        return false;
    }

    roadPointVector.resize(pointNum);
    unsigned int i = 0;
    while (true)
    {
        f1 = f2 = f3 = 0.0;
        ret = fscanf_s(f, "%lf %lf %lf %d %x\n", &f1, &f2, &f3, &radius, &color);
        if (ret < 5 )
        {
            break;
        }
        assert(i < pointNum);
        roadPointVector[i].p = vector3dd(f1, f2, f3);
        roadPointVector[i].radius = radius;
        roadPointVector[i].color.color = color;
        i++;
    }
    fclose(f);
    refreshFirstLast();
    return true;
}

bool Road::writeData()
{
    FILE* f;
    int ret = 0;
    errno_t error = fopen_s(&f, roadDataFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open road data file for write %s\n", roadDataFilename.c_str());
        return false;
    }

    ret = fprintf(f, "%lu\n", roadPointVector.size());
    for (unsigned int i = 0; i < roadPointVector.size(); i++)
    {
        ret = fprintf(f, "%lf %lf %lf %d %x\n",
            roadPointVector[i].p.X, roadPointVector[i].p.Y, roadPointVector[i].p.Z, roadPointVector[i].radius, roadPointVector[i].color.color);
    }
    fclose(f);
    refreshFirstLast();
    return true;
}

void Road::addRoadFarPoint(const irr::core::vector3df& pos)
{
    vector3dd apos;
    apos.X = (double)pos.X + (double)OffsetManager::getInstance()->getOffset().X;
    apos.Y = (double)pos.Y + (double)OffsetManager::getInstance()->getOffset().Y;
    apos.Z = (double)pos.Z + (double)OffsetManager::getInstance()->getOffset().Z;
    if (!roadPointVector.empty())
    {
#if 0
        irr::core::vector3df bp = roadPointVector.back().p;
        irr::core::vector3df dir = apos - bp;
        irr::core::vector3df tmpp;
        float dist = dir.getLength();
        while (dist > 4.f)
        {
            dir.setLength(4.f);
            tmpp = bp + dir;
            
            tmpp.Y = TheEarth::getInstance()->getHeight(tmpp - OffsetManager::getInstance()->getOffset());
            addRoadPoint(tmpp);
            
            bp = tmpp;
            dir = apos - bp;
            dist = dir.getLength();
        }
#else // 0 v 1
        irr::core::vector2df pos2d = irr::core::vector2df(pos.X, pos.Z);
        irr::core::vector2df bp = irr::core::vector2df((float)(roadPointVector.back().p.X-(double)OffsetManager::getInstance()->getOffset().X),
            (float)(roadPointVector.back().p.Z-(double)OffsetManager::getInstance()->getOffset().Z));
        irr::core::vector2df dir = pos2d - bp;
        irr::core::vector2df tmpp;
        vector3dd tmpp3d;
        float dist = dir.getLength();
        float cur = 0.f;
        while (cur + 6.f < dist)
        {
            cur += 4.f;
            tmpp = bp + dir*(cur/dist);
            
            tmpp3d.X = (double)tmpp.X+(double)OffsetManager::getInstance()->getOffset().X;
            tmpp3d.Z = (double)tmpp.Y+(double)OffsetManager::getInstance()->getOffset().Z;
            tmpp3d.Y = TheEarth::getInstance()->getHeight(tmpp/*3d - OffsetManager::getInstance()->getOffset()*/);
            //printf("add rp 2: %f, %f (+ %f, %f) = %f, %f\n", tmpp.X, tmpp.Y,
            //    OffsetManager::getInstance()->getOffset().X, OffsetManager::getInstance()->getOffset().Z,
            //    tmpp3d.X, tmpp3d.Z);
            addRoadPoint(tmpp3d);
        }
#endif // 0 v 1
    }
    addRoadPoint(apos);
}

void Road::addRoadFarPointBegin(const irr::core::vector3df& pos)
{
    vector3dd apos;
    apos.X = (double)pos.X + (double)OffsetManager::getInstance()->getOffset().X;
    apos.Y = (double)pos.Y + (double)OffsetManager::getInstance()->getOffset().Y;
    apos.Z = (double)pos.Z + (double)OffsetManager::getInstance()->getOffset().Z;
    if (!roadPointVector.empty())
    {
#if 0
        irr::core::vector3df bp = roadPointVector.front().p;
        irr::core::vector3df dir = apos - bp;
        irr::core::vector3df tmpp;
        float dist = dir.getLength();
        while (dist > 4.f)
        {
            dir.setLength(4.f);
            tmpp = bp + dir;
            
            tmpp.Y = TheEarth::getInstance()->getHeight(tmpp - OffsetManager::getInstance()->getOffset());
            addRoadPointBegin(tmpp);
            
            bp = tmpp;
            dir = apos - bp;
            dist = dir.getLength();
        }
#else // 0 v 1
        irr::core::vector2df pos2d = irr::core::vector2df(pos.X, pos.Z);
        irr::core::vector2df bp = irr::core::vector2df((float)(roadPointVector.front().p.X-(double)OffsetManager::getInstance()->getOffset().X),
            (float)(roadPointVector.front().p.Z-(double)OffsetManager::getInstance()->getOffset().Z));
        irr::core::vector2df dir = pos2d - bp;
        irr::core::vector2df tmpp;
        vector3dd tmpp3d;
        float dist = dir.getLength();
        float cur = 0.f;
        while (cur + 6.f < dist)
        {
            cur += 4.f;
            tmpp = bp + dir*(cur/dist);
            
            tmpp3d.X = (double)tmpp.X+(double)OffsetManager::getInstance()->getOffset().X;
            tmpp3d.Z = (double)tmpp.Y+(double)OffsetManager::getInstance()->getOffset().Z;
            tmpp3d.Y = TheEarth::getInstance()->getHeight(tmpp/*3d - OffsetManager::getInstance()->getOffset()*/);
            addRoadPointBegin(tmpp3d);
        }
#endif // 0 v 1
    }
    addRoadPointBegin(apos);
}

void Road::removeRoadPoint()
{
    if (!loaded)
    {
        readData();
    }

    if (!roadPointVector.empty())
    {
        roadPointVector.pop_back();
    }
    refreshFirstLast(0, -1);
}

void Road::removeRoadPointBegin()
{
    if (!loaded)
    {
        readData();
    }

    if (!roadPointVector.empty())
    {
        roadPointVector.erase(roadPointVector.begin());
    }
    refreshFirstLast(-1);
}


void Road::addRoadPoint(const vector3dd& pos)
{
    if (!loaded)
    {
        readData();
    }

    RoadPoint roadPoint;

    roadPoint.p = pos;
    roadPoint.color = RoadManager::getInstance()->editorColor;
    roadPoint.radius = RoadManager::getInstance()->editorRadius;

    roadPointVector.push_back(roadPoint);
}

void Road::addRoadPointBegin(const vector3dd& pos)
{
    if (!loaded)
    {
        readData();
    }

    RoadPoint roadPoint;

    roadPoint.p = pos;
    roadPoint.color = RoadManager::getInstance()->editorColor;
    roadPoint.radius = RoadManager::getInstance()->editorRadius;

    roadPointVector.insert(roadPointVector.begin(), roadPoint);
    refreshFirstLast(1);
}

void Road::editorRender(bool editorRoad)
{
    irr::video::IVideoDriver* driver = TheGame::getInstance()->getDriver();

    unsigned int step = 31;
    if (editorRoad) step = 7;
    unsigned int i = 0;
    const unsigned int lastIndex = roadPointVector.size() - 1;

    for (roadPointVector_t::iterator it = roadPointVector.begin();
         it != roadPointVector.end();
         it++)
    {
        if (i == 0 || i == lastIndex || (i & step) == 0)
        {
            irr::core::vector3df renderPos = irr::core::vector3df((float)(it->p.X - (double)OffsetManager::getInstance()->getOffset().X),
                (float)(it->p.Y - (double)OffsetManager::getInstance()->getOffset().Y),
                (float)(it->p.Z - (double)OffsetManager::getInstance()->getOffset().Z));

            irr::core::vector3df min = renderPos;
            irr::core::vector3df max = renderPos;
            irr::video::SColor color(255, 0, 0, 0);
            max.Y += 8.f;

            if (firstSaved <= (int)i && (int)i <= lastSaved)
            {
                color.setBlue(255);
                if (editorRoad) color.setRed(255);
            }
            else
            {
                if (editorRoad) color.setRed(100);
            }
            if (i == lastIndex)
            {
                color.setRed(255);
                color.setBlue(0);
            }

            driver->draw3DLine(min, max, color);
        }
        i++;
    }
}

void Road::refreshFirstLast(int firstOffset, int lastOffset)
{
    unsigned long pointNum = roadPointVector.size();
    if (firstOffset < 0)
    {
        assert(firstOffset == -1);
        assert(lastOffset == 0);
        if (lastSaved != -1)
        {
            lastSaved += firstOffset;
        }
        if (firstSaved > 0 || firstSaved > lastSaved)
        {
            firstSaved += firstOffset;
        }
    }
    else
    if (firstOffset > 0)
    {
        assert(firstOffset == 1);
        assert(lastOffset == 0);
        if (lastSaved != -1)
        {
            lastSaved += firstOffset;
        }
        if (firstSaved != -1)
        {
            firstSaved += firstOffset;
        }
    }
    else
    if (lastOffset != 0)
    {
        assert(firstOffset == 0);
        assert(lastOffset < 0);

        if (lastSaved >= (int)pointNum)
        {
            lastSaved = pointNum - 1;
        }
        if (firstSaved >= (int)pointNum)
        {
            firstSaved = pointNum - 1;
        }
    }
    else
    {
        assert(firstOffset == 0);
        assert(lastOffset == 0);
        if (pointNum > 0)
        {
            firstSaved = 0;
            lastSaved = pointNum-1;
        }
        else
        {
            firstSaved = -1;
            lastSaved = -1;
        }
    }
    dprintf(MY_DEBUG_NOTE, "refreshFirstLast: %d - %d\n", firstSaved, lastSaved);
}
