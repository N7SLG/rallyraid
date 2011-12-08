
#include "ObjectWireGlobalObject.h"
#include "ObjectPool.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "TheEarth.h"
#include "TheGame.h"
#include <assert.h>

ObjectWireGlobalObject::ObjectWireGlobalObject(ObjectPool* objectPool,
    const irr::core::vector3df& apos,
    const irr::core::vector3df& rot,
    const irr::core::vector3df& scale)
    : objectPool(objectPool),
      apos(apos),
      rot(rot),
      scale(scale),
      visible(false),
      softVisible(false),
      offsetObject(0)
{
}

ObjectWireGlobalObject::~ObjectWireGlobalObject()
{
    setVisible(false);
}

void ObjectWireGlobalObject::setVisible(bool p_visible)
{
    if (visible == p_visible) return;
    
    visible = p_visible;
    
    if (visible)
    {
        if (objectPool)
        {
            /*
            irr::core::vector3df objectPos = irr::core::vector3df(apos.X, -50.f, apos.Z);
            objectPos.Y = TheEarth::getInstance()->getHeight(objectPos-OffsetManager::getInstance()->getOffset());
            //printf("opos: %f %f %f\n", objectPos.X, objectPos.Y, objectPos.Z);
            if (objectPos.Y > 10.f)
            {
                offsetObject = objectPool->getObject(objectPos, scale, rot);
                if (offsetObject)
                {
                    //offsetObject->getNode()->setRotation(rot);
                    softVisible = true;
                }
            }
            */
            offsetObject = objectPool->getObject(apos, scale, rot);
            if (offsetObject)
            {
                //offsetObject->getNode()->setRotation(rot);
                softVisible = true;
            }
        }
    }
    else
    {
        if (objectPool && offsetObject)
        {
            objectPool->putObject(offsetObject);
            offsetObject = 0;
            softVisible = false;
        }
    }
    
    updateVisible();
}

void ObjectWireGlobalObject::setSoftVisible(bool softVisible)
{
    if (softVisible == this->softVisible || offsetObject == 0 || offsetObject->getNode() == 0) return;

    this->softVisible = softVisible;
    offsetObject->getNode()->setVisible(softVisible);
}

void ObjectWireGlobalObject::editorRender(bool last)
{
    irr::video::IVideoDriver* driver = TheGame::getInstance()->getDriver();
    irr::core::vector3df renderPos = apos - OffsetManager::getInstance()->getOffset();

    //printf("apos (%f, %f, %f), offset: (%f, %f, %f), renderpos (%f, %f, %f)\n",
    //    apos.X, apos.Y, apos.Z,
    //    OffsetManager::getInstance()->getOffset().X, OffsetManager::getInstance()->getOffset().Y, OffsetManager::getInstance()->getOffset().Z,
    //    renderPos.X, renderPos.Y, renderPos.Z);
    //assert(0);

    irr::core::vector3df min = renderPos;
    irr::core::vector3df max = renderPos;
    irr::video::SColor color(255, 255, 255, 0);
    min.X -= 2.f;
    min.Z -= 2.f;
    max.X += 2.f;
    max.Y += 20.f;
    max.Z += 2.f;
    //printf("min: (%f, %f, %f), max: (%f, %f, %f)\n", min.X, min.Y, min.Z, max.X, max.Y, max.Z);

    if (last) color.setGreen(0);

    driver->draw3DBox(irr::core::aabbox3df(min, max), color);
    // not used: driver->draw3DLine(min, max, color);
}
