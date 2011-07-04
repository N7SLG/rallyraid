
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <irrlicht.h>
#include "ObjectWireGlobalObject.h"

class WayPoint : public ObjectWireGlobalObject
{
public:
    WayPoint(const irr::core::vector3df& apos, unsigned int num);
    virtual ~WayPoint();

    virtual void editorRender(bool last);

    unsigned int getNum() const; // inline
    
private:
    virtual void updateVisible();

private:
    unsigned int    num;
};


inline unsigned int WayPoint::getNum() const
{
    return num;
}


#endif // WAYPOINT_H
