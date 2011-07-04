
#ifndef ITITERPOINT_H
#define ITITERPOINT_H

#include <irrlicht.h>
#include "ObjectWireGlobalObject.h"

class ItinerPoint : public ObjectWireGlobalObject
{
public:
    ItinerPoint(const irr::core::vector3df& apos,
        unsigned int num,
        float globalDistance,
        float localDistance,
        const std::string& itinerImageName,
        const std::string& itinerImageName2,
        const std::string& description);
    virtual ~ItinerPoint();

    virtual void editorRender(bool last);

    unsigned int getNum() const; // inline
    float getGlobalDistance() const; // inline
    float getLocalDistance() const; // inline
    const std::string& getItinerImageName() const; // inline
    irr::video::ITexture* getItinerImage() const; // inline
    const std::string& getItinerImageName2() const; // inline
    irr::video::ITexture* getItinerImage2() const; // inline
    const std::string& getDescription() const; // inline
    
private:
    virtual void updateVisible();

private:
    unsigned int            num;
    float                   globalDistance;
    float                   localDistance;
    std::string             itinerImageName;
    irr::video::ITexture*   itinerImage;
    std::string             itinerImageName2;
    irr::video::ITexture*   itinerImage2;
    std::string             description;
};


inline unsigned int ItinerPoint::getNum() const
{
    return num;
}

inline float ItinerPoint::getGlobalDistance() const
{
    return globalDistance;
}

inline float ItinerPoint::getLocalDistance() const
{
    return localDistance;
}

inline const std::string& ItinerPoint::getItinerImageName() const
{
    return itinerImageName;
}

inline irr::video::ITexture* ItinerPoint::getItinerImage() const
{
    return itinerImage;
}

inline const std::string& ItinerPoint::getItinerImageName2() const
{
    return itinerImageName2;
}

inline irr::video::ITexture* ItinerPoint::getItinerImage2() const
{
    return itinerImage2;
}

inline const std::string& ItinerPoint::getDescription() const
{
    return description;
}


#endif // ITITERPOINT_H
