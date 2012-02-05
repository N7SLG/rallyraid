
#ifndef OBJECTWIRE_H
#define OBJECTWIRE_H

#include <irrlicht.h>
#include <map>
#include <set>
#include <string>
#include <list>
#include "OffsetObject.h"

class ObjectWireGlobalObject;

class ObjectWireTile : public OffsetObjectUpdateCB, public irr::scene::ISceneNode
{
private:
    ObjectWireTile(const irr::core::vector2df& apos, const irr::core::vector2di& rpos, bool _near);
    ~ObjectWireTile();

    virtual void render() {}
    virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const; // inline
    virtual void handleUpdatePos(bool phys);

    virtual void setVisible(bool visible);

private:
    irr::core::vector2df        apos;
    irr::core::vector2di        rpos;
    irr::core::aabbox3d<irr::f32>   bbox;
    OffsetObject*               offsetObject;
    std::list<OffsetObject*>    objectList;
    bool                        _near;


    friend class ObjectWire;
    friend class ObjectWireNear;
    friend class MenuPageEditor;
};

class ObjectWire
{
public:
    static void initialize();
    static void finalize();
    static ObjectWire* getInstance() {return objectWire;}
private:
    static ObjectWire* objectWire;

public:
    ObjectWire();
    ~ObjectWire();
    
    // called by TheGame::loop()
    bool update(const irr::core::vector3df& newPos, bool force = false);
    
    // called by GamePlay::startGame()
    void reset();

    ObjectWireGlobalObject* addGlobalObject(const std::string& objectPoolName,
        const irr::core::vector3df& apos,
        const irr::core::vector3df& rot = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));
        
    ObjectWireGlobalObject* addGlobalObject(ObjectWireGlobalObject* globalObject);

    void removeGlobalObject(ObjectWireGlobalObject* globalObject, bool deleteObject = true);
    
private:
    typedef std::set<ObjectWireGlobalObject*> globalObjectSet_t;
    typedef std::map<int, globalObjectSet_t> globalObjectWire_t;

    ObjectWireTile**        tiles;
    irr::core::vector2di    lastWireCenter;
   // devided by objectWireSize
    globalObjectWire_t      globalObjectWire;


    friend class MenuPageEditor;
};

class ObjectWireNear
{
public:
    static void initialize();
    static void finalize();
    static ObjectWireNear* getInstance() {return objectWireNear;}
private:
    static ObjectWireNear* objectWireNear;

public:
    ObjectWireNear();
    ~ObjectWireNear();
    
    // called by TheGame::loop()
    bool update(const irr::core::vector3df& newPos, bool force = false);
    
    // called by GamePlay::startGame()
    void reset();

private:
    ObjectWireTile**        tiles;
    irr::core::vector2di    lastWireCenter;


    friend class MenuPageEditor;
};

inline const irr::core::aabbox3d<irr::f32>& ObjectWireTile::getBoundingBox() const
{
    return bbox;
}

#endif // OBJECTWIRE_H
