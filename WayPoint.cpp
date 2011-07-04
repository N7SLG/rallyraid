
#include "WayPointManager.h"
#include "WayPoint.h"
#include "TheGame.h"
#include "OffsetManager.h"
#include <irrlicht.h>


WayPoint::WayPoint(const irr::core::vector3df& apos, unsigned int num)
    : ObjectWireGlobalObject(0, apos),
      num(num)
{
}

WayPoint::~WayPoint()
{
}

void WayPoint::editorRender(bool last)
{
    irr::video::IVideoDriver* driver = TheGame::getInstance()->getDriver();
    irr::core::vector3df renderPos = getPos() - OffsetManager::getInstance()->getOffset();

    //printf("apos (%f, %f, %f), offset: (%f, %f, %f), renderpos (%f, %f, %f)\n",
    //    apos.X, apos.Y, apos.Z,
    //    OffsetManager::getInstance()->getOffset().X, OffsetManager::getInstance()->getOffset().Y, OffsetManager::getInstance()->getOffset().Z,
    //    renderPos.X, renderPos.Y, renderPos.Z);
    //assert(0);

    irr::core::vector3df min = renderPos;
    irr::core::vector3df max = renderPos;
    irr::video::SColor color(255, 255, 0, 255);
    min.X -= 5.f;
    min.Y += 5.f;
    min.Z -= 5.f;
    max.X += 5.f;
    max.Y += 15.f;
    max.Z += 5.f;
    //printf("min: (%f, %f, %f), max: (%f, %f, %f)\n", min.X, min.Y, min.Z, max.X, max.Y, max.Z);

    if (last)
    {
        color.setGreen(0);
        color.setBlue(0);
    }

    driver->draw3DBox(irr::core::aabbox3df(min, max), color);
    //driver->draw3DLine(min, max, color);
}
    
void WayPoint::updateVisible()
{
    if (getVisible())
    {
        WayPointManager::getInstance()->addActiveWayPoint(this);
    }
    else
    {
        WayPointManager::getInstance()->removeActiveWayPoint(this);
    }
}
