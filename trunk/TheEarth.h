
#ifndef THEEARTH_H
#define THEEARTH_H

#include <irrlicht.h>
#include <map>
#include "Tile.h"
#include "MyThread.h"

namespace irr
{
    namespace video
    {
        class IImage;
        class SColor;
    }
}
class Terrain;

#define MINIMAP_SIZE 128
#define MINIMAP_HSIZE (MINIMAP_SIZE / 2)

class TheEarth : public MyThread
{
public:
    static void initialize();
    static void finalize();
    
    static TheEarth* getInstance() {return theEarth;}
    
private:
    static TheEarth* theEarth;
    static irr::video::SColor baseColor;
    
private:
    typedef std::map<unsigned int, Tile*> tileMap_t;

private:
    TheEarth();
    virtual ~TheEarth();

    virtual void run();

    bool setEarthHeight(unsigned int x, unsigned int y, unsigned short val);
    bool setHasDetail(unsigned int x, unsigned int y, bool val);
    bool setIsLoaded(unsigned int x, unsigned int y, bool val);
    //bool setHasDetailTex(unsigned int x, unsigned int y, bool val);
    bool setEarthDensity(unsigned int x, unsigned int y, const irr::video::SColor& val);
    void refreshMiniMap();
    void clearSetInUseFlagsForTiles();
    void removeNotInUseTiles();
    
public:
    unsigned short getTileHeight(unsigned int x, unsigned int y); // x and y devide by TILE_SCALE
    const irr::video::SColor& getTileTexture(unsigned int x, unsigned int y); // x and y devide by TILE_SCALE
    void getTileHeightAndTexture(unsigned int x, unsigned int y, unsigned short& height, irr::video::SColor& textureColor); // x and y devide by TILE_SCALE
    const irr::video::SColor& getTileFineTexture(unsigned int x, unsigned int y);   // x and y are not devided by TILE_SCALE, but TILE_FINE_SCALE
    const irr::video::SColor& getTileFineDensity(unsigned int x, unsigned int y);   // x and y are not devided by TILE_SCALE, but TILE_FINE_SCALE
    void setTileFineTexture(unsigned int x, unsigned int y, const irr::video::SColor& val = irr::video::SColor()); // x and y are not devided by TILE_SCALE, but TILE_FINE_SCALE
    void setTileFineDensity(unsigned int x, unsigned int y, const irr::video::SColor& val = irr::video::SColor()); // x and y are not devided by TILE_SCALE, but TILE_FINE_SCALE
        
    unsigned short getEarthHeight(unsigned int x, unsigned int y) const;
          // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    bool getHasDetail(unsigned int x, unsigned int y) const;
                      // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    bool getIsLoaded(unsigned int x, unsigned int y) const;
                       // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    //bool getHasDetailTex(unsigned int x, unsigned int y) const;
                   // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    irr::video::SColor getEarthDensity(unsigned int x, unsigned int y) const;
     // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    irr::video::SColor getEarthTexture(unsigned int x, unsigned int y) const;
     // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline
    void getEarthHeightAndTexture(unsigned int x, unsigned int y, unsigned short& p_height, irr::video::SColor& textureColor) const;
    // x and y devide by TILE_SCALE * TILE_POINTS_NUM // inline


private:
    unsigned short getEarthHeight(unsigned int tileNum) const;
            // tileNum < xsize*ysize, tileNum = tileX + (xsize*tileY), tileX and tileY are devided by TILE_SCALE * TILE_POINTS_NUM, inline
    bool getHasDetail(unsigned int tileNum) const;
                        // tileNum < xsize*ysize, tileNum = tileX + (xsize*tileY), tileX and tileY are devided by TILE_SCALE * TILE_POINTS_NUM, inline
    bool getIsLoaded(unsigned int tileNum) const;
                         // tileNum < xsize*ysize, tileNum = tileX + (xsize*tileY), tileX and tileY are devided by TILE_SCALE * TILE_POINTS_NUM, inline
    void setIsLoaded(unsigned int tileNum, bool val);
                         // tileNum < xsize*ysize, tileNum = tileX + (xsize*tileY), tileX and tileY are devided by TILE_SCALE * TILE_POINTS_NUM, inline
    //bool getHasDetailTex(unsigned int tileNum) const;
                     // tileNum < xsize*ysize, tileNum = tileX + (xsize*tileY), tileX and tileY are devided by TILE_SCALE * TILE_POINTS_NUM, inline


public:

    bool read();
    bool readHeight();
    bool readHasDetail();
    //bool readHasDetailTex();
    bool readDensity();
    bool readEarthTexture();
    
    bool write();
    bool writeHeight();
    bool writeHasDetail();
    //bool writeHasDetailTex();
    
