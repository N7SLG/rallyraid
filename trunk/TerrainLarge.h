#ifndef TERRAINLARGE_H
#define TERRAINLARGE_H

#include <irrlicht.h>
#include "Terrain.h"


class TerrainLarge : public Terrain
{
public:
    TerrainLarge(const irr::core::vector3di& posi, TheEarth* earth);
    virtual ~TerrainLarge();

    virtual void load(TheEarth* earth);
    //virtual void setVisible(bool visible);

private:
};


#endif // TERRAINLARGE_H
