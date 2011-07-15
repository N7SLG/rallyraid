
#include "TheEarth.h"
#include "Tile.h"
#include "TheGame.h"


#include <irrlicht.h>
#include <stdio.h>
#include <assert.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <direct.h>
#include <errno.h>
#include "TerrainDetail.h"
#include "TerrainCircle.h"
#include "TerrainLarge.h"
#include "Settings.h"
#include "RoadManager.h"
#include "Shaders.h"
#include "stdafx.h"


const irr::core::vector3di TheEarth::VisualMembers::terrainPos[3][3] =
{
    {
        irr::core::vector3di(-TILE_SIZE-TILE_HSIZE, 0,            TILE_HSIZE),
        irr::core::vector3di(-TILE_SIZE-TILE_HSIZE, 0,           -TILE_HSIZE),
        irr::core::vector3di(-TILE_SIZE-TILE_HSIZE, 0, -TILE_SIZE-TILE_HSIZE)

    },
    {
        irr::core::vector3di(          -TILE_HSIZE, 0,            TILE_HSIZE),
        irr::core::vector3di(          -TILE_HSIZE, 0,           -TILE_HSIZE),
        irr::core::vector3di(          -TILE_HSIZE, 0, -TILE_SIZE-TILE_HSIZE)
    },
    {
        irr::core::vector3di(           TILE_HSIZE, 0,            TILE_HSIZE),
        irr::core::vector3di(           TILE_HSIZE, 0,           -TILE_HSIZE),
        irr::core::vector3di(           TILE_HSIZE, 0, -TILE_SIZE-TILE_HSIZE)
    }
};

const irr::core::vector3df TheEarth::VisualMembers::terrainPosf[3][3] =
{
    {
        irr::core::vector3df(-TILE_SIZE_F-TILE_HSIZE_F, 0.0f,              TILE_HSIZE_F),
        irr::core::vector3df(-TILE_SIZE_F-TILE_HSIZE_F, 0.0f,             -TILE_HSIZE_F),
        irr::core::vector3df(-TILE_SIZE_F-TILE_HSIZE_F, 0.0f, -TILE_SIZE_F-TILE_HSIZE_F)

    },
    {
        irr::core::vector3df(            -TILE_HSIZE_F, 0.0f,              TILE_HSIZE_F),
        irr::core::vector3df(            -TILE_HSIZE_F, 0.0f,             -TILE_HSIZE_F),
        irr::core::vector3df(            -TILE_HSIZE_F, 0.0f, -TILE_SIZE_F-TILE_HSIZE_F)
    },
    {
        irr::core::vector3df(             TILE_HSIZE_F, 0.0f,              TILE_HSIZE_F),
        irr::core::vector3df(             TILE_HSIZE_F, 0.0f,             -TILE_HSIZE_F),
        irr::core::vector3df(             TILE_HSIZE_F, 0.0f, -TILE_SIZE_F-TILE_HSIZE_F)
    }
};

const irr::core::vector3di TheEarth::VisualMembers::terrainLargePos =
    irr::core::vector3di(-TILE_LARGE_HSIZE, 0, -TILE_LARGE_HSIZE);
const irr::core::vector3df TheEarth::VisualMembers::terrainLargePosf =
    irr::core::vector3df(-TILE_LARGE_HSIZE_F, 0.0f, -TILE_LARGE_HSIZE_F);

TheEarth::VisualMembers::VisualMembers()
    : terrainLarge(0), removeLarge(true)
{
    memset(terrainCircle, 0, sizeof(Terrain*)*3*3);
}

TheEarth::VisualMembers::~VisualMembers()
{
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            if (terrainCircle[i][j])
            {
                delete terrainCircle[i][j];
                terrainCircle[i][j] = 0;
            }
        }
    }
    if (terrainLarge && removeLarge)
    {
        delete terrainLarge;
        terrainLarge = 0;
    }
}

void TheEarth::VisualMembers::setVisible(bool visible)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            if (terrainCircle[i][j])
            {
                terrainCircle[i][j]->setVisible(visible);
            }
        }
    }
    if (terrainLarge)
    {
        terrainLarge->setVisible(visible);
    }
}