    bool writeToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const;
    bool writeHeightToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const;
    bool writeHasDetailToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const;

    bool writeDensityToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const;
    bool writeEarthTextureToPNG(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver) const;

    // called by GamePlay::startGame()
    void createFirst(const irr::core::vector3df& apos, const irr::core::vector3df& dir);
    
    // called by TheGame::loop() if small time elapsed
    void update(const irr::core::vector3df& apos, const irr::core::vector3df& dir);
    
    // called by the TheGame::loop()
    void registerVisual();

    irr::video::ITexture* getMiniMapTexture() {return miniMapTexture;}

    float getHeight(float x, float z);
    float getHeight(const irr::core::vector3df& pos);
    float getHeight(const irr::core::vector2df& pos);
    
    float getNewHeight(float x, float z);
    float getNewHeight(const irr::core::vector3df& pos);
    float getNewHeight(const irr::core::vector2df& pos);
    
    unsigned int getSizeX() {return xsize;}
    unsigned int getSizeY() {return ysize;}

    unsigned int calculateTileNum(unsigned int x, unsigned int y) const; // inline
    bool threadIsRunning() {return newVisualPart!=0;}
    
    irr::video::ITexture* getSmokeTexture(); // inline
    irr::video::E_MATERIAL_TYPE getSmokeMaterial(); // inline

private:
    const tileMap_t& getTileMap() {return tileMap;}

private:
    // common data
    
    // visual data - real Z coord
    irr::video::IImage* earthTexture;
    struct VisualMembers
    {
        VisualMembers();
        ~VisualMembers();
        
        void setVisible(bool visible);
        void createMembers(const irr::core::vector3di& centerPosi, const irr::core::vector3di& largeCenterPosi, TheEarth* earth, Terrain* lastTerrainLarge = 0);
        void loadMembers(TheEarth* earth);
        void finalizeMembers();
        void registerMembers();
        float getHeight(float x, float z);
        
        Terrain* terrainCircle[3][3];
        Terrain* terrainLarge;
        bool removeLarge;
        
        static const irr::core::vector3di terrainPos[3][3];
        static const irr::core::vector3df terrainPosf[3][3];
        static const irr::core::vector3di terrainLargePos;
        static const irr::core::vector3df terrainLargePosf;
    };
    
    VisualMembers*      visualPart;
    VisualMembers*      newVisualPart;
    VisualMembers*      newReadyVisualPart;
    
    irr::core::aabbox3df    lastPosBox;
    irr::core::vector3df    lastCenterPos;
    irr::core::vector3di    lastCenterPosi;
    irr::core::vector3di    lastLargeCenterPosi;

    irr::video::IImage*     miniMap;
    irr::video::ITexture*   miniMapTexture;
    irr::core::dimension2di lastMiniMapPos;
    irr::video::ITexture*   smokeTexture;
    irr::video::E_MATERIAL_TYPE smokeMaterial;
    
    
    // store data - reveresed Z coord
    unsigned int        xsize;
    unsigned int        ysize;

    unsigned short*     height;
    irr::video::IImage* density;
    unsigned char*      hasDetail;
    unsigned char*      isLoaded;
    //unsigned char*      hasDetailTex;

    unsigned short      maxHeight;
    unsigned short      minHeight;
    
    tileMap_t           tileMap;
    
    
    friend class MenuPageEditor;

};


inline unsigned short TheEarth::getEarthHeight(unsigned int x, unsigned int y) const
{
    if (height && x < xsize && y < ysize)
    {
        //return height[x + (xsize*y)];
        return getEarthHeight(x + (xsize*y));
    }
    else
    {
        return 0;
    }
}

inline bool TheEarth::getHasDetail(unsigned int x, unsigned int y) const
{
    unsigned int pos = (x + (xsize*y));
    if (hasDetail && x < xsize && y < ysize)
    {
        //return (hasDetail[pos/8] & (0x1 << (pos%8))) == (0x1 << (pos%8));
        return getHasDetail(pos);
    }
    else
    {
        return false;
    }
}

inline bool TheEarth::getIsLoaded(unsigned int x, unsigned int y) const
{
    unsigned int pos = (x + (xsize*y));
    if (isLoaded && x < xsize && y < ysize)
    {
        //return (isLoaded[pos/8] & (0x1 << (pos%8))) == (0x1 << (pos%8));
        return getIsLoaded(pos);
    }
    else
    {
        return false;
    }
}

/*inline bool TheEarth::getHasDetailTex(unsigned int x, unsigned int y) const
{
    unsigned int pos = (x + (xsize*y));
    if (hasDetailTex && x < xsize && y < ysize)
    {
        return (hasDetailTex[pos/8] & (0x1 << (pos%8))) == (0x1 << (pos%8));
    }
    else
    {
        return false;
    }
}*/

