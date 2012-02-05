#ifndef TERRAINDETAIL_H
#define TERRAINDETAIL_H

#include <irrlicht.h>
#include "Terrain.h"
#include "Terrain_defs.h"


class TerrainDetail : public Terrain
{
public:
    static void initialize();
    static void finalize();

public:
    TerrainDetail(const irr::core::vector3di& posi, TheEarth* earth);
    virtual ~TerrainDetail();

    virtual void load(TheEarth* earth);
    //virtual void setVisible(bool visible);

    float get(int x, int y); // inline

private:
    float get(int pos); // inline
    void set(int x, int y, float val); // inline
    void set(int pos, float val); // inline
    void add(int x, int y, float val); // inline

    void firstPassRoad();
    void secondPassRoad();
    void thirdPassRoad();
    float getRoadAverage(int x, int y, int radius);
    float getRoadAverage2(int x, int y, int radius);

private:
    static char* adjustMap;
    static float* adjustMap2;

private:
    float* fineHeights;
    float* baseHeights;


    friend class HeightFieldHelper;
};

inline float TerrainDetail::get(int x, int y)
{
    return fineHeights[(x+TILE_DETAIL_RATE) + ((TILE_DETAIL_POINTS_NUM+(3*TILE_DETAIL_RATE))*(y+TILE_DETAIL_RATE))];
}

inline float TerrainDetail::get(int pos)
{
    return fineHeights[pos];
}

inline void TerrainDetail::set(int x, int y, float val)
{
    fineHeights[(x+TILE_DETAIL_RATE) + ((TILE_DETAIL_POINTS_NUM+(3*TILE_DETAIL_RATE))*(y+TILE_DETAIL_RATE))] = val;
}

inline void TerrainDetail::set(int pos, float val)
{
    fineHeights[pos] = val;
}

inline void TerrainDetail::add(int x, int y, float val)
{
    fineHeights[(x+TILE_DETAIL_RATE) + ((TILE_DETAIL_POINTS_NUM+(3*TILE_DETAIL_RATE))*(y+TILE_DETAIL_RATE))] += val;
}

#endif // TERRAINDETAIL_H