void TheEarth::VisualMembers::createMembers(const irr::core::vector3di& centerPosi, const irr::core::vector3di& largeCenterPosi, 
    TheEarth* earth, Terrain* lastTerrainLarge)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            assert(terrainCircle[i][j] == 0);
            if (i == 1 && j == 1)
            {
                if (Settings::getInstance()->useTerrainDetail)
                {
                    terrainCircle[i][j] = new TerrainDetail(centerPosi+terrainPos[i][j], earth);
                }
                else
                {
                    terrainCircle[i][j] = new TerrainCircle(centerPosi+terrainPos[i][j], earth, true);
                }
            }
            else
            {
                terrainCircle[i][j] = new TerrainCircle(centerPosi+terrainPos[i][j], earth, false);
            }
        }
    }
    assert(terrainLarge == 0);
    if (lastTerrainLarge)
    {
        terrainLarge = lastTerrainLarge;
    }
    else
    {
        terrainLarge = new TerrainLarge(largeCenterPosi+terrainLargePos, earth);
    }
}

void TheEarth::VisualMembers::loadMembers(TheEarth* earth)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            assert(terrainCircle[i][j] != 0);
            terrainCircle[i][j]->load(earth);
        }
    }
    assert(terrainLarge);
    terrainLarge->load(earth);
}

void TheEarth::VisualMembers::finalizeMembers()
{
}

void TheEarth::VisualMembers::registerMembers()
{
    //assert(0);
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            if (terrainCircle[i][j])
            {
                terrainCircle[i][j]->registerTerrain();
            }
        }
    }
    if (terrainLarge)
    {
        terrainLarge->registerTerrain();
    }
}

float TheEarth::VisualMembers::getHeight(float x, float z)
{
    const float height = -50.f;
    float tmp;
    
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            if (terrainCircle[i][j])
            {
                tmp = terrainCircle[i][j]->getHeight(x, z);
                if (tmp > height) return tmp;
            }
        }
    }
    if (terrainLarge)
    {
        terrainLarge->getHeight(x, z);
    }
    return height;
}


TheEarth* TheEarth::theEarth = 0;
irr::video::SColor TheEarth::baseColor;

void TheEarth::initialize()
{
    if (theEarth == 0)
    {
        theEarth = new TheEarth();
    }
}

void TheEarth::finalize()
{
    if (theEarth)
    {
        delete theEarth;
        theEarth = 0;
    }
}
    
    
TheEarth::TheEarth()
    : earthTexture(0),
      visualPart(0),
      newVisualPart(0),
      newReadyVisualPart(0),
      lastPosBox(),
      lastCenterPos(),
      lastCenterPosi(),
      lastLargeCenterPosi(),
      miniMap(0),
      miniMapTexture(0),
      lastMiniMapPos(),
      smokeTexture(0),
      smokeMaterial((irr::video::E_MATERIAL_TYPE)0),
      xsize(0),
      ysize(0),
      height(0),
      density(0),
      hasDetail(0),
      isLoaded(0),
      //hasDetailTex(0),
      maxHeight(0),
      minHeight(0xFFFF),
      tileMap()
{
    TheGame::getInstance()->getDevice()->getFileSystem()->grab();
    read();
#if 1
    miniMap = TheGame::getInstance()->getDriver()->createImage(irr::video::ECF_R8G8B8, irr::core::dimension2du(MINIMAP_SIZE,MINIMAP_SIZE));
#else
    miniMap = TheGame::getInstance()->getDriver()->createImage(irr::video::ECF_R8G8B8, irr::core::dimension2du(TILE_POINTS_NUM, TILE_POINTS_NUM));
#endif
    smokeTexture = TheGame::getInstance()->getDriver()->getTexture("data/smoke/dirt.png");
    smokeMaterial = Shaders::getInstance()->materialMap["smoke"];
}

TheEarth::~TheEarth()
{
    if (earthTexture)
    {
        earthTexture->drop();
        earthTexture = 0;
    }
    if (height)
    {
        delete [] height;
        height = 0;
    }
    if (density)
    {
        density->drop();
        density = 0;
    }
    if (hasDetail)
    {
        delete [] hasDetail;
        hasDetail = 0;
    }
    /*if (hasDetailTex)
    {
        delete [] hasDetailTex;
        hasDetailTex = 0;
    }*/
    
    if (visualPart)
    {
        delete visualPart;
        visualPart = 0;
    }
    if (newReadyVisualPart)
    {
        delete newReadyVisualPart;
        newReadyVisualPart = 0;
    }
    
    for (tileMap_t::iterator it = tileMap.begin(); it != tileMap.end(); it++)
    {
        delete it->second;
    }
    tileMap.clear();
    if (miniMap)
    {
        miniMap->drop();
    }
    miniMapTexture = 0;
    TheGame::getInstance()->getDevice()->getFileSystem()->drop();
}

