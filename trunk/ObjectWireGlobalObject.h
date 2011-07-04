
#ifndef OBJECTWIREGLOBALOBJECT_H
#define OBJECTWIREGLOBALOBJECT_H

#include <irrlicht.h>
#include <string>


class ObjectPool;
class OffsetObject;

class ObjectWireGlobalObject
{
public:
    ObjectWireGlobalObject(ObjectPool* objectPool,
        const irr::core::vector3df& apos,
        const irr::core::vector3df& rot = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));
    virtual ~ObjectWireGlobalObject();

    void setVisible(bool p_visible);
    bool getVisible() {return visible;}
    void setSoftVisible(bool softVisible); 
    bool getSoftVisible(); // inline

    ObjectPool* getObjectPool() {return objectPool;}
    const irr::core::vector3df& getPos() {return apos;}
    const irr::core::vector3df& getRot() {return rot;}
    const irr::core::vector3df& getScale() {return scale;}

    virtual void editorRender(bool last);
    
protected:
    virtual void updateVisible() {} 

private:
    ObjectPool*             objectPool;
    irr::core::vector3df    apos;
    irr::core::vector3df    rot;
    irr::core::vector3df    scale;
    bool                    visible;
    bool                    softVisible;
    OffsetObject*           offsetObject;
    
    friend class ObjectWire;
};

inline bool ObjectWireGlobalObject::getSoftVisible()
{
    return softVisible;
}

#endif // OBJECTWIREGLOBALOBJECT_H
