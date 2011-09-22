
#include "TerrainCircle.h"
#include "Terrain_defs.h"
#include "TheGame.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "TheEarth.h"
#include <math.h>
#include "Shaders.h"
#include "Settings.h"
#include "stdafx.h"
//#include <CSceneManager.h>


class HeightFieldHelper : public hkpSampledHeightFieldShape
{
public:
    HeightFieldHelper(const hkpSampledHeightFieldBaseCinfo& ci, TheEarth* earth, int offsetX, int offsetY)
        : hkpSampledHeightFieldShape(ci),
          earth(earth),
          offsetX(offsetX),
          offsetY(offsetY)
    {
    }
    
    HK_FORCE_INLINE hkReal getHeightAtImpl(int x, int z) const
    {
        //printf("gh ");
        unsigned short h = earth->getTileHeight((unsigned int)abs(offsetX+x), (unsigned int)abs(offsetY+z));
        //printf("x: %d, y: %d, offsetX: %d, offsetY: %d, h: %hu\n", x, z, offsetX, offsetY, h);
        return (hkReal)h;
    }
    
    /// this should return true if the two triangles share the edge p00-p11
    ///      otherwise it should return false if the triangles shere the edge p01-p10
    HK_FORCE_INLINE hkBool getTriangleFlipImpl() const
    {
        return true;
    }
    
    virtual void collideSpheres( const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
    {
        hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
    }
private:
    TheEarth* earth;
    int offsetX;
    int offsetY;
};

TerrainCircle::TerrainCircle(const irr::core::vector3di& posi, TheEarth* earth, bool detail)
    : Terrain("circle"), detail(detail)
{
    offsetX = (posi.X / TILE_SCALE);
    offsetY = (posi.Z / TILE_SCALE);
    terrain = new irr::scene::TerrainSceneNode(0/*(irr::scene::CSceneManager*)TheGame::getInstance()->getSmgr()*/,
        TheGame::getInstance()->getSmgr(),
        TheGame::getInstance()->getDevice()->getFileSystem(),
        -1,
        4,
        irr::scene::ETPS_17,
        irr::core::vector3df((float)posi.X, 0.0f, (float)posi.Z)/*-OffsetManager::getInstance()->getOffset()*/,
        irr::core::vector3df(),
        irr::core::vector3df(TILE_SCALE_F, 1.0f, TILE_SCALE_F));
    terrain->setVisible(visible);
    offsetObject = new OffsetObject(terrain);

    if (earth)
    {
        hk::lock();
        hkpSampledHeightFieldBaseCinfo ci;
        ci.m_xRes = TILE_POINTS_NUM + 1;
        ci.m_zRes = TILE_POINTS_NUM + 1;
        ci.m_scale.set(TILE_SCALE_F, 1.0f, TILE_SCALE_F);

        hkShape = new HeightFieldHelper(ci, earth, offsetX, offsetY/*posi.X, posi.Z*/);
        hk::unlock();
    }
    postConstruct();
}

TerrainCircle::~TerrainCircle()
{
}

void TerrainCircle::load(TheEarth* earth)
{
    if (!image && !detail)
    {
        image = TheGame::getInstance()->getDriver()->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2du(TILE_POINTS_NUM, TILE_POINTS_NUM));
    }
    terrain->loadHeightMap(earth, offsetX, offsetY, TILE_POINTS_NUM+1, image);

    if (!image)
    {
        image = TheGame::getInstance()->getDriver()->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2du(TILE_FINE_POINTS_NUM, TILE_FINE_POINTS_NUM));
        int offsetXFine = offsetX * TILE_FINE_RATE;
        int offsetYFine = offsetY * TILE_FINE_RATE;
        dprintf(MY_DEBUG_NOTE, "===============================\n%d %d\n===============================\n", offsetXFine, offsetYFine);
        for (int x = 0; x < TILE_FINE_POINTS_NUM; x++)
        {
            for (int z = 0; z < TILE_FINE_POINTS_NUM; z++)
            {
                image->setPixel(TILE_FINE_POINTS_NUM - x - 1, z, earth->getTileFineTexture((unsigned int)abs(offsetXFine+x), (unsigned int)abs(offsetYFine+z)));
            }
        }
    }
    postLoad();
}

