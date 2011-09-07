
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <irrlicht.h>
#include "ObjectWireGlobalObject.h"

class WayPoint : public ObjectWireGlobalObject
{
public:
    enum Type
    {
        Hidden,
       // this must be the first, because the editor use this
        NumberOfTypes
    };

public:
    WayPoint(const irr::core::vector3df& apos, unsigned int num, WayPoint::Type type = WayPoint::Hidden);
    virtual ~WayPoint();

    virtual void editorRender(bool last);

    unsigned int getNum() const; // inline
    WayPoint::Type getType() const; // inline
    
private:
    virtual void updateVisible();

private:
    unsigned int    num;
    Type            type;
};


inline unsigned int WayPoint::getNum() const
{
    return num;
}

inline WayPoint::Type WayPoint::getType() const
{
    return type;
}

#endif // WAYPOINT_H
