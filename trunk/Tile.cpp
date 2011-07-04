
#include "TheGame.h"
#include "Tile.h"
#include "TheEarth.h"
#include "stdafx.h"

const irr::video::SColor Tile::baseColor;

Tile::Tile(unsigned int posx, unsigned int posy, unsigned int tileNum,
           const irr::video::SColor& color00,
           const irr::video::SColor& color10,
           const irr::video::SColor& color01,
           const irr::video::SColor& color11,
           const irr::video::SColor& density,
           bool autoRead)
    : catx(posx/CATDIV), caty(posy/CATDIV),
      posx(posx), posy(posy), tileNum(tileNum),
      height(new unsigned short[TILE_POINTS_NUM*TILE_POINTS_NUM]),
      inUse(true),
      colors(new irr::video::SColor[TILE_POINTS_NUM*TILE_POINTS_NUM]),
      fineColors(new irr::video::SColor[TILE_FINE_POINTS_NUM*TILE_FINE_POINTS_NUM]),
      fineDensity(new irr::video::SColor[TILE_FINE_POINTS_NUM*TILE_FINE_POINTS_NUM]),
      adjusted(new unsigned char[(TILE_FINE_POINTS_NUM*TILE_FINE_POINTS_NUM)/8+1]),
      roadRoadChunkList()
{
    memset(height, 0, (TILE_POINTS_NUM*TILE_POINTS_NUM)*sizeof(unsigned short));
    memset(adjusted, 0, (TILE_FINE_POINTS_NUM*TILE_FINE_POINTS_NUM)/8+1);
    if (autoRead)
    {
        read();
    }
#if 0
    // x first
    for (unsigned int x = 0; x < TILE_POINTS_NUM; x++)
    {
        const unsigned int cdiv = (TILE_POINTS_NUM + 1);
        const unsigned int cx0 = cdiv - x;
        const unsigned int cx1 = /*cdiv -*/ x;
        const irr::video::SColor color0(255,
              (color00.getRed()*cx0 +   color10.getRed()*cx1) / cdiv,
            (color00.getGreen()*cx0 + color10.getGreen()*cx1) / cdiv,
             (color00.getBlue()*cx0 +  color10.getBlue()*cx1) / cdiv);
        const irr::video::SColor color1(255,
              (color01.getRed()*cx0 +   color11.getRed()*cx1) / cdiv,
            (color01.getGreen()*cx0 + color11.getGreen()*cx1) / cdiv,
             (color01.getBlue()*cx0 +  color11.getBlue()*cx1) / cdiv);
        for (unsigned int y = 0; y < TILE_POINTS_NUM; y++)
        {
            const unsigned int cy0 = cdiv - y;
            const unsigned int cy1 = y;

            const unsigned int red =     (color0.getRed()*cy0 +   color1.getRed()*cy1) / cdiv;
            const unsigned int green = (color0.getGreen()*cy0 + color1.getGreen()*cy1) / cdiv;
            const unsigned int blue =   (color0.getBlue()*cy0 +  color1.getBlue()*cy1) / cdiv;
            //printf("%u %u %u\n", red, green, blue);
            //printf("%u ", red);
            colors[x + (TILE_POINTS_NUM*y)].set(255, red, green, blue);
        }
        //printf("\n\n");
    }

    for (unsigned int x = 0; x < TILE_POINTS_NUM - 1; x++)
    {
        for (unsigned int y = 0; y < TILE_POINTS_NUM - 1; y++)
        {
            const irr::video::SColor& fcolor00 = getColor(x, y);
            const irr::video::SColor& fcolor10 = getColor(x + 1, y);
            const irr::video::SColor& fcolor01 = getColor(x, y + 1);
            const irr::video::SColor& fcolor11 = getColor(x + 1, y + 1);
            
            for (unsigned int fx = 0; fx < TILE_FINE_RATE; fx++)
            {
                const unsigned int cdiv = (TILE_FINE_RATE + 1);
                const unsigned int cx0 = cdiv - fx;
                const unsigned int cx1 = /*cdiv -*/ fx;
                const irr::video::SColor color0(255,
                      (fcolor00.getRed()*cx0 +   fcolor10.getRed()*cx1) / cdiv,
                    (fcolor00.getGreen()*cx0 + fcolor10.getGreen()*cx1) / cdiv,
                     (fcolor00.getBlue()*cx0 +  fcolor10.getBlue()*cx1) / cdiv);
                const irr::video::SColor color1(255,
                      (fcolor01.getRed()*cx0 +   fcolor11.getRed()*cx1) / cdiv,
                    (fcolor01.getGreen()*cx0 + fcolor11.getGreen()*cx1) / cdiv,
                     (fcolor01.getBlue()*cx0 +  fcolor11.getBlue()*cx1) / cdiv);
                for (unsigned int fy = 0; fy < TILE_POINTS_NUM; fy++)
                {
                    const unsigned int cy0 = cdiv - fy;
                    const unsigned int cy1 = fy;
    
                    const unsigned int red =     (color0.getRed()*cy0 +   color1.getRed()*cy1) / cdiv;
                    const unsigned int green = (color0.getGreen()*cy0 + color1.getGreen()*cy1) / cdiv;
                    const unsigned int blue =   (color0.getBlue()*cy0 +  color1.getBlue()*cy1) / cdiv;
                    //printf("%u %u %u\n", red, green, blue);
                    //printf("%u ", red);
                    fineColors[((x*TILE_FINE_RATE+fx)) + (((y*TILE_FINE_RATE)+fy)*TILE_FINE_POINTS_NUM)].set(255, red, green, blue);
                }
                //printf("\n\n");
            }
        }
        //printf("\n\n");
    }
    
    for (unsigned int x = 0; x < TILE_FINE_POINTS_NUM; x++)
    {
        for (unsigned int y = 0; y < TILE_FINE_POINTS_NUM; y++)
        {
            fineDensity[x + (TILE_FINE_POINTS_NUM*y)] = density;
        }
    }
#else // if 0
// y first

    const unsigned int cdiv = (TILE_POINTS_NUM + 1);
    for (unsigned int y = 0, fy = 0; y < TILE_POINTS_NUM; y++, fy+=TILE_POINTS_NUM)
    {
        const unsigned int cy0 = cdiv - y;
        const unsigned int cy1 = /*cdiv -*/ y;
        const irr::video::SColor color0(255,
              (color00.getRed()*cy0 +   color01.getRed()*cy1) / cdiv,
            (color00.getGreen()*cy0 + color01.getGreen()*cy1) / cdiv,
             (color00.getBlue()*cy0 +  color01.getBlue()*cy1) / cdiv);
        const irr::video::SColor color1(255,
              (color10.getRed()*cy0 +   color11.getRed()*cy1) / cdiv,
            (color10.getGreen()*cy0 + color11.getGreen()*cy1) / cdiv,
             (color10.getBlue()*cy0 +  color11.getBlue()*cy1) / cdiv);
        for (unsigned int x = 0; x < TILE_POINTS_NUM; x++)
        {
            const unsigned int cx0 = cdiv - x;
            const unsigned int cx1 = x;

            const unsigned int red =     (color0.getRed()*cx0 +   color1.getRed()*cx1) / cdiv;
            const unsigned int green = (color0.getGreen()*cx0 + color1.getGreen()*cx1) / cdiv;
            const unsigned int blue =   (color0.getBlue()*cx0 +  color1.getBlue()*cx1) / cdiv;
            //printf("%u %u %u\n", red, green, blue);
            //printf("%u ", red);
            colors[x + fy].set(255, red, green, blue);
        }
        //printf("\n\n");
    }

    const unsigned int cdivf = (TILE_FINE_POINTS_NUM + 1);
    for (unsigned int y = 0, fy = 0; y < TILE_FINE_POINTS_NUM; y++, fy+=TILE_FINE_POINTS_NUM)
    {
        const unsigned int cy0 = cdivf - y;
        const unsigned int cy1 = /*cdiv -*/ y;
        const irr::video::SColor color0(255,
              (color00.getRed()*cy0 +   color01.getRed()*cy1) / cdivf,
            (color00.getGreen()*cy0 + color01.getGreen()*cy1) / cdivf,
             (color00.getBlue()*cy0 +  color01.getBlue()*cy1) / cdivf);
        const irr::video::SColor color1(255,
              (color10.getRed()*cy0 +   color11.getRed()*cy1) / cdivf,
            (color10.getGreen()*cy0 + color11.getGreen()*cy1) / cdivf,
             (color10.getBlue()*cy0 +  color11.getBlue()*cy1) / cdivf);
        for (unsigned int x = 0; x < TILE_FINE_POINTS_NUM; x++)
        {
            const unsigned int cx0 = cdivf - x;
            const unsigned int cx1 = x;

            const unsigned int red =     (color0.getRed()*cx0 +   color1.getRed()*cx1) / cdivf;
            const unsigned int green = (color0.getGreen()*cx0 + color1.getGreen()*cx1) / cdivf;
            const unsigned int blue =   (color0.getBlue()*cx0 +  color1.getBlue()*cx1) / cdivf;
            //printf("%u %u %u\n", red, green, blue);
            //printf("%u ", red);
            fineColors[x + fy].set(255, red, green, blue);
        }
        //printf("\n\n");
    }

#if 0
    const unsigned int cdivf = (TILE_FINE_RATE + 1);
    for (unsigned int y = 0, yy = 0; y < TILE_POINTS_NUM-1; y++, yy += TILE_FINE_RATE)
    {
        for (unsigned int x = 0, xx = 0; x < TILE_POINTS_NUM-1; x++, xx+=TILE_FINE_RATE)
        {
            const irr::video::SColor& fcolor00 = getColor(x, y);
            const irr::video::SColor& fcolor10 = getColor(x + 1, y);
            const irr::video::SColor& fcolor01 = getColor(x, y + 1);
            const irr::video::SColor& fcolor11 = getColor(x + 1, y + 1);
            
            for (unsigned int fy = 0, ffy = yy * TILE_FINE_POINTS_NUM; fy < TILE_FINE_RATE; fy++, ffy+=TILE_FINE_POINTS_NUM)
            {
                const unsigned int cy0 = cdivf - fy;
                const unsigned int cy1 = /*cdiv -*/ fy;
                const irr::video::SColor color0(255,
                      (fcolor00.getRed()*cy0 +   fcolor01.getRed()*cy1) / cdivf,
                    (fcolor00.getGreen()*cy0 + fcolor01.getGreen()*cy1) / cdivf,
                     (fcolor00.getBlue()*cy0 +  fcolor01.getBlue()*cy1) / cdivf);
                const irr::video::SColor color1(255,
                      (fcolor10.getRed()*cy0 +   fcolor11.getRed()*cy1) / cdivf,
                    (fcolor10.getGreen()*cy0 + fcolor11.getGreen()*cy1) / cdivf,
                     (fcolor10.getBlue()*cy0 +  fcolor11.getBlue()*cy1) / cdivf);
                for (unsigned int fx = 0; fx < TILE_POINTS_NUM; fx++)
                {
                    const unsigned int cx0 = cdivf - fx;
                    const unsigned int cx1 = fx;
    
                    const unsigned int red =     (color0.getRed()*cx0 +   color1.getRed()*cx1) / cdivf;
                    const unsigned int green = (color0.getGreen()*cx0 + color1.getGreen()*cx1) / cdivf;
                    const unsigned int blue =   (color0.getBlue()*cx0 +  color1.getBlue()*cx1) / cdivf;
                    //printf("%u %u %u\n", red, green, blue);
                    //printf("%u ", red);
                    fineColors[xx + fx + ffy].set(255, red, green, blue);
                }
                //printf("\n\n");
            }
        }
        //printf("\n\n");
    }
#endif
    for (unsigned int y = 0, fy = 0; y < TILE_FINE_POINTS_NUM; y++, fy+=TILE_FINE_POINTS_NUM)
    {
        for (unsigned int x = 0; x < TILE_FINE_POINTS_NUM; x++)
        {
            fineDensity[x + fy] = density;
        }
    }
#endif // if 0
    dprintf(MY_DEBUG_NOTE, "tile pos(%u): %u, %u\n", posx + (TheEarth::getInstance()->getSizeX()*posy), posx, posy);
    /*
    dprintf(MY_DEBUG_NOTE, "%u %u %u \n", color00.getRed(), color00.getGreen(), color00.getBlue());
    dprintf(MY_DEBUG_NOTE, "%u %u %u \n", color10.getRed(), color10.getGreen(), color10.getBlue());
    dprintf(MY_DEBUG_NOTE, "%u %u %u \n", color01.getRed(), color01.getGreen(), color01.getBlue());
    dprintf(MY_DEBUG_NOTE, "%u %u %u \n", color11.getRed(), color11.getGreen(), color11.getBlue());
    dprintf(MY_DEBUG_NOTE, "\n");
    */
    //assert(0);
    //RoadManager::getInstance()->addChunkListToVisible(roadRoadChunkList, this);
    RoadManager::getInstance()->setVisibleStageRoad(TheEarth::getInstance()->calculateTileNum(posx, posy), this);
}

