
#include "ItinerManager.h"
#include "ItinerPoint.h"
#include "TheGame.h"
#include "OffsetManager.h"
#include <irrlicht.h>


ItinerPoint::ItinerPoint(const irr::core::vector3df& apos,
        unsigned int num,
        float globalDistance,
        float localDistance,
        const std::string& itinerImageName,
        const std::string& itinerImageName2,
        const std::string& description)
    : ObjectWireGlobalObject(0, apos),
      num(num),
      globalDistance(globalDistance),
      localDistance(localDistance),
      itinerImageName(itinerImageName),
      itinerImage(0),
      itinerImageName2(itinerImageName2),
      itinerImage2(0),
      description(description)
{
    itinerImage = ItinerManager::getInstance()->getItinerImage(itinerImageName);
    itinerImage2 = ItinerManager::getInstance()->getItinerImage2(itinerImageName2);
}

ItinerPoint::~ItinerPoint()
{
}

void ItinerPoint::editorRender(bool last)
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
    irr::video::SColor color(255, 255, 255, 255);
    min.X -= 2.f;
    min.Y += 10.f;
    min.Z -= 2.f;
    max.X += 2.f;
    max.Y += 15.f;
    max.Z += 2.f;
    //printf("min: (%f, %f, %f), max: (%f, %f, %f)\n", min.X, min.Y, min.Z, max.X, max.Y, max.Z);

    if (last)
    {
        color.setGreen(0);
        color.setBlue(0);
    }

    driver->draw3DBox(irr::core::aabbox3df(min, max), color);
    //driver->draw3DLine(min, max, color);
}
    
void ItinerPoint::updateVisible()
{
    if (getVisible())
    {
        ItinerManager::getInstance()->addActiveItinerPoint(this);
    }
    else
    {
        ItinerManager::getInstance()->removeActiveItinerPoint(this);
    }
}
