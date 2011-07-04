#ifndef OFFSETMANAGER_H
#define OFFSETMANAGER_H

#include <irrlicht.h>
#include "hk.h"

class OffsetObject;
typedef irr::core::list<OffsetObject*> offsetObjectList_t;

class OffsetManager
{
public:
    static void initialize();
    static void finalize();
    static OffsetManager* getInstance() {return offsetManager;}
private:
    static OffsetManager* offsetManager;

public:
    OffsetManager();
    ~OffsetManager();
    bool update(const irr::core::vector3df& newPos, bool force = false);
    const irr::core::vector3df& getOffset() const {return offset;}
    void addObject(OffsetObject* object);
    void removeObject(OffsetObject* object);
    bool empty() {return objects.empty();}
    void reset();
    
private:
    offsetObjectList_t objects;
    irr::core::vector3df offset;
    irr::core::vector3di last;
};

#endif // OFFSETMANAGER_H
