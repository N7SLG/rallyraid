
#include "ObjectWire.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "ObjectPoolManager.h"
#include "ObjectPool.h"
#include "ObjectWireGlobalObject.h"
#include "Settings.h"
#include "TheEarth.h"
#include "TheGame.h"
#include "Terrain_defs.h"
#include "stdafx.h"
#include <assert.h>
#include <string.h>
#include <math.h>



ObjectWireTile::ObjectWireTile(const irr::core::vector2df& apos, const irr::core::vector2di& rpos)
    : irr::scene::ISceneNode(0, TheGame::getInstance()->getSmgr(), -1, irr::core::vector3df(apos.X, 0.f, apos.Y)),
      apos(apos),
      rpos(rpos),
      bbox(),
      offsetObject(0),
      objectList()
{
    //printf("ObjectWireTile::ObjectWireTile()\n");

    offsetObject = new OffsetObject(this);
    offsetObject->setUpdateCB(this);
    offsetObject->addToManager();

    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    const unsigned int objectWireSize = Settings::getInstance()->objectWireSize;
    const unsigned int objectDensity = Settings::getInstance()->objectDensity;
    // TODO: we need more detailed density maps.
    irr::video::SColor density = TheEarth::getInstance()->getEarthDensity(
        (unsigned int)abs((int)apos.X/TILE_SIZE), (unsigned int)abs((int)apos.Y/TILE_SIZE));

    const ObjectPoolManager::objectPoolMap_t& objectPoolMap = ObjectPoolManager::getInstance()->getObjectPoolMap();
        
    for (ObjectPoolManager::objectPoolMap_t::const_iterator opIt = objectPoolMap.begin();
            opIt != objectPoolMap.end();
            opIt++)
    {
        ObjectPool* objectPool = opIt->second;
        unsigned int num = objectPool->getNum();
        unsigned int category = objectPool->getCategory();
            
        unsigned int rep = 0;
            
        if ((category & 1) == 1)
        {
            rep += density.getRed();
        }
        if ((category & 2) == 2)
        {
            rep += density.getGreen();
        }
        if ((category & 4) == 4)
        {
            rep += density.getBlue();
        }
        if (rep > 255) rep = 255;
            
        //unsigned int cnt = (rep * num /* Settings::getInstance()->objectDensity*/) / (255/*100*/);

        //printf("%s: %u\n", objectPool->getName().c_str(), cnt);
        const unsigned int req = rep * objectDensity;

        if (req == 0) continue;
            
        for (unsigned int i = 0; i < num; i++)
        {
            irr::core::vector3df objectPos = irr::core::vector3df(
                    (float)(rand()%(objectWireSize*10)) / 10.0f + apos.X,
                    -50.f,
                    (float)(rand()%(objectWireSize*10)) / 10.0f + apos.Y);
                
            irr::video::SColor fineDensity = TheEarth::getInstance()->getTileFineDensity(
                (unsigned int)abs((int)objectPos.X/TILE_FINE_SCALE), (unsigned int)abs((int)objectPos.Z/TILE_FINE_SCALE));
                
            unsigned int newRep = 0;
                
            if ((category & 1) == 1)
            {
                newRep += fineDensity.getRed();
            }
            if ((category & 2) == 2)
            {
                newRep += fineDensity.getGreen();
            }
            if ((category & 4) == 4)
            {
                newRep += fineDensity.getBlue();
            }
            if (rep > 255) rep = 255;
                
            const unsigned int newReq = newRep * objectDensity;
            if (newReq >= 255*100 || (unsigned int)(rand()%(255*100)) < newReq)
            {
                
                objectPos.Y = TheEarth::getInstance()->getHeight(objectPos-OffsetManager::getInstance()->getOffset());
                //printf("opos: %f %f %f\n", objectPos.X, objectPos.Y, objectPos.Z);
                
                if (objectPos.Y > 10.f)
                {                
                    OffsetObject* oo = objectPool->getObject(objectPos);
                    if (oo)
                    {
                        objectList.push_back(oo);
                    }
                }
            }
        }
    }
}
    
ObjectWireTile::~ObjectWireTile()
{
    offsetObject->removeFromManager();
    delete offsetObject;
    offsetObject = 0;

    for (std::list<OffsetObject*>::iterator it = objectList.begin();
            it != objectList.end();
            it++)
    {
        ObjectPoolManager::getInstance()->putObject(*it);
    }
    objectList.clear();
}

