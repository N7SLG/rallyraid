
#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <string>
#include <list>
#include <irrlicht.h>
#include "hk.h"

class OffsetObject;
namespace irr
{
    namespace scene
    {
        class CTreeGenerator;
    }
}

class ObjectPool
{
public:
    enum ObjectType
    {
        Standard = 0,
        Vehicle,
        Grass,
        Tree,
        MyTree
    };

    enum Category
    {
        All = 0,
        Normal = 1,
        HalfDesert = 2,
        Desert = 4
    };

    typedef std::list<OffsetObject*> objectList_t;

public:
    ObjectPool(const std::string& name,
        const std::string& meshFilename, const std::string& textureFilename,
        const std::string& texture2Filename,
        bool physics, ObjectType objectType,
        const std::string& materialName, const std::string& material2Name,
        unsigned int num, unsigned int category,
        float friction, float mass, const irr::core::vector3df& center);
    ~ObjectPool();

    OffsetObject* getObject(const irr::core::vector3df& apos, const irr::core::vector3df& scale = irr::core::vector3df(1.0f, 1.0f, 1.0f), const irr::core::vector3df& rot = irr::core::vector3df(), bool addToOffsetManager = true);
    void putObject(OffsetObject* object);

    OffsetObject* createNewInstance();
    
    int getCategory() {return category;}
    unsigned int getNum() {return num;}
    const std::string& getName() {return name;}
    
    // read mso object, which is my format can be edited by a simple text editor
    static irr::scene::SAnimatedMesh* readMySimpleObject(const std::string& meshFilename, float scale = 1.0f);
    
    // calculate convex hull from an irrlicht mesh
    static hkpShape* calculateCollisionMesh(irr::scene::IAnimatedMesh* objectMesh, ObjectType objectType, bool box = false);
    
    // calculate general havok shape from an irrlicht mesh: it is only used for the roads
    static hkpExtendedMeshShape* calculateNonConvexCollisionMeshMeshes(irr::scene::IAnimatedMesh* objectMesh);
    
    // generate a grass object mesh
    static irr::scene::SAnimatedMesh* generateGrassMesh();

private:
    std::string                 name;
    objectList_t                objectList;

    irr::scene::IAnimatedMesh*  objectMesh;
    irr::scene::CTreeGenerator* treeGenerator;
    hkpShape*                   hkShape;
    int                         category;

    irr::video::ITexture*       texture;
    irr::video::ITexture*       texture2;
    ObjectType                  objectType;
    irr::video::E_MATERIAL_TYPE material;
    irr::video::E_MATERIAL_TYPE material2;
    float                       friction;
    float                       mass;
    irr::core::vector3df        center;
    unsigned int                num;
    
    unsigned int                inUse;
    
    
    friend class MenuPageEditor;
};

#endif // OBJECTPOOL_H
