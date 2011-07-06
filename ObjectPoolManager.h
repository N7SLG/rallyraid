
#ifndef OBJECTPOOLMANAGER_H
#define OBJECTPOOLMANAGER_H

#include <string>
#include <map>
#include <irrlicht.h>

class ObjectPool;
class OffsetObject;

class ObjectPoolManager
{
public:
    static void initialize();
    static void finalize();
    
    static ObjectPoolManager* getInstance() {return objectPoolManager;}

    typedef std::map<std::string, ObjectPool*> objectPoolMap_t;

private:
    static ObjectPoolManager* objectPoolManager;

private:
    ObjectPoolManager();
    ~ObjectPoolManager();

    void read();

public:
    OffsetObject* getObject(const std::string& objectPoolName,
        const irr::core::vector3df& apos,
        const irr::core::vector3df& scale = irr::core::vector3df(1.0f, 1.0f, 1.0f),
        const irr::core::vector3df& rot = irr::core::vector3df(),
        bool addToOffsetManager = true);
    void putObject(OffsetObject* offsetObject);
    
    const objectPoolMap_t& getObjectPoolMap() {return objectPoolMap;}

private:
    objectPoolMap_t objectPoolMap;

    ObjectPool*     editorPool;
    float           editorScale;
    float           editorRot;


    friend class MenuPageEditor;
};

#endif // OBJECTPOOLMANAGER_H
