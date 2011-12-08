
#include "TerrainLarge.h"
#include "Terrain_defs.h"
#include "TheGame.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "TheEarth.h"
#include <math.h>
#include "Shaders.h"
#include "Settings.h"
#include "stdafx.h"
#include <CSceneManager.h>


TerrainLarge::TerrainLarge(const irr::core::vector3di& posi, TheEarth* earth)
    : Terrain("large")
{
    offsetX = (posi.X / TILE_LARGE_SCALE);
    offsetY = (posi.Z / TILE_LARGE_SCALE);
    terrain = new irr::scene::TerrainSceneNode((irr::scene::CSceneManager*)TheGame::getInstance()->getSmgr(),
        TheGame::getInstance()->getSmgr(),
        TheGame::getInstance()->getDevice()->getFileSystem(),
        -1,
        4,
        irr::scene::ETPS_17,
        irr::core::vector3df((float)posi.X, 0.0f, (float)posi.Z)/*-OffsetManager::getInstance()->getOffset()*/,
        irr::core::vector3df(),
        irr::core::vector3df(TILE_LARGE_SCALE_F, 1.0f, TILE_LARGE_SCALE_F));
    terrain->setVisible(visible);
    offsetObject = new OffsetObject(terrain);

    postConstruct();
}

TerrainLarge::~TerrainLarge()
{
}

void TerrainLarge::load(TheEarth* earth)
{
    if (!image)
    {
        image = TheGame::getInstance()->getDriver()->createImage(irr::video::ECF_A8R8G8B8, irr::core::dimension2du(TILE_LARGE_POINTS_NUM, TILE_LARGE_POINTS_NUM));
    }
    terrain->loadHeightMap(this, earth, offsetX, offsetY, TILE_LARGE_POINTS_NUM+1, image);
    /*
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
    */
    postLoad();
}

