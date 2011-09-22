
#include "ItinerManager.h"
#include "ItinerPoint.h"
#include <irrlicht.h>
#include <set>
#include "ConfigDirectory.h"
#include "ConfigFile.h"
#include "TheGame.h"
#include "stdafx.h"
#include "StringConverter.h"
#include "ObjectWire.h"
#include "Settings.h"
#include "Player.h"
#include "Hud.h"
#include <assert.h>


ItinerManager* ItinerManager::itinerManager = 0;

/*static */ void ItinerManager::initialize()
{
    if (itinerManager == 0)
    {
        itinerManager = new ItinerManager();
    }
}

/*static */ void ItinerManager::finalize()
{
    if (itinerManager)
    {
        delete itinerManager;
        itinerManager = 0;
    }
}

ItinerManager::ItinerManager()
    : activeItinerPointSet(),
      itinerImageMap(),
      itinerImageMap2(),
      editorGlobalDistance(0.f),
      editorLocalDistance(0.f),
      editorItinerImageName(),
      editorItinerImageName2(),
      editorDescription()
{
    readItinerImages();
    readItinerImages2();
}

ItinerManager::~ItinerManager()
{
    activeItinerPointSet.clear();
    itinerImageMap.clear();
    itinerImageMap2.clear();
}

bool ItinerManager::update(const irr::core::vector3df& newPos, bool force)
{
// activeItinerPointSet
    //printf("ass: %s, valid: %s\n", Settings::getInstance()->navigationAssistant?"true":"false", Player::getInstance()->isCurrItinerValid()?"true":"false");
    if (Settings::getInstance()->navigationAssistant && Player::getInstance()->isCurrItinerValid())
    {
        /*
        ItinerPoint* ip = *Player::getInstance()->getCurrItiner();
        const float dist = ip->getPos().getDistanceFrom(newPos);
        //printf("%f\n", dist);

        if (dist < 30.f)
        {
            dprintf(MY_DEBUG_NOTE, "step itiner by the assistant\n");
            Player::getInstance()->stepItiner();
            Player::getInstance()->resetDistance();
            Hud::getInstance()->updateRoadBook();
        }
        */
        ItinerPoint* pip = *Player::getInstance()->getCurrItiner();
        for (itinerPointSet_t::const_iterator it = activeItinerPointSet.begin();
             it != activeItinerPointSet.end() && pip;
             it++)
        {
            ItinerPoint* ip = *it;
            if (pip->getNum() <= ip->getNum())
            {
                const float dist = ip->getPos().getDistanceFrom(newPos);
                //printf("%f\n", dist);

                if (dist < 30.f)
                {
                    dprintf(MY_DEBUG_NOTE, "step itiner by the assistant\n");
                    do
                    {
                        Player::getInstance()->stepItiner();
                        if (Player::getInstance()->isCurrItinerValid())
                        {
                            pip = *Player::getInstance()->getCurrItiner();
                        }
                        else
                        {
                            pip = 0;
                        }
                    } while (pip && pip->getNum() <= ip->getNum());
                    Player::getInstance()->resetDistance();
                    Hud::getInstance()->updateRoadBook();
                }
            }
        }
    }
    return false;
}

bool ItinerManager::readItinerImages()
{
    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read itiner images directory:\n");

    bool ret = ConfigDirectory::load(ITINERIMAGES_DIR.c_str(), fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read itiner images directory\n");
        return false;
    }
    
    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string itinerImage = it->c_str();
        itinerImageMap[itinerImage] = TheGame::getInstance()->getDriver()->getTexture((ITINERIMAGES_DIR+"/"+itinerImage).c_str());
    }
    return true;
}

bool ItinerManager::readItinerImages2()
{
    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read itiner images 2 directory:\n");

    bool ret = ConfigDirectory::load(ITINERIMAGES2_DIR.c_str(), fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read itiner images 2 directory\n");
        return false;
    }
    
    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string itinerImage = it->c_str();
        itinerImageMap2[itinerImage] = TheGame::getInstance()->getDriver()->getTexture((ITINERIMAGES2_DIR+"/"+itinerImage).c_str());
    }
    return true;
}