unsigned short TheEarth::getTileHeight(unsigned int x, unsigned int y)
{
    const unsigned int tileX = x / TILE_POINTS_NUM;
    const unsigned int tileY = y / TILE_POINTS_NUM;
    const unsigned int inX = x % TILE_POINTS_NUM;
    const unsigned int inY = y % TILE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                tile = new Tile(tileX, tileY, tileNum,
                    getEarthTexture(tileX, tileY),
                    getEarthTexture(tileX+1, tileY),
                    getEarthTexture(tileX, tileY+1),
                    getEarthTexture(tileX+1, tileY+1),
                    getEarthDensity(tileX, tileY));
                tileMap[tileNum] = tile;
                setIsLoaded(tileNum, true);
            }
            else
            {
                return 0;
            }
        }
        return tile->getHeight(inX, inY);
    }
    else
    {
        return 0;
    }
}

const irr::video::SColor& TheEarth::getTileTexture(unsigned int x, unsigned int y)
{
    const unsigned int tileX = x / TILE_POINTS_NUM;
    const unsigned int tileY = y / TILE_POINTS_NUM;
    const unsigned int inX = x % TILE_POINTS_NUM;
    const unsigned int inY = y % TILE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                tile = new Tile(tileX, tileY, tileNum,
                    getEarthTexture(tileX, tileY),
                    getEarthTexture(tileX+1, tileY),
                    getEarthTexture(tileX, tileY+1),
                    getEarthTexture(tileX+1, tileY+1),
                    getEarthDensity(tileX, tileY));
                tileMap[tileNum] = tile;
                setIsLoaded(tileNum, true);
            }
            else
            {
                baseColor = getEarthTexture(tileX, tileY);
                return baseColor;
            }
        }
        return tile->getColor(inX, inY);
    }
    else
    {
        return baseColor;
    }
}

void TheEarth::getTileHeightAndTexture(unsigned int x, unsigned int y, 
    unsigned short& height, irr::video::SColor& textureColor)
{
    const unsigned int tileX = x / TILE_POINTS_NUM;
    const unsigned int tileY = y / TILE_POINTS_NUM;
    const unsigned int inX = x % TILE_POINTS_NUM;
    const unsigned int inY = y % TILE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                tile = new Tile(tileX, tileY, tileNum,
                    getEarthTexture(tileX, tileY),
                    getEarthTexture(tileX+1, tileY),
                    getEarthTexture(tileX, tileY+1),
                    getEarthTexture(tileX+1, tileY+1),
                    getEarthDensity(tileX, tileY));
                tileMap[tileNum] = tile;
                setIsLoaded(tileNum, true);
            }
            else
            {
                height = 0;
                textureColor = /*irr::video::SColor(0, 0, 255, 0);//*/getEarthTexture(tileX, tileY);
                return;
            }
        }
        height = tile->getHeight(inX, inY);
        textureColor = /*irr::video::SColor(0, 255, 0, 0);getEarthTexture(tileX, tileY);*/tile->getColor(inX, inY);
        //printf("%u, %u: %u %u %u\n", inX, inY, textureColor.getRed(), textureColor.getGreen(), textureColor.getBlue());
    }
    else
    {
        height = 0;
        textureColor = /*irr::video::SColor(0, 0, 0, 255);//*/baseColor;
    }
}

const irr::video::SColor& TheEarth::getTileFineTexture(unsigned int x, unsigned int y)
{
    const unsigned int tileX = x / TILE_FINE_POINTS_NUM;
    const unsigned int tileY = y / TILE_FINE_POINTS_NUM;
    const unsigned int inX = x % TILE_FINE_POINTS_NUM;
    const unsigned int inY = y % TILE_FINE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                tile = new Tile(tileX, tileY, tileNum,
                    getEarthTexture(tileX, tileY),
                    getEarthTexture(tileX+1, tileY),
                    getEarthTexture(tileX, tileY+1),
                    getEarthTexture(tileX+1, tileY+1),
                    getEarthDensity(tileX, tileY));
                tileMap[tileNum] = tile;
                setIsLoaded(tileNum, true);
            }
            else
            {
                baseColor = getEarthTexture(tileX, tileY);
                return baseColor;
            }
        }
        return tile->getFineColor(inX, inY);
    }
    else
    {
        return baseColor;
    }
}

