
#include "AIPoint.h"
#include "TheGame.h"
#include "OffsetManager.h"
#include <irrlicht.h>
#include "ConfigFile.h"
#include "StringConverter.h"
#include <assert.h>

AIPoint::AIPoint(const irr::core::vector3df& apos,
        float globalDistance,
        float localDistance)
    : ObjectWireGlobalObject(0, apos),
      globalDistance(globalDistance),
      localDistance(localDistance)
{
}

AIPoint::~AIPoint()
{
}

void AIPoint::editorRender(bool last)
{
    irr::video::IVideoDriver* driver = TheGame::getInstance()->getDriver();
    irr::core::vector3df renderPos = getPos() - OffsetManager::getInstance()->getOffset();

    //printf("apos (%f, %f, %f), offset: (%f, %f, %f), renderpos (%f, %f, %f)\n",
    //    apos.X, apos.Y, apos.Z,
    //    OffsetManager::getInstance()->getOffset().X, OffsetManager::getInstance()->getOffset().Y, OffsetManager::getInstance()->getOffset().Z,
    //    renderPos.X, renderPos.Y, renderPos.Z);
    //assert(0);

    irr::core::vector3df min = renderPos;
    irr::core::vector3df max = renderPos;
    irr::video::SColor color(255, 0, 0, 0);
    min.X -= 4.f;
    min.Y += 7.f;
    min.Z -= 4.f;
    max.X += 4.f;
    max.Y += 15.f;
    max.Z += 4.f;
    //printf("min: (%f, %f, %f), max: (%f, %f, %f)\n", min.X, min.Y, min.Z, max.X, max.Y, max.Z);

    if (last)
    {
        color.setRed(255);
    }

    driver->draw3DBox(irr::core::aabbox3df(min, max), color);
    //driver->draw3DLine(min, max, color);
}
/*
void AIPoint::updateVisible()
{
    if (getVisible())
    {
        AIManager::getInstance()->addActiveAIPoint(this);
    }
    else
    {
        AIManager::getInstance()->removeActiveAIPoint(this);
    }
}
*/

/* static */ void AIPoint::readAIPointList(const std::string& AIPointListFilename, AIPoint::AIPointList_t& AIPointList)
{
    ConfigFile cf;
    cf.load(AIPointListFilename);

    dprintf(MY_DEBUG_NOTE, "Read AIs cfg file: %s\n", AIPointListFilename.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    float globalDistance = 0.f;
    bool firstPoint = true;
    irr::core::vector3df lastPos;
    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        irr::core::vector3df apos;

        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tSection: %s\n", secName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valName = i->second;
            dprintf(MY_DEBUG_NOTE, "\t\tkey: %s, value: %s\n", keyName.c_str(), valName.c_str());

            if (keyName == "pos")
            {
                StringConverter::parseFloat3(valName, apos.X, apos.Y, apos.Z);
            }
        }

        if (!secName.empty())
        {
            float localDistance = 0.f;
            if (firstPoint)
            {
                firstPoint = false;
            }
            else
            {
                localDistance = lastPos.getDistanceFrom(apos);
            }
            globalDistance += localDistance;
            lastPos = apos;
            AIPoint* aiPoint = new AIPoint(apos, globalDistance, localDistance);
            AIPointList.push_back(aiPoint);
        }
    }
    //assert(0);
}

/* static */ bool AIPoint::writeAIPointList(const std::string& AIPointListFilename, const AIPoint::AIPointList_t& AIPointList)
{
    FILE* f;
    errno_t error = fopen_s(&f, AIPointListFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", AIPointListFilename.c_str());
        return false;
    }

    unsigned int id = 1;
    char fillZero[5] = {'0', '0', '0', '0', 0};

    assert(AIPointList.size() < 100000);

    for (AIPointList_t::const_iterator it = AIPointList.begin();
         it != AIPointList.end();
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

        fprintf_s(f, "pos=%f %f %f\n", (*it)->getPos().X, (*it)->getPos().Y, (*it)->getPos().Z);

        id++;
    }

    fclose(f);
    return true;
}

/* static */ void AIPoint::clearAIPointList(AIPoint::AIPointList_t& AIPointList)
{
    for (AIPointList_t::iterator it = AIPointList.begin();
         it != AIPointList.end();
         it++)
    {
        delete (*it);
    }
    AIPointList.clear();
}

/* static */ void AIPoint::editorRenderAIPointList(const AIPoint::AIPointList_t& AIPointList)
{
    for (AIPointList_t::const_iterator it = AIPointList.begin();
         it != AIPointList.end();
         it++)
    {
        (*it)->editorRender(*it == AIPointList.back());
    }
}
