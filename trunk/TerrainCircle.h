#ifndef TERRAINCIRCLE_H
#define TERRAINCIRCLE_H

#include <irrlicht.h>
#include "Terrain.h"


class TerrainCircle : public Terrain
{
public:
    TerrainCircle(const irr::core::vector3di& posi, TheEarth* earth, bool detail);
    virtual ~TerrainCircle();

    virtual void load(TheEarth* earth);
    //virtual void setVisible(bool visible);

private:
    bool detail;
};


#endif // TERRAINCIRCLE_H