void ObjectWireTile::handleUpdatePos(bool phys)
{
    const float objectWireSize = (float)Settings::getInstance()->objectWireSize;
    bbox.MinEdge = getPosition();
    bbox.MaxEdge = getPosition() + irr::core::vector3df(objectWireSize, 10000.f, objectWireSize);
}

void ObjectWireTile::setVisible(bool visible)
{
    if (this->IsVisible == visible) return;

    this->IsVisible = visible;

    for (std::list<OffsetObject*>::iterator it = objectList.begin();
            it != objectList.end();
            it++)
    {
        if ((*it)->getNode())
        {
            (*it)->getNode()->setVisible(visible);
        }
    }
}


ObjectWire* ObjectWire::objectWire = 0;

void ObjectWire::initialize()
{
    if (objectWire == 0)
    {
        objectWire = new ObjectWire();
    }
}

void ObjectWire::finalize()
{
    if (objectWire)
    {
        delete objectWire;
        objectWire = 0;
    }
}


ObjectWire::ObjectWire()
    : tiles(0),
      lastWireCenter(),
      globalObjectWire()
{
    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    assert(objectWireNum);
    
    tiles = new ObjectWireTile*[objectWireNum*objectWireNum];
    memset(tiles, 0, objectWireNum*objectWireNum*sizeof(ObjectWireTile*));
}

ObjectWire::~ObjectWire()
{
    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    if (tiles)
    {
        for (unsigned int x = 0; x < objectWireNum; x++)
        {
            for (unsigned int y = 0; y < objectWireNum; y++)
            {
                if (tiles[x + objectWireNum*y])
                {
                    delete tiles[x + objectWireNum*y];
                    tiles[x + objectWireNum*y] = 0;
                }
            }
        }
        delete tiles;
        tiles = 0;
    }
     // clear globals
     for (globalObjectWire_t::iterator it = globalObjectWire.begin();
          it != globalObjectWire.end();
          it++)
     {
         for (globalObjectSet_t::const_iterator oit = it->second.begin();
              oit != it->second.end();
              oit++)
         {
             delete (*oit);
         }
         it->second.clear();
     }
     globalObjectWire.clear();
}

void ObjectWire::reset()
{
    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    dprintf(MY_DEBUG_NOTE, "ObjectWire::reset()\n");
    
    for (unsigned int x = 0; x < objectWireNum; x++)
    {
        for (unsigned int y = 0; y < objectWireNum; y++)
        {
            if (tiles[x + objectWireNum*y])
            {
                delete tiles[x + objectWireNum*y];
                tiles[x + objectWireNum*y] = 0;
            }
        }
    }
    
    for (globalObjectWire_t::iterator it = globalObjectWire.begin();
        it != globalObjectWire.end();
        it++)
    {
        for (globalObjectSet_t::const_iterator oit = it->second.begin();
            oit != it->second.end();
            oit++)
        {
            (*oit)->setVisible(false);
        }
        //it->second.clear();
    }
    
    lastWireCenter = irr::core::vector2di();
}

