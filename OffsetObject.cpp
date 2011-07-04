
#include "OffsetObject.h"
//#include "TerrainPool.h"
//#include "gameplay.h"
#include <stdio.h>
#include "stdafx.h"

OffsetObject::dynamicObjectSet_t OffsetObject::dynamicObjectSet;
irr::core::matrix4 OffsetObject::helperMatrix;

OffsetObject::OffsetObject()
    : node(0), hkBody(0), pos(), iterator(), dynamic(false),
      offsetManager(OffsetManager::getInstance()), pool(0), updateCB(0),
      skipNodeUpdate(false)
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::OffsetObject(1): %p\n", this);
}

OffsetObject::OffsetObject(irr::scene::ISceneNode* node)
    : node(node), hkBody(0), pos(), iterator(), dynamic(false),
      offsetManager(OffsetManager::getInstance()), pool(0), updateCB(0),
      skipNodeUpdate(false)
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::OffsetObject(2): %p\n", this);
    /*
    if (node)
    {
        pos = node->getPosition();
    }
    */
}

OffsetObject::OffsetObject(irr::scene::ISceneNode* node, hkpRigidBody* hkBody)
    : node(node), hkBody(hkBody), pos(), iterator(), dynamic(false),
      offsetManager(OffsetManager::getInstance()), pool(0), updateCB(0),
      skipNodeUpdate(false)
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::OffsetObject(3): %p\n", this);
    /*
    if (node)
    {
        pos = node->getPosition();
    }
    */
}

OffsetObject::OffsetObject(irr::scene::ISceneNode* node, bool dynamic)
    : node(node), hkBody(0), pos(), iterator(), dynamic(dynamic),
      offsetManager(OffsetManager::getInstance()), pool(0), updateCB(0),
      skipNodeUpdate(false)
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::OffsetObject(4): %p\n", this);
    if (dynamic)
    {
        dynamicObjectSet.insert(this);
    }
    /*
    if (node)
    {
        pos = node->getPosition();
    }
    */
}

OffsetObject::OffsetObject(irr::scene::ISceneNode* node, hkpRigidBody* hkBody, bool dynamic)
    : node(node), hkBody(hkBody), pos(), iterator(), dynamic(dynamic),
      offsetManager(OffsetManager::getInstance()), pool(0), updateCB(0),
      skipNodeUpdate(false)
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::OffsetObject(5): %p\n", this);
    if (dynamic)
    {
        dynamicObjectSet.insert(this);
    }
    /*
    if (node)
    {
        pos = node->getPosition();
    }
    */
}

OffsetObject::~OffsetObject()
{
    //dprintf(MY_DEBUG_NOTE, "OffsetObject::~OffsetObject(): %p\n", this);
    if (iterator != offsetObjectList_t::Iterator())
    {
        printf("offsetObject is in list but under deletion\n");
        assert(0);
        *iterator = 0;
    }
    if (dynamic)
    {
        dynamicObjectSet.erase(this);
    }
}

void OffsetObject::update(const irr::core::vector3df& offset, const irr::core::vector3df& loffset)
{
    if (node)
    {
        //if (dynamic)
        //{
            pos = node->getPosition();
            //printf("update dyn before: pos: %f, %f loffset: %f, %f\n", pos.X, pos.Z, loffset.X, loffset.Z);
            if (skipNodeUpdate == false)
            {
                node->setPosition(pos-loffset);
            }
            //printf("update dyn after: pos: %f, %f \n", node->getPosition().X, node->getPosition().Z);
        //}
        //else
        //{
        //    node->setPosition(pos-offset);
        //}
    }
    //printf("update: %p body %p\n", this, hkBody);
    if (hkBody && !skipNodeUpdate)
    {
        irr::core::vector3df tv;
        //if (dynamic)
        //{
            tv = pos-loffset;
        //}
        //else
        //{
        //    tv = pos-offset;
        //}
        //hk::lock();
        hkBody->setPosition(hkVector4(tv.X, tv.Y, tv.Z));
        //hk::unlock();
    }
    //printf("update: %p body %p end\n", this, hkBody);
    if (updateCB)
    {
        updateCB->handleUpdatePos(false);
    }
}

void OffsetObject::addToManager(bool p_skipNodeUpdate)
{
    skipNodeUpdate = p_skipNodeUpdate;
    offsetManager->addObject(this);
    skipNodeUpdate = false;
}

void OffsetObject::removeFromManager()
{
    offsetManager->removeObject(this);
}

void OffsetObject::setUpdateCB(OffsetObjectUpdateCB* p_updateCB)
{
    updateCB = p_updateCB;
}
    
/*static*/ void OffsetObject::updateDynamicToPhys()
{
    for (dynamicObjectSet_t::iterator it = dynamicObjectSet.begin();
         it != dynamicObjectSet.end();
         it++)
    {
        if ((*it)->hkBody && (*it)->node)
        {
            hkVector4 hkPos = (*it)->hkBody->getPosition();
            hkQuaternion hkQuat = (*it)->hkBody->getRotation();
            irr::core::vector3df newPos(hkPos(0), hkPos(1), hkPos(2));
            irr::core::quaternion(hkQuat.m_vec(0), hkQuat.m_vec(1), hkQuat.m_vec(2), hkQuat.m_vec(3)).getMatrix(helperMatrix, newPos);

            (*it)->node->setPosition(newPos);
            (*it)->node->setRotation(helperMatrix.getRotationDegrees());
            
            if ((*it)->updateCB)
            {
                (*it)->updateCB->handleUpdatePos(true);
            }
        }
    }
}
