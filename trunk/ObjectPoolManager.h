
#ifndef OBJECTPOOLMANAGER_H
#define OBJECTPOOLMANAGER_H

#include <string>
#include <set>
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
    typedef std::set<irr::scene::ISceneNode*> sceneNodeSet_t;

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
    
    const objectPoolMap_t& getObjectPoolMap() const; // inline
    const sceneNodeSet_t& getShadowNodeSet() const; // inline
    void addShadowNode(irr::scene::ISceneNode* node); // inline
    void removeShadowNode(irr::scene::ISceneNode* node); // inline

private:
    objectPoolMap_t objectPoolMap;

    ObjectPool*     editorPool;
    float           editorScale;
    float           editorRot;
    sceneNodeSet_t  shadowNodeSet;


    friend class MenuPageEditor;
};

inline const ObjectPoolManager::objectPoolMap_t& ObjectPoolManager::getObjectPoolMap() const
{
    return objectPoolMap;
}

inline const ObjectPoolManager::sceneNodeSet_t& ObjectPoolManager::getShadowNodeSet() const
{
    return shadowNodeSet;
}

inline void ObjectPoolManager::addShadowNode(irr::scene::ISceneNode* node)
{
    shadowNodeSet.insert(node);
}

inline void ObjectPoolManager::removeShadowNode(irr::scene::ISceneNode* node)
{
    shadowNodeSet.erase(node);
}

#endif // OBJECTPOOLMANAGER_H