bool ObjectWire::update(const irr::core::vector3df& newPos, bool force)
{
    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    const unsigned int objectWireSize = Settings::getInstance()->objectWireSize;
    assert(objectWireNum);

    bool ret = false;
    
    irr::core::vector2di newWireCenter = irr::core::vector2di((int)newPos.X/(int)objectWireSize, (int)newPos.Z/(int)objectWireSize);
    
    if (newWireCenter != lastWireCenter)
    {
        dprintf(MY_DEBUG_NOTE, "ObjectWire::update(): (%d, %d) -> (%d, %d)\n",
            lastWireCenter.X, lastWireCenter.Y, newWireCenter.X, newWireCenter.Y);
        irr::core::vector2di offset = newWireCenter - lastWireCenter;
        ObjectWireTile** newTiles = new ObjectWireTile*[objectWireNum*objectWireNum];
        memset(newTiles, 0, objectWireNum*objectWireNum*sizeof(ObjectWireTile*));
        
        // search for overlap in old tiles, if no overlap delete
        for (unsigned int y = 0, fy = 0; y < objectWireNum; y++, fy+=objectWireNum)
        {
            for (unsigned int x = 0; x < objectWireNum; x++)
            {
                unsigned int newX = x - offset.X;
                unsigned int newY = y + offset.Y;
                unsigned int oldTileNum = x + fy;
                if (newX < objectWireNum && newY < objectWireNum && tiles[oldTileNum/*x + objectWireNum*y*/])
                {
                    // overlap
                    newTiles[newX + objectWireNum*newY] = tiles[oldTileNum/*x + objectWireNum*y*/];
                    newTiles[newX + objectWireNum*newY]->rpos = irr::core::vector2di(newX, newY);
                }
                else
                {
                    if (tiles[oldTileNum/*x + objectWireNum*y*/])
                    {
                        delete tiles[oldTileNum/*x + objectWireNum*y*/];
                        tiles[oldTileNum/*x + objectWireNum*y*/] = 0;
                        
                        // check globals
                        globalObjectWire_t::const_iterator it = globalObjectWire.find((lastWireCenter.X-(int)(objectWireNum/2)+(int)x) +
                                               (TheEarth::getInstance()->getSizeX() * (lastWireCenter.Y+(int)(objectWireNum/2)-(int)y)));
                        if (it != globalObjectWire.end())
                        {
                            for (globalObjectSet_t::const_iterator oit = it->second.begin();
                                 oit != it->second.end();
                                 oit++)
                            {
                                (*oit)->setVisible(false);
                            }
                        }
                    }
                }
            }
        }
        
        delete [] tiles;
        tiles = newTiles;
        lastWireCenter = newWireCenter;
        
        // search for non-filed newTiles, create new
        for (unsigned int y = 0, fy = 0; y < objectWireNum; y++, fy += objectWireNum)
        {
            for (unsigned int x = 0; x < objectWireNum; x++)
            {
                unsigned int tileNum = x + fy;
                if (tiles[tileNum/*x + objectWireNum*y*/] == 0)
                {
                    tiles[tileNum/*x + objectWireNum*y*/] = new ObjectWireTile(
                        irr::core::vector2df(
                            (float)((lastWireCenter.X-(int)(objectWireNum/2)+(int)x)*(int)objectWireSize),
                            (float)((lastWireCenter.Y-1+(int)(objectWireNum/2)-(int)y)*(int)objectWireSize)),
                        irr::core::vector2di(x, y));

                    // check globals
                    globalObjectWire_t::const_iterator it = globalObjectWire.find((lastWireCenter.X-(int)(objectWireNum/2)+(int)x) +
                                           (TheEarth::getInstance()->getSizeX() * (lastWireCenter.Y+(int)(objectWireNum/2)-(int)y)));
                    //printf("check global in %d\n", (lastWireCenter.X-(int)(objectWireNum/2)+(int)x) +
                    //                       (TheEarth::getInstance()->getSizeX() * (lastWireCenter.Y+(int)(objectWireNum/2)-(int)y)));
                    if (it != globalObjectWire.end())
                    {
                        for (globalObjectSet_t::const_iterator oit = it->second.begin();
                             oit != it->second.end();
                             oit++)
                        {
                            (*oit)->setVisible(true);
                            //printf("set visible global object: %p\n", *oit);
                        }
                    }
                }
            }
        }
        ret = true;
    }

    //return ret;

    const float newDistance = (float)(objectWireNum*objectWireSize) * 0.75f;
    const float rate = newDistance / TheGame::getInstance()->getCamera()->getFarValue();
    const irr::core::vector3df& cpos = TheGame::getInstance()->getCamera()->getPosition();
    irr::core::aabbox3d<irr::f32> cbbox = TheGame::getInstance()->getCamera()->getViewFrustum()->getBoundingBox();

    // normalize
    cbbox.MinEdge = ((cbbox.MinEdge - cpos) * rate) + cpos;
    cbbox.MaxEdge = ((cbbox.MaxEdge - cpos) * rate) + cpos;
    //printf("%f, %f, %f - %f, %f, %f\n",
    //    cbbox.MinEdge.X, cbbox.MinEdge.Y, cbbox.MinEdge.Z,
    //    cbbox.MaxEdge.X, cbbox.MaxEdge.Y, cbbox.MaxEdge.Z);

    for (unsigned int y = 0, fy = 0; y < objectWireNum; y++, fy += objectWireNum)
    {
        for (unsigned int x = 0; x < objectWireNum; x++)
        {
            unsigned int tileNum = x + fy;
            
            if (cbbox.intersectsWithBox(tiles[tileNum]->getBoundingBox()))
            {
                if (!tiles[tileNum]->isVisible())
                {
                    tiles[tileNum]->setVisible(true);
                    globalObjectWire_t::const_iterator it = globalObjectWire.find((lastWireCenter.X-(int)(objectWireNum/2)+(int)x) +
                                           (TheEarth::getInstance()->getSizeX() * (lastWireCenter.Y+(int)(objectWireNum/2)-(int)y)));
                    if (it != globalObjectWire.end())
                    {
                        for (globalObjectSet_t::const_iterator oit = it->second.begin();
                             oit != it->second.end();
                             oit++)
                        {
                            (*oit)->setSoftVisible(true);
                        }
                    }
                }
            }
            else
            {
                if (tiles[tileNum]->isVisible())
                {
                    tiles[tileNum]->setVisible(false);
                    globalObjectWire_t::const_iterator it = globalObjectWire.find((lastWireCenter.X-(int)(objectWireNum/2)+(int)x) +
                                           (TheEarth::getInstance()->getSizeX() * (lastWireCenter.Y+(int)(objectWireNum/2)-(int)y)));
                    if (it != globalObjectWire.end())
                    {
                        for (globalObjectSet_t::const_iterator oit = it->second.begin();
                             oit != it->second.end();
                             oit++)
                        {
                            (*oit)->setSoftVisible(false);
                        }
                    }
                }
            }
        }
    }
    return ret;
}