Tile::~Tile()
{
    //RoadManager::getInstance()->removeChunkListToVisible(roadRoadChunkList, this);
    RoadManager::getInstance()->setInvisibleStageRoad(TheEarth::getInstance()->calculateTileNum(posx, posy), this);

    if (height)
    {
        delete [] height;
        height = 0;
    }
    if (colors)
    {
        delete [] colors;
        colors = 0;
    }
    if (fineColors)
    {
        delete [] fineColors;
        fineColors = 0;
    }
    if (adjusted)
    {
        delete [] adjusted;
        adjusted = 0;
    }
}

bool Tile::setHeight(unsigned int x, unsigned int y, unsigned short val)
{
    if (height && x < TILE_POINTS_NUM && y < TILE_POINTS_NUM)
    {
        height[x + (TILE_POINTS_NUM*y)] = val;
        //if (val < minHeight) minHeight = val;
        //if (val > maxHeight) maxHeight = val;
        return true;
    }
    else
    {
        return false;
    }
}

bool Tile::read()
{
    int rc = 0;
    char filename[256];
    char filenameRRC[256];
    char zipfilename[256];
    char dirname[256];
    sprintf_s(dirname, "data/earthdata/tiles/%d_%d", catx, caty);
    sprintf_s(zipfilename, "data/earthdata/tiles/%d_%d.zip", catx, caty);
    sprintf_s(filename, "%s/%d_%d.dat", dirname, posx, posy);
    sprintf_s(filenameRRC, "%s/%d_%d.rrc", dirname, posx, posy);

    TheGame::getInstance()->getDevice()->getFileSystem()->addFileArchive(zipfilename, false, false, irr::io::EFAT_ZIP);
    irr::io::IReadFile* file = TheGame::getInstance()->getDevice()->getFileSystem()->createAndOpenFile(filename);
    
    if (!file)
    {
        printf("%s tile file not found\n", filename);
        assert(0);
        return false;
    }

    if (!height)    
    {
        height = new unsigned short[TILE_POINTS_NUM*TILE_POINTS_NUM];
        memset(height, 0, (TILE_POINTS_NUM*TILE_POINTS_NUM)*sizeof(unsigned short));
    }

    file->seek(6);    
    rc = file->read(height, (TILE_POINTS_NUM*TILE_POINTS_NUM)*sizeof(unsigned short));
    file->drop();

    //RoadManager::readRoadRoadChunk(filenameRRC, roadRoadChunkList, RoadManager::getInstance()->getRoadMap());

    return true;
}