const irr::video::SColor& TheEarth::getTileFineDensity(unsigned int x, unsigned int y)
{
    const unsigned int tileX = x / TILE_FINE_POINTS_NUM;
    const unsigned int tileY = y / TILE_FINE_POINTS_NUM;
    const unsigned int inX = x % TILE_FINE_POINTS_NUM;
    const unsigned int inY = y % TILE_FINE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                //assert(0 && "should be called only on loaded tiles");
                baseColor = irr::video::SColor(0);
            }
            else
            {
                baseColor = getEarthDensity(tileX, tileY);
            }
            return baseColor;
        }
        return tile->getFineDensity(inX, inY);
    }
    else
    {
        return baseColor;
    }
}

void TheEarth::setTileFineTexture(unsigned int x, unsigned int y, const irr::video::SColor& val)
{
    const unsigned int tileX = x / TILE_FINE_POINTS_NUM;
    const unsigned int tileY = y / TILE_FINE_POINTS_NUM;
    const unsigned int inX = x % TILE_FINE_POINTS_NUM;
    const unsigned int inY = y % TILE_FINE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                assert(0 && "should be called only on loaded tiles");
            }
            else
            {
                return;
            }
        }
        return tile->setFineColor(inX, inY, val);
    }
    else
    {
        return;
    }
}

void TheEarth::setTileFineDensity(unsigned int x, unsigned int y, const irr::video::SColor& val)
{
    const unsigned int tileX = x / TILE_FINE_POINTS_NUM;
    const unsigned int tileY = y / TILE_FINE_POINTS_NUM;
    const unsigned int inX = x % TILE_FINE_POINTS_NUM;
    const unsigned int inY = y % TILE_FINE_POINTS_NUM;
    const unsigned int tileNum = tileX + (xsize*tileY);

    if (tileX < xsize && tileY < ysize)
    {
        Tile* tile;
        if (getIsLoaded(tileNum))
        {
            tile = tileMap[tileNum];
            tile->setInUse();
        }
        else
        {
            if (getHasDetail(tileNum))
            {
                assert(0 && "should be called only on loaded tiles");
            }
            else
            {
                return;
            }
        }
        return tile->setFineDensity(inX, inY, val);
    }
    else
    {
        return;
    }
}


bool TheEarth::read()
{
    bool ret = true;
    ret &= readHeight();
    ret &= readHasDetail();
    //ret &= readHasDetailTex();
    ret &= readDensity();
    ret &= readEarthTexture();
    return ret;
}