ObjectWireGlobalObject* ObjectWire::addGlobalObject(const std::string& objectPoolName,
    const irr::core::vector3df& apos,
    const irr::core::vector3df& rot,
    const irr::core::vector3df& scale)
{
    ObjectPool* objectPool = 0;
    ObjectPoolManager::objectPoolMap_t::const_iterator poolIt = ObjectPoolManager::getInstance()->getObjectPoolMap().find(objectPoolName);
    if (poolIt != ObjectPoolManager::getInstance()->getObjectPoolMap().end())
    {
        objectPool = poolIt->second;
    }
    else
    {
        dprintf(MY_DEBUG_WARNING, "no object pool for %s\n", objectPoolName.c_str());
    }
    //assert(objectPool);
    ObjectWireGlobalObject* globalObject = new ObjectWireGlobalObject(objectPool, apos, rot, scale);
    //globalObjectWire[x + (TheEarth::getInstance()->getSizeX() * y)].insert(globalObject);
    //return globalObject;
    return addGlobalObject(globalObject);
}
    
ObjectWireGlobalObject* ObjectWire::addGlobalObject(ObjectWireGlobalObject* globalObject)
{
    const unsigned int objectWireNum2 = Settings::getInstance()->objectWireNum/2;
    const unsigned int objectWireSize = Settings::getInstance()->objectWireSize;
    int x = (int)globalObject->getPos().X / (int)objectWireSize;
    int y = (int)globalObject->getPos().Z / (int)objectWireSize;
    
    globalObjectWire[x + (TheEarth::getInstance()->getSizeX() * y)].insert(globalObject);
    //printf("add global object: %d - %p\n", x + (TheEarth::getInstance()->getSizeX() * y), globalObject);
    if ((unsigned int)abs(x-lastWireCenter.X) < objectWireNum2 && (unsigned int)abs(y-lastWireCenter.Y) < objectWireNum2)
    {
        globalObject->setVisible(true);
    }
    return globalObject;
}

void ObjectWire::removeGlobalObject(ObjectWireGlobalObject* globalObject, bool deleteObject)
{
    if (globalObject)
    {
        const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
        const unsigned int objectWireSize = Settings::getInstance()->objectWireSize;
        int x = (int)globalObject->apos.X / (int)objectWireSize;
        int y = (int)globalObject->apos.Z / (int)objectWireSize;
        
        
        globalObjectWire_t::iterator it = globalObjectWire.find(x + (TheEarth::getInstance()->getSizeX() * y));
        if (it != globalObjectWire.end())
        {
            it->second.erase(globalObject);
            if (it->second.empty())
            {
                globalObjectWire.erase(it);
            }
        }
        
        if (deleteObject)
        {
            delete globalObject;
        }
        else
        {
            globalObject->setVisible(false);
        }
    }
}