inline irr::video::SColor TheEarth::getEarthDensity(unsigned int x, unsigned int y) const
{
    if (density && xsize > 0 && ysize > 0)
    {
        if (x >= xsize) x = xsize - 1;
        if (y >= ysize) y = ysize - 1;
        return density->getPixel(x, y);
    }
    else
    {
        return baseColor;
    }
}

inline irr::video::SColor TheEarth::getEarthTexture(unsigned int x, unsigned int y) const
{
    if (earthTexture && xsize > 0 && ysize > 0)
    {
        if (x >= xsize) x = xsize - 1;
        if (y >= ysize) y = ysize - 1;
        return earthTexture->getPixel(x, y);
    }
    else
    {
        return baseColor;
    }
}

inline void TheEarth::getEarthHeightAndTexture(unsigned int x, unsigned int y, unsigned short& p_height, irr::video::SColor& textureColor) const
{
    if (height && earthTexture && xsize > 0 && ysize > 0)
    {
        if (x >= xsize) x = xsize - 1;
        if (y >= ysize) y = ysize - 1;
        p_height = getEarthHeight(x + (xsize*y));
        textureColor = earthTexture->getPixel(x, y);
    }
    else
    {
        p_height = 0;
    }
}

inline bool TheEarth::setEarthHeight(unsigned int x, unsigned int y, unsigned short val)
{
    if (height && x < xsize && y < ysize)
    {
        height[x + (xsize*y)] = val;
        if (val < minHeight) minHeight = val;
        if (val > maxHeight) maxHeight = val;
        return true;
    }
    else
    {
        return false;
    }
}

inline bool TheEarth::setHasDetail(unsigned int x, unsigned int y, bool val)
{
    unsigned int pos = (x + (xsize*y));
    if (hasDetail && x < xsize && y < ysize)
    {
        if (val)
        {
            hasDetail[pos/8] |= (0x1 << (pos%8));
        }
        else
        {
            hasDetail[pos/8] &= ~(0x1 << (pos%8));
        }
        return true;
    }
    else
    {
        return false;
    }
}

inline bool TheEarth::setIsLoaded(unsigned int x, unsigned int y, bool val)
{
    unsigned int pos = (x + (xsize*y));
    if (isLoaded && x < xsize && y < ysize)
    {
        /*if (val)
        {
            isLoaded[pos/8] |= (0x1 << (pos%8));
        }
        else
        {
            isLoaded[pos/8] &= ~(0x1 << (pos%8));
        }
        */
        setIsLoaded(pos, val);
        return true;
    }
    else
    {
        return false;
    }
}

/*inline bool TheEarth::setHasDetailTex(unsigned int x, unsigned int y, bool val)
{
    unsigned int pos = (x + (xsize*y));
    if (hasDetailTex && x < xsize && y < ysize)
    {
        if (val)
        {
            hasDetailTex[pos/8] |= (0x1 << (pos%8));
        }
        else
        {
            hasDetailTex[pos/8] &= ~(0x1 << (pos%8));
        }
        return true;
    }
    else
    {
        return false;
    }
}*/

inline bool TheEarth::setEarthDensity(unsigned int x, unsigned int y, const irr::video::SColor& val)
{
    if (density && xsize > 0 && ysize > 0)
    {
        if (x >= xsize) x = xsize - 1;
        if (y >= ysize) y = ysize - 1;
        density->setPixel(x, y, val);
        return true;
    }
    else
    {
        return false;
    }
}


inline unsigned short TheEarth::getEarthHeight(unsigned int tileNum) const
{
    return height[tileNum];
}

inline bool TheEarth::getHasDetail(unsigned int tileNum) const
{
    return (hasDetail[tileNum/8] & (0x1 << (tileNum%8))) == (0x1 << (tileNum%8));
}

inline bool TheEarth::getIsLoaded(unsigned int tileNum) const
{
    return (isLoaded[tileNum/8] & (0x1 << (tileNum%8))) == (0x1 << (tileNum%8));
}

inline void TheEarth::setIsLoaded(unsigned int tileNum, bool val)
{
    if (val)
    {
        isLoaded[tileNum/8] |= (0x1 << (tileNum%8));
    }
    else
    {
        isLoaded[tileNum/8] &= ~(0x1 << (tileNum%8));
    }
}

inline unsigned int TheEarth::calculateTileNum(unsigned int x, unsigned int y) const
{
    return (x + (xsize*y));
}


inline irr::video::ITexture* TheEarth::getSmokeTexture()
{
    return smokeTexture;
}

inline irr::video::E_MATERIAL_TYPE TheEarth::getSmokeMaterial()
{
    return smokeMaterial;
}

#endif // THEEARTH_H
