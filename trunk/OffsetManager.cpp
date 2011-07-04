#include "OffsetManager.h"
#include "OffsetObject.h"
#include "ObjectPoolManager.h"
//#include "TerrainPool.h"
//#include "gameplay.h"
#include <stdio.h>
#include <assert.h>
#include "stdafx.h"

OffsetManager* OffsetManager::offsetManager = 0;

void OffsetManager::initialize()
{
    if (offsetManager == 0)
    {
        offsetManager = new OffsetManager();
    }
}

void OffsetManager::finalize()
{
    if (offsetManager)
    {
        delete offsetManager;
        offsetManager = 0;
    }
}

OffsetManager::OffsetManager()
    : objects(), offset(), last()
{
}

OffsetManager::~OffsetManager()
{
    while (objects.size() > 0)
    {
        OffsetObject* oo = *objects.begin();
        if (oo->getPool())
        {
            ObjectPoolManager::getInstance()->putObject(oo);
        }
        else
        {
            // TODO: who will remove it?
            removeObject(oo);
        }
    }
}

void OffsetManager::addObject(OffsetObject* object)
{
    objects.push_front(object);
    object->iterator = objects.begin();
    object->update(offset, offset);
}

void OffsetManager::removeObject(OffsetObject* object)
{
    if (object->iterator != offsetObjectList_t::Iterator())
    {
        objects.erase(object->iterator);
        object->iterator = offsetObjectList_t::Iterator();
    }
}

//#warning use proper value for the offset unit
#define OFFSET_UNIT 4096.f
bool OffsetManager::update(const irr::core::vector3df& newPos, bool force)
{
    const irr::core::vector3di new_((irr::s32)(newPos.X/OFFSET_UNIT), 0, (irr::s32)(newPos.Z/OFFSET_UNIT)-1);
    
    if (last.X != new_.X || last.Z != new_.Z || force)
    {
        dprintf(MY_DEBUG_NOTE, "offset manager start update ... \n");
        irr::core::vector3df loffset((float)(new_.X-last.X)*OFFSET_UNIT, 0.f, (float)(new_.Z-last.Z)*OFFSET_UNIT);
        last.X = new_.X;
        last.Z = new_.Z;
        offset.X = (float)last.X * OFFSET_UNIT;
        offset.Z = (float)last.Z * OFFSET_UNIT;
        unsigned int updated = 0;
        for (offsetObjectList_t::Iterator it = objects.begin(); it != objects.end(); it++)
        {
            (*it)->update(offset, loffset);
            updated++;
        }
        dprintf(MY_DEBUG_NOTE, "offset manager start update ... done\n");
        return true;
    }
    else
    {
        return false;
    }
}

void OffsetManager::reset()
{
    unsigned int reseted = 0;
    dprintf(MY_DEBUG_NOTE, "OffsetManager::reset(): %u objects in the list, 0 can be no problem\n", objects.size());
    //return;
    //assert(0);
    /*
    for (offsetObjectList_t::Iterator it = objects.begin(); it != objects.end();)
    {
        delete (*it);
        it = objects.erase(it);
        reseted++;
    }
    */
    last = irr::core::vector3di();
    offset = irr::core::vector3df();
}

