
#include "WayPointManager.h"
#include "WayPoint.h"
#include <irrlicht.h>
#include <set>
#include "ConfigFile.h"
#include "TheGame.h"
#include "stdafx.h"
#include "StringConverter.h"
#include "ObjectWire.h"
#include "MessageManager.h"
#include "Player.h"
#include "RaceManager.h"
#include "Stage.h"
#include <assert.h>


WayPointManager* WayPointManager::wayPointManager = 0;

/*static */ void WayPointManager::initialize()
{
    if (wayPointManager == 0)
    {
        wayPointManager = new WayPointManager();
    }
}

/*static */ void WayPointManager::finalize()
{
    if (wayPointManager)
    {
        delete wayPointManager;
        wayPointManager = 0;
    }
}

WayPointManager::WayPointManager()
    : activeWayPointSet(),
      showCompass(false),
      angle(0.f),
      editorWayPointType(WayPoint::Hidden)
{
}

WayPointManager::~WayPointManager()
{
    activeWayPointSet.clear();
}

bool WayPointManager::update(const irr::core::vector3df& newPos, bool force)
{
    showCompass = false;
    if (!activeWayPointSet.empty())
    {
        WayPoint* wp = *activeWayPointSet.begin();
        float dist = wp->getPos().getDistanceFrom(newPos);
        unsigned int wpNum = wp->getNum();

        if (dist < 800.f)
        {
            bool isPassed = Player::getInstance()->isPassedWayPointNum(wpNum);
            if (isPassed) return false;

            if (dist < 50.f)
            {
                if (Player::getInstance()->addPassedWayPointNum(wpNum))
                {
                    irr::core::stringw str;
                    str += L"You have just passed the ";
                    str += wpNum;
                    str += L". waypoint. (";
                    str += Player::getInstance()->getPassedWayPointsCount();
                    str += L" / ";
                    str += RaceManager::getInstance()->getCurrentStage()->getWayPointList().size();
                    str += L")";
                    MessageManager::getInstance()->addText(str.c_str(), 5);
                    return true;
                }
                assert(0);
            }
            else
            {
                irr::core::vector3df dir = wp->getPos() - newPos;
                angle = (float)irr::core::vector2df(dir.X, dir.Z).getAngle();
                showCompass = true;
            }
        }
    }
    return false;
}

/* static */ void WayPointManager::readWayPointList(const std::string& wayPointListFilename, WayPointManager::wayPointList_t& wayPointList)
{
    ConfigFile cf;
    cf.load(wayPointListFilename);

    dprintf(MY_DEBUG_NOTE, "Read wayPoints cfg file: %s\n", wayPointListFilename.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    unsigned int num = 1;
    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        irr::core::vector3df apos;
        unsigned int type = (unsigned int)WayPoint::Hidden;

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
            } else if (keyName == "type")
            {
                type = StringConverter::parseUnsignedInt(valName, (unsigned int)WayPoint::Hidden);
                if (type >= (unsigned int)WayPoint::NumberOfTypes) type = (unsigned int)WayPoint::Hidden;
            }
        }

        if (!secName.empty())
        {
            WayPoint* wayPoint = new WayPoint(apos, num, (WayPoint::Type)type);
            wayPointList.push_back(wayPoint);
            num++;
        }
    }
    //assert(0);
}

/* static */ bool WayPointManager::writeWayPointList(const std::string& wayPointListFilename, const WayPointManager::wayPointList_t& wayPointList)
{
    FILE* f;
    errno_t error = fopen_s(&f, wayPointListFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", wayPointListFilename.c_str());
        return false;
    }

    unsigned int id = 1;
    char fillZero[5] = {'0', '0', '0', '0', 0};

    assert(wayPointList.size() < 100000);

    for (wayPointList_t::const_iterator it = wayPointList.begin();
         it != wayPointList.end();
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
        fprintf_s(f, "type=%u\n", (*it)->getType());

        id++;
    }

    fclose(f);
    return true;
}

/* static */ void WayPointManager::clearWayPointList(WayPointManager::wayPointList_t& wayPointList)
{
    for (wayPointList_t::iterator it = wayPointList.begin();
         it != wayPointList.end();
         it++)
    {
        delete (*it);
    }
    wayPointList.clear();
}

/* static */ void WayPointManager::addWayPointListToObjectWire(const WayPointManager::wayPointList_t& wayPointList)
{
    for (wayPointList_t::const_iterator it = wayPointList.begin();
         it != wayPointList.end();
         it++)
    {
        ObjectWire::getInstance()->addGlobalObject(*it);
    }
}

/* static */ void WayPointManager::removeWayPointListFromObjectWire(const WayPointManager::wayPointList_t& wayPointList)
{
    for (wayPointList_t::const_iterator it = wayPointList.begin();
         it != wayPointList.end();
         it++)
    {
        ObjectWire::getInstance()->removeGlobalObject(*it, false);
    }
}

/* static */ void WayPointManager::editorRenderWayPointList(const WayPointManager::wayPointList_t& wayPointList)
{
    for (wayPointList_t::const_iterator it = wayPointList.begin();
         it != wayPointList.end();
         it++)
    {
        (*it)->editorRender(*it == wayPointList.back());
    }
}
