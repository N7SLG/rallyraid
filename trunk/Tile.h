#ifndef TILE_H
#define TILE_H

#include "Terrain_defs.h"
#include <irrlicht.h>
#include "RoadManager.h"

class Tile
{
public:
    Tile(unsigned int posx, unsigned int posy, unsigned int tileNum,
        const irr::video::SColor& color00,
        const irr::video::SColor& color10,
        const irr::video::SColor& color01,
        const irr::video::SColor& color11,
        const irr::video::SColor& density,
        bool autoRead = true);
    ~Tile();

    unsigned short getHeight(unsigned int x, unsigned int y) const
    {
        if (height && x < TILE_POINTS_NUM && y < TILE_POINTS_NUM)
        {
            return height[x + (TILE_POINTS_NUM*y)];
        }
        else
        {
            return 0;
        }
    }
    
    const irr::video::SColor& getColor(unsigned int x, unsigned int y) const
    {
        if (colors && x < TILE_POINTS_NUM && y < TILE_POINTS_NUM)
        {
            return colors[x + (TILE_POINTS_NUM*y)];
        }
        else
        {
            return baseColor;
        }
    }

    const irr::video::SColor& getFineColor(unsigned int x, unsigned int y) const
    {
        if (fineColors && x < TILE_FINE_POINTS_NUM && y < TILE_FINE_POINTS_NUM)
        {
            return fineColors[x + (TILE_FINE_POINTS_NUM*y)];
        }
        else
        {
            return baseColor;
        }
    }
    
    const irr::video::SColor& getFineDensity(unsigned int x, unsigned int y) const
    {
        if (fineDensity && x < TILE_FINE_POINTS_NUM && y < TILE_FINE_POINTS_NUM)
        {
            return fineDensity[x + (TILE_FINE_POINTS_NUM*y)];
        }
        else
        {
            return baseColor;
        }
    }
    
    void setFineColor(unsigned int x, unsigned int y, const irr::video::SColor& val = irr::video::SColor())
    {
        if (fineColors && x < TILE_FINE_POINTS_NUM && y < TILE_FINE_POINTS_NUM)
        {
            fineColors[x + (TILE_FINE_POINTS_NUM*y)] = val;
        }
    }
    
    void setFineDensity(unsigned int x, unsigned int y, const irr::video::SColor& val = irr::video::SColor())
    {
        if (fineDensity && x < TILE_FINE_POINTS_NUM && y < TILE_FINE_POINTS_NUM)
        {
            fineDensity[x + (TILE_FINE_POINTS_NUM*y)] = val;
        }
    }
    
    void setFineColor(unsigned int x, unsigned int y, int radius = 0, const irr::video::SColor& val = irr::video::SColor())
    {
        if (fineColors && x < TILE_FINE_POINTS_NUM && y < TILE_FINE_POINTS_NUM)
        {
            fineColors[x + (TILE_FINE_POINTS_NUM*y)] = val;
        }
    }
    
    void setFineColorAndZeroDensity(unsigned int x, unsigned int y, unsigned int reqradius, const irr::video::SColor& adjustColor)
    {
        if (!fineColors || !fineDensity) return;
        const unsigned int radius = reqradius;// + 1;
        const int adjustRed = ((int)adjustColor.getRed()-0x80);
        const int adjustGreen = ((int)adjustColor.getGreen()-0x80);
        const int adjustBlue = ((int)adjustColor.getBlue()-0x80);
        //const int adjustRed2 = adjustRed / 2;
        //const int adjustGreen2 = adjustGreen / 2;
        //const int adjustBlue2 = adjustBlue / 2;
        for (unsigned int yy = y - radius, fy = TILE_FINE_POINTS_NUM*(y-radius); yy <= y + radius; yy++, fy += TILE_FINE_POINTS_NUM)
        {
            for (unsigned int xx = x - radius; xx <= x + radius; xx++)
            {
                if (xx < TILE_FINE_POINTS_NUM && yy < TILE_FINE_POINTS_NUM && !getAdjusted(xx + fy))
                {
                    int radjustRed = adjustRed;
                    int radjustGreen = adjustGreen;
                    int radjustBlue = adjustBlue;
                    /*
                    if (yy < y-reqradius || y+reqradius < yy || xx < x-reqradius || x+reqradius < xx)
                    {
                        unsigned int nextAdj = 0;
                        if (yy - 1 < TILE_FINE_POINTS_NUM && getAdjusted((xx)+(fy-TILE_FINE_POINTS_NUM))) nextAdj++;
                        if (yy + 1 < TILE_FINE_POINTS_NUM && getAdjusted((xx)+(fy+TILE_FINE_POINTS_NUM))) nextAdj++;
                        if (xx - 1 < TILE_FINE_POINTS_NUM && getAdjusted((xx-1)+(fy))) nextAdj++;
                        if (xx + 1 < TILE_FINE_POINTS_NUM && getAdjusted((xx+1)+(fy))) nextAdj++;
                        if (nextAdj >= 2)
                        {
                            radjustRed = 255;//adjustRed2;
                            radjustGreen = 255;//adjustGreen2;
                            radjustBlue = 255;//adjustBlue2;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    */
                    fineDensity[xx + fy] = irr::video::SColor(0);
                    fineColors[xx + fy].setAlpha(0);
                    setAdjusted(xx + fy);
                    //fineColors[xx + fy] = val;
                    int newVal = (int)fineColors[xx + fy].getRed() + radjustRed;
                    if (newVal < 0) newVal = 0;
                    if (newVal>0xff) newVal = 0xff;
                    fineColors[xx + fy].setRed(newVal);

                    newVal = (int)fineColors[xx + fy].getGreen() + radjustGreen;
                    if (newVal < 0) newVal = 0;
                    if (newVal>0xff) newVal = 0xff;
                    fineColors[xx + fy].setGreen(newVal);

                    newVal = (int)fineColors[xx + fy].getBlue() + radjustBlue;
                    if (newVal < 0) newVal = 0;
                    if (newVal>0xff) newVal = 0xff;
                    fineColors[xx + fy].setBlue(newVal);
                    /*
                    int newColor = (int)fineColors[xx + fy].color + ((int)adjustColor.color-0x808080);
                    if (newColor > 0)
                    {
                        fineColors[xx + fy].color = (unsigned int)newColor;
                    }
                    else
                    {
                        fineColors[xx + fy].color = 0;
                    }
                    */
                }
            }
        }
    }

private:
    bool getAdjusted(unsigned int pos) const
    {
        return (adjusted[pos/8] & (0x1 << (pos%8))) == (0x1 << (pos%8));
    }

    void setAdjusted(unsigned int pos)
    {
        adjusted[pos/8] |= (0x1 << (pos%8));
    }

public:
    bool getInUse() const {return inUse;}
    bool isInUse() const {return inUse;}
    void setInUse() {inUse = true;}
    void clearInUse() {inUse = false;}

    unsigned int getPosX() const {return posx;}
    unsigned int getPosY() const {return posy;}
    unsigned int getTileNum() const {return tileNum;}

private:
    bool setHeight(unsigned int x, unsigned int y, unsigned short val);
    bool read();


private:
    unsigned int catx;
    unsigned int caty;
    unsigned int posx;
    unsigned int posy;
    unsigned int tileNum;

    unsigned short* height;
    bool inUse;
    irr::video::SColor* colors;
    irr::video::SColor* fineColors;
    irr::video::SColor* fineDensity;
    unsigned char*      adjusted;
    
    roadRoadChunkList_t roadRoadChunkList;

    static const irr::video::SColor baseColor;
    
    
    friend class MenuPageEditor;
};

#endif // TILE_H
