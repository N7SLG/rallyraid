#ifndef OFFSETOBJECT_H
#define OFFSETOBJECT_H

#include <irrlicht.h>
#include <set>
#include "hk.h"
#include "OffsetManager.h"

class ObjectPool;

class OffsetObjectUpdateCB
{
protected:
    virtual ~OffsetObjectUpdateCB() {}
    friend class OffsetObject;
    virtual void handleUpdatePos(bool phys) = 0;
};

class OffsetObject
{
public:
    OffsetObject();
    OffsetObject(irr::scene::ISceneNode* node);
    OffsetObject(irr::scene::ISceneNode* node, hkpRigidBody* hkBody);
    OffsetObject(irr::scene::ISceneNode* node, bool dynamic);
    OffsetObject(irr::scene::ISceneNode* node, hkpRigidBody* hkBody, bool dynamic);
    virtual ~OffsetObject();

    irr::scene::ISceneNode* getNode() {return node;}
    void setNode(irr::scene::ISceneNode* p_node)
    {
        node = p_node;
        if (node)
        {
            pos = node->getPosition();
        }
        else
        {
            pos = irr::core::vector3df();
        }
    }

    hkpRigidBody* getBody() {return hkBody;}
    void setBody(hkpRigidBody* p_hkBody)
    {
        if (node)
        {
            hkBody = p_hkBody;
        }
        else
        {
            hkBody = 0;
        }
    }
    
    void update(const irr::core::vector3df& offset, const irr::core::vector3df& loffset);
    
    //void setPos(const irr::core::vector3df& apos) {pos = apos;}
    //irr::core::vector3df& getPos() {return pos;}

    ObjectPool* getPool() {return pool;}
    void setPool(ObjectPool* p_pool) {pool = p_pool;}

    void addToManager(bool p_skipNodeUpdate = false);
    void addToManagerBegin(bool p_skipNodeUpdate = false);
    void removeFromManager();

    void setUpdateCB(OffsetObjectUpdateCB* p_updateCB);
    
    static void updateDynamicToPhys();
    
private:
    friend class OffsetManager;
    irr::scene::ISceneNode*         node;
    hkpRigidBody*                   hkBody;
    irr::core::vector3df            pos;
    offsetObjectList_t::Iterator    iterator;
    bool                            dynamic;
    OffsetManager*                  offsetManager;
    ObjectPool*                     pool;
    OffsetObjectUpdateCB*           updateCB;
    bool                            skipNodeUpdate;
    
    typedef std::set<OffsetObject*> dynamicObjectSet_t;
    static dynamicObjectSet_t       dynamicObjectSet;
    static irr::core::matrix4       helperMatrix;
};

#endif // OFFSETOBJECT_H