/* static */ void ItinerManager::readItinerPointList(const std::string& itinerListFilename, ItinerManager::itinerPointList_t& itinerPointList)
{
    ConfigFile cf;
    cf.load(itinerListFilename);

    dprintf(MY_DEBUG_NOTE, "Read itiners cfg file: %s\n", itinerListFilename.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    float globalDistance = 0.f;
    unsigned int num = 1;
    std::string secName, keyName, valName;
    while (seci.hasMoreElements())
    {
        irr::core::vector3df apos;
        float localDistance = 0.f;
        std::string itinerImageName;
        std::string itinerImageName2;
        std::string description;

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
            //} else if (keyName == "gd")
            //{
            //    globalDistance = StringConverter::parseFloat(valName, 0.f);
            } else if (keyName == "ld")
            {
                localDistance = StringConverter::parseFloat(valName, 0.f);
            } else if (keyName == "image")
            {
                itinerImageName = valName;
            } else if (keyName == "image2")
            {
                itinerImageName2 = valName;
            } else if (keyName == "description")
            {
                description = valName;
            }
        }

        if (!secName.empty())
        {
            globalDistance += localDistance;
            ItinerPoint* itinerPoint = new ItinerPoint(apos, num, globalDistance, localDistance, itinerImageName, itinerImageName2, description);
            itinerPointList.push_back(itinerPoint);
            num++;
        }
    }
    //assert(0);
}

/* static */ bool ItinerManager::writeItinerPointList(const std::string& itinerListFilename, const ItinerManager::itinerPointList_t& itinerPointList)
{
    FILE* f;
    errno_t error = fopen_s(&f, itinerListFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", itinerListFilename.c_str());
        return false;
    }

    unsigned int id = 1;
    char fillZero[5] = {'0', '0', '0', '0', 0};

    assert(itinerPointList.size() < 100000);

    for (itinerPointList_t::const_iterator it = itinerPointList.begin();
         it != itinerPointList.end();
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
        //fprintf_s(f, "gd=%f\n", (*it)->getGlobalDistance());
        fprintf_s(f, "ld=%f\n", (*it)->getLocalDistance());
        fprintf_s(f, "image=%s\n", (*it)->getItinerImageName().c_str());
        fprintf_s(f, "image2=%s\n", (*it)->getItinerImageName2().c_str());
        fprintf_s(f, "description=%s\n", (*it)->getDescription().c_str());

        id++;
    }

    fclose(f);
    return true;
}

/* static */ void ItinerManager::clearItinerPointList(ItinerManager::itinerPointList_t& itinerPointList)
{
    for (itinerPointList_t::iterator it = itinerPointList.begin();
         it != itinerPointList.end();
         it++)
    {
        delete (*it);
    }
    itinerPointList.clear();
}

/* static */ void ItinerManager::addItinerPointListToObjectWire(const ItinerManager::itinerPointList_t& itinerPointList)
{
    for (itinerPointList_t::const_iterator it = itinerPointList.begin();
         it != itinerPointList.end();
         it++)
    {
        ObjectWire::getInstance()->addGlobalObject(*it);
    }
}

/* static */ void ItinerManager::removeItinerPointListFromObjectWire(const ItinerManager::itinerPointList_t& itinerPointList)
{
    for (itinerPointList_t::const_iterator it = itinerPointList.begin();
         it != itinerPointList.end();
         it++)
    {
        ObjectWire::getInstance()->removeGlobalObject(*it, false);
    }
}

/* static */ void ItinerManager::editorRenderItinerPointList(const ItinerManager::itinerPointList_t& itinerPointList)
{
    for (itinerPointList_t::const_iterator it = itinerPointList.begin();
         it != itinerPointList.end();
         it++)
    {
        (*it)->editorRender(*it == itinerPointList.back());
    }
}