bool TheEarth::readHeight()
{
    int rc = 0;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_height.dat", "rb");
    if (error)
    {
        printf("unable to open file for read data/earthdata/earth_data_height.dat\n");
        return false;
    }

    rc = fscanf_s(f, "%u\n%u\n", &xsize, &ysize);
    if (rc < 2)
    {
        printf("unable to read xsize, ysize\n");
        fclose(f);
        return false;
    }

    if (height) delete [] height;
    height = new unsigned short[xsize*ysize];
    memset(height, 0, (xsize*ysize) * sizeof(unsigned short));

    if (isLoaded) delete [] isLoaded;
    isLoaded = new unsigned char[(xsize*ysize)/8+1];
    memset(isLoaded, 0, (xsize*ysize)/8+1);

    unsigned int twsize = xsize * ysize;// * sizeof(unsigned short);
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fread(height+(twsize-i), sizeof(unsigned short), wsize, f);
        if (ret != wsize)
        {
             printf("unable to read offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }

    maxHeight = 0;
    minHeight = 0xffff;

    for (unsigned int j = 0; j < ysize; j++)
    {
        for (unsigned int i = 0; i < xsize; i++)
        {
            unsigned short val = getEarthHeight(i, j);
            if (val > maxHeight) maxHeight = val;
            if (val < minHeight) minHeight = val;
        }
    }
    if (minHeight > maxHeight) minHeight = maxHeight;

    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map read height ok, min %hu, max %hu, diff %hu, step %hu\n",
        xsize, ysize, minHeight, maxHeight, maxHeight - minHeight, (maxHeight-minHeight)/255);
    return true;
}

bool TheEarth::readHasDetail()
{
    int rc = 0;
    unsigned int readxsize;
    unsigned int readysize;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_hasDetail.dat", "rb");
    if (error)
    {
        printf("unable to open file for read data/earthdata/earth_data_hasDetail.dat\n");
        return false;
    }

    rc = fscanf_s(f, "%u\n%u\n", &readxsize, &readysize);
    if (rc < 2)
    {
        printf("unable to read xsize, ysize\n");
        fclose(f);
        return false;
    }

    assert(readxsize == xsize && readysize == ysize);

    if (hasDetail) delete [] hasDetail;
    hasDetail = new unsigned char[(xsize*ysize)/8+1];
    memset(hasDetail, 0, (xsize*ysize)/8+1);

    unsigned int twsize = (xsize*ysize)/8+1;
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fread(hasDetail+(twsize-i), sizeof(unsigned char), wsize, f);
        if (ret != wsize)
        {
             printf("unable to read road data offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }

    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map read hasDetail ok\n", xsize, ysize);
    return true;
}

/*bool TheEarth::readHasDetailTex()
{
    int rc = 0;
    unsigned int readxsize;
    unsigned int readysize;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_hasDetailTex.dat", "rb");
    if (error)
    {
        printf("unable to open file for read data/earthdata/earth_data_hasDetailTex.dat\n");
        return false;
    }

    rc = fscanf_s(f, "%u\n%u\n", &readxsize, &readysize);
    if (rc < 2)
    {
        printf("unable to read xsize, ysize\n");
        fclose(f);
        return false;
    }

    assert(readxsize == xsize && readysize == ysize);

    if (hasDetailTex) delete [] hasDetailTex;
    hasDetailTex = new unsigned char[(xsize*ysize)/8+1];
    memset(hasDetailTex, 0, (xsize*ysize)/8+1);

    unsigned int twsize = (xsize*ysize)/8+1;
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fread(hasDetailTex+(twsize-i), sizeof(unsigned char), wsize, f);
        if (ret != wsize)
        {
             printf("unable to read road data offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }

    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map read hasDetailTex ok\n", xsize, ysize);
    return true;
}*/

bool TheEarth::readDensity()
{

    if (density) density->drop();
    density = TheGame::getInstance()->getDriver()->createImageFromFile("data/earthdata/earth_data_density.png");
    dprintf(MY_DEBUG_NOTE, "%ux%u map read density ok\n", density->getDimension().Width, density->getDimension().Height);
    assert(density->getDimension().Width == xsize && density->getDimension().Height == ysize);
    return true;
}

bool TheEarth::readEarthTexture()
{

    if (earthTexture) earthTexture->drop();
    earthTexture = TheGame::getInstance()->getDriver()->createImageFromFile("data/earthdata/earth_data_texture.png");
    dprintf(MY_DEBUG_NOTE, "%ux%u map read texture ok\n", earthTexture->getDimension().Width, earthTexture->getDimension().Height);
    assert(earthTexture->getDimension().Width == xsize && earthTexture->getDimension().Height == ysize);
    return true;
}


bool TheEarth::write()
{
    bool ret = true;
    ret &= writeHeight();
    ret &= writeHasDetail();
    //ret &= writeHasDetailTex();
    return ret;
}

bool TheEarth::writeHeight()
{
    if (height==0) return false;
    
    int rc = 0;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_height.dat", "wb");
    if (error) return false;

    rc = fprintf(f, "%u\n%u\n", xsize, ysize);
    if (rc <= 0)
    {
        printf("unable to write sizes\n");
        fclose(f);
        return false;
    }

    unsigned int twsize = xsize * ysize;// * sizeof(unsigned short);
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fwrite(height+(twsize-i), sizeof(unsigned short), wsize, f);
        if (ret != wsize)
        {
             printf("unable to write offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }
    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map write height ok, min %hu, max %hu, diff %hu, step %hu\n",
        xsize, ysize, minHeight, maxHeight, maxHeight - minHeight, (maxHeight-minHeight)/255);
    return true;
}

bool TheEarth::writeHasDetail()
{
    if (hasDetail==0) return false;
    
    int rc = 0;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_hasDetail.dat", "wb");
    if (error) return false;

    rc = fprintf(f, "%u\n%u\n", xsize, ysize);
    if (rc <= 0)
    {
        printf("unable to write sizes\n");
        fclose(f);
        return false;
    }

    unsigned int twsize = (xsize*ysize)/8+1;
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fwrite(hasDetail+(twsize-i), sizeof(unsigned char), wsize, f);
        if (ret != wsize)
        {
             printf("unable to write road data offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }

    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map write hasDetail ok\n", xsize, ysize);
    return true;
}

/*bool TheEarth::writeHasDetailTex()
{
    if (hasDetailTex==0) return false;
    
    int rc = 0;
    FILE* f;
    errno_t error = fopen_s(&f, "data/earthdata/earth_data_hasDetailTex.dat", "wb");
    if (error) return false;

    rc = fprintf(f, "%u\n%u\n", xsize, ysize);
    if (rc <= 0)
    {
        printf("unable to write sizes\n");
        fclose(f);
        return false;
    }

    unsigned int twsize = (xsize*ysize)/8+1;
    unsigned int i = twsize;
    while (i)
    {
        size_t wsize = 1024;
        if (i < wsize) wsize = (size_t)i;
        size_t ret = fwrite(hasDetailTex+(twsize-i), sizeof(unsigned char), wsize, f);
        if (ret != wsize)
        {
             printf("unable to write road data offset %u / %u\n", twsize-i, twsize);
             fclose(f);
             return false;
        }
        i -= wsize;
    }

    fclose(f);
    dprintf(MY_DEBUG_NOTE, "%ux%u map write hasDetailTex ok\n", xsize, ysize);
    return true;
}*/

bool TheEarth::writeToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const
{
    irr::video::IImageWriter* writer = 0;
    irr::io::path path = "earth_data.png";

    for (unsigned int i = 0; i < driver->getImageWriterCount(); i++)
    {
        irr::video::IImageWriter* tmpwriter = driver->getImageWriter(i);
        if (tmpwriter->isAWriteableFileExtension(path))
        {
            writer = tmpwriter;
            break;
        }
    }

    if (writer == 0)
    {
        printf("There is no writer for [earth_data.png]\n");
        return false;
    }

    bool ret = true;
    ret &= writeHeightToPNG(device, driver);
    ret &= writeHasDetailToPNG(device, driver);
    // ret &= writeHasDetailTexToPNG(device, driver);

    ret &= writeDensityToPNG(device, driver);
    ret &= writeEarthTextureToPNG(device, driver);
    return ret;
}

bool TheEarth::writeHeightToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const
{
    if (height==0) return false;
    if (minHeight == maxHeight)
    {
        printf("min max equal\n");
        return false;
    }
    irr::video::IImage* image = driver->createImage(irr::video::ECF_R8G8B8, irr::core::dimension2d<irr::u32>(xsize, ysize));
    irr::io::path path = "data/earthdata/earth_data_height.png";

    for (unsigned int i = 0; i < xsize; i++)
    {
        for (unsigned int j = 0; j < ysize; j++)
        {
            unsigned int color = (255*(unsigned int)(getEarthHeight(i, j)-minHeight))/(unsigned int)(maxHeight-minHeight);
            image->setPixel(i, j, irr::video::SColor(255, color, color, color));
        }
    }

    bool ret = driver->writeImageToFile(image, device->getFileSystem()->createAndWriteFile(path));
    image->drop();
    return ret;
}

bool TheEarth::writeHasDetailToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const
{
    if (hasDetail==0) return false;

    irr::video::IImage* image = driver->createImage(irr::video::ECF_R8G8B8, irr::core::dimension2d<irr::u32>(xsize, ysize));
    irr::io::path path = "data/earthdata/earth_data_hasDetail.png";

    for (unsigned int i = 0; i < xsize; i++)
    {
        for (unsigned int j = 0; j < ysize; j++)
        {
            unsigned int color = getHasDetail(i, j) ? 255 : 0;
            image->setPixel(i, j, irr::video::SColor(255, color, color, color));
        }
    }

    bool ret = driver->writeImageToFile(image, device->getFileSystem()->createAndWriteFile(path));
    image->drop();
    return ret;
}


bool TheEarth::writeDensityToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const
{
    if (density==0) return false;

    irr::io::path path = "data/earthdata/earth_data_density.png";

    bool ret = driver->writeImageToFile(density, device->getFileSystem()->createAndWriteFile(path));
    return ret;
}

bool TheEarth::writeEarthTextureToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const
{
    if (earthTexture==0) return false;

    irr::io::path path = "data/earthdata/earth_data_texture.png";

    bool ret = driver->writeImageToFile(earthTexture, device->getFileSystem()->createAndWriteFile(path));
    return ret;
}

void TheEarth::createFirst(const irr::core::vector3df& apos, const irr::core::vector3df& dir)
{
    lastCenterPosi = irr::core::vector3di(((int)(apos.X/TILE_SCALE_F))*TILE_SCALE, 0, ((int)(apos.Z/TILE_SCALE_F))*TILE_SCALE);
    lastCenterPos = irr::core::vector3df((float)lastCenterPosi.X, (float)lastCenterPosi.Y, (float)lastCenterPosi.Z);
    lastPosBox = irr::core::aabbox3df(lastCenterPos.X-VISUAL_BOX_HSIZE_F, -1000.0f, lastCenterPos.Z-VISUAL_BOX_HSIZE_F,
        lastCenterPos.X+VISUAL_BOX_HSIZE_F, 10000.0f, lastCenterPos.Z+VISUAL_BOX_HSIZE_F);
    lastLargeCenterPosi = irr::core::vector3di(((int)(apos.X/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE, 0, ((int)(apos.Z/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE);

    if (visualPart)
    {
        delete visualPart;
        visualPart = 0;
    }
    if (newReadyVisualPart)
    {
        delete newReadyVisualPart;
        newReadyVisualPart = 0;
    }
    newVisualPart = 0;
    clearSetInUseFlagsForTiles();
    removeNotInUseTiles();
    RoadManager::getInstance()->clearVisible();

    visualPart = new VisualMembers();
    dprintf(MY_DEBUG_NOTE, "create members ... ");
    visualPart->createMembers(lastCenterPosi, lastLargeCenterPosi, this);
    dprintf(MY_DEBUG_NOTE, "done\nload members ... ");
    visualPart->loadMembers(this);
    dprintf(MY_DEBUG_NOTE, "done\ngenerate road stuff ... ");
    newVisualPart = visualPart;
    RoadManager::getInstance()->generateNewVisual();
    RoadManager::getInstance()->switchToNewVisual();
    newVisualPart = 0;
    dprintf(MY_DEBUG_NOTE, "done\nset visible members ... ");
    visualPart->setVisible(true);
    dprintf(MY_DEBUG_NOTE, "done\nrefresh minimap ... ");
    refreshMiniMap();
    dprintf(MY_DEBUG_NOTE, "done\n");

}

void TheEarth::update(const irr::core::vector3df& apos, const irr::core::vector3df& dir)
{
    //return;
#if 0
    if (!lastPosBox.isPointInside(irr::core::vector3df(apos.X, 0.0f, apos.Z)))
    {
        createFirst(apos, dir);
    }
#else // 0
    if (newVisualPart)
    {
        //printf("under construction\n");
        return;
    }
    else if (newReadyVisualPart)
    {
        dprintf(MY_DEBUG_NOTE, "switch to new, set visible true ... \n");
        delete visualPart;
        visualPart = newReadyVisualPart;
        newReadyVisualPart = 0;
        visualPart->setVisible(true);
        RoadManager::getInstance()->switchToNewVisual();
        dprintf(MY_DEBUG_NOTE, "switch to new, set visible true ... done\n");
        
        return;
    }

    if (!lastPosBox.isPointInside(irr::core::vector3df(apos.X, 0.0f, apos.Z)))
    {
        dprintf(MY_DEBUG_NOTE, "start create new ...\n");
        //irr::core::vector3df compPos = apos;
        irr::core::vector3df compPos = apos + (dir * 200.0f);
        newVisualPart = new VisualMembers();
        lastCenterPosi = irr::core::vector3di(((int)(compPos.X/TILE_SCALE_F))*TILE_SCALE, 0, ((int)(compPos.Z/TILE_SCALE_F))*TILE_SCALE);
        //lastCenterPosi = irr::core::vector3di((int)apos.X, 0, (int)apos.Z);
        lastCenterPos = irr::core::vector3df((float)lastCenterPosi.X, (float)lastCenterPosi.Y, (float)lastCenterPosi.Z);
        lastPosBox = irr::core::aabbox3df(lastCenterPos.X-VISUAL_BOX_HSIZE_F, -100.0f, lastCenterPos.Z-VISUAL_BOX_HSIZE_F,
            lastCenterPos.X+VISUAL_BOX_HSIZE_F, 9000.0f, lastCenterPos.Z+VISUAL_BOX_HSIZE_F);
#if 0
        irr::core::vector3di newLargeCenterPosi = irr::core::vector3di(((int)(apos.X/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE, 0, ((int)(apos.Z/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE);
        if (lastLargeCenterPosi == newLargeCenterPosi)
        {
            newVisualPart->createMembers(lastCenterPosi, lastLargeCenterPosi, this, visualPart->terrainLarge);
            visualPart->removeLarge = false;
        }
        else
        {
            lastLargeCenterPosi = newLargeCenterPosi;
            newVisualPart->createMembers(lastCenterPosi, newLargeCenterPosi, this);
        }
#else
        lastLargeCenterPosi = irr::core::vector3di(((int)(compPos.X/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE, 0, ((int)(compPos.Z/TILE_LARGE_SCALE_F))*TILE_LARGE_SCALE);
        newVisualPart->createMembers(lastCenterPosi, lastLargeCenterPosi, this);
#endif
        dprintf(MY_DEBUG_NOTE, "start create new ... done\n");
        execute();
        refreshMiniMap();
    }
#endif // 0
}

void TheEarth::run()
{
    clearSetInUseFlagsForTiles();
    //RoadManager::getInstance()->clearVisible();
    dprintf(MY_DEBUG_NOTE, "thread: loading visual part ...\n");
    newVisualPart->loadMembers(this);
    dprintf(MY_DEBUG_NOTE, "thread: loading visual part ... done\n");
    dprintf(MY_DEBUG_NOTE, "thread: generate road stuff ... \n");
    RoadManager::getInstance()->generateNewVisual();
    dprintf(MY_DEBUG_NOTE, "thread: generate road stuff ... done\n");
    newReadyVisualPart = newVisualPart;
    newVisualPart = 0;
    removeNotInUseTiles();
}

void TheEarth::registerVisual()
{
    //assert(0);
    if (visualPart)
    {
        visualPart->registerMembers();
    }
}

void TheEarth::refreshMiniMap()
{
    irr::core::dimension2di currentPos(abs(lastCenterPosi.X / TILE_SIZE), abs(lastCenterPosi.Z / TILE_SIZE));

    if (currentPos != lastMiniMapPos)
    {
        lastMiniMapPos = currentPos;
        irr::core::dimension2di generateFrom(currentPos.Width - MINIMAP_HSIZE, currentPos.Height - MINIMAP_HSIZE);

        if (generateFrom.Width < 0) generateFrom.Width = 0;
        if (generateFrom.Height < 0) generateFrom.Height = 0;
        if (generateFrom.Width + MINIMAP_SIZE > (int)xsize) generateFrom.Width = xsize - MINIMAP_SIZE;
        if (generateFrom.Height + MINIMAP_SIZE > (int)ysize) generateFrom.Height = ysize - MINIMAP_SIZE;

        irr::video::SColor color;

        for (int x = generateFrom.Width; x < generateFrom.Width + MINIMAP_SIZE; x++)
        {
            for (int y = generateFrom.Height; y < generateFrom.Height + MINIMAP_SIZE; y++)
            {
                color = getEarthTexture(x, y);
                if (x == currentPos.Width || y == currentPos.Height)
                //if (x == generateFrom.Width+MINIMAP_SIZE-1 || y == generateFrom.Height+MINIMAP_SIZE-1)
                {
                    color.setRed(255);
                }
                miniMap->setPixel(x - generateFrom.Width, y - generateFrom.Height, color);
            }
        }

        char miniMapName[255];
        sprintf_s(miniMapName, "minimap_%d_%d", currentPos.Width, currentPos.Height);
        miniMapTexture = TheGame::getInstance()->getDriver()->addTexture(miniMapName, miniMap);
    }
}

float TheEarth::getHeight(float x, float z)
{
    return visualPart->getHeight(x, z);
}

float TheEarth::getHeight(const irr::core::vector3df& pos)
{
    return getHeight(pos.X, pos.Z);
}

float TheEarth::getHeight(const irr::core::vector2df& pos)
{
    return getHeight(pos.X, pos.Y);
}

float TheEarth::getNewHeight(float x, float z)
{
    return newVisualPart->getHeight(x, z);
}

float TheEarth::getNewHeight(const irr::core::vector3df& pos)
{
    return getNewHeight(pos.X, pos.Z);
}

float TheEarth::getNewHeight(const irr::core::vector2df& pos)
{
    return getNewHeight(pos.X, pos.Y);
}

void TheEarth::clearSetInUseFlagsForTiles()
{
    for (tileMap_t::const_iterator it = tileMap.begin(); it != tileMap.end(); it++)
    {
        it->second->clearInUse();
    }
}

void TheEarth::removeNotInUseTiles()
{
    for (tileMap_t::iterator it = tileMap.begin(); it != tileMap.end(); /*it++*/)
    {
        if (!it->second->isInUse())
        {
            setIsLoaded(it->second->getTileNum(), false);
            delete it->second;

            tileMap_t::iterator tmpIt = it;
            it++;
            tileMap.erase(tmpIt);
        }
        else
        {
            it++;
        }
    }
}
