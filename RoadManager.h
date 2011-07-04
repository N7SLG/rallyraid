
#ifndef ROADMANAGER_H
#define ROADMANAGER_H

#include <string>
#include <map>
#include <list>
#include <set>
#include <utility>
#include <irrlicht.h>

class Road;
class VisualRoad;
class Tile;

typedef std::pair<unsigned int, unsigned int> roadChunk_t;

struct RoadRoadChunk
{
    Road*       road;
    roadChunk_t roadChunk;

    bool operator<(const RoadRoadChunk& other) const
    {
        return road < other.road ||
            (road == other.road && roadChunk.first < other.roadChunk.first);
    }
};
typedef std::list<RoadRoadChunk> roadRoadChunkList_t;   // store data for stages road chunks, and for tile chunks
typedef std::set<RoadRoadChunk> roadRoadChunkSet_t;    // maintain visible parts

class RoadManager
{
public:
    static void initialize();
    static void finalize();
    
    static RoadManager* getInstance() {return roadManager;}

    typedef std::map<std::string, Road*>                roadMap_t;
    typedef std::map<unsigned int, roadRoadChunkList_t> stageRoadChunkListMap_t;
    typedef std::list<VisualRoad*>                      visualRoadList_t;

private:
    static RoadManager* roadManager;

private:
    RoadManager();
    ~RoadManager();

    void read();

public:
    Road* getRoad(const std::string& roadName); // inline
    const roadMap_t& getRoadMap(); // inline

    void addStageRoad(Road* road);  // call when start a new stage: add race, day and stage roads
    void addStageRoad(const roadMap_t& p_roadMap);  // call when start a new stage: add race, day and stage roads
    void clearStageRoads();         // call when a stage is ended

    // -------------------------------------------------------------------
    // these stuffs are called from the earth visible part builder thread!
    // -------------------------------------------------------------------
    
    // call when start to a brand new earth stuff, called from earth
    void clearVisible();

    // call when a tile become visible, called from setVisibleStageRoad and from tiles constructor, called from thread
    void addChunkListToVisible(const roadRoadChunkList_t& roadRoadChunkList, Tile* tile);
    // call from the tile which become visible, called from thread
    void setVisibleStageRoad(unsigned int tileNum, Tile* tile);

    // call when a tile become invisible, called from setInvisibleStageRoad and from tiles destructor, called from thread
    void removeChunkListToVisible(const roadRoadChunkList_t& roadRoadChunkList, Tile* tile);
    // call from the tile which become invisible, called from thread
    void setInvisibleStageRoad(unsigned int tileNum, Tile* tile);

    // called from the thread
    void generateNewVisual();

    // call from the earth stuff, the non-threaded part at the end
    void switchToNewVisual();

    // general static finctions
    static void readRoads(const std::string& dirName, roadMap_t& roadMap, bool global, bool doRead = false);
    static bool readRoadRoadChunk(const std::string& fileName, roadRoadChunkList_t& roadRoadChunkList, const RoadManager::roadMap_t& roadMap);
    static bool writeRoadRoadChunk(const std::string& fileName, const roadRoadChunkList_t& roadRoadChunkList);
    static void clearRoadMap(roadMap_t& roadMap);
    //static bool writeGlobalObjects(const std::string& fileName, const globalObjectList_t& globalObjectList);

private:
    static void editorRenderRoads(const RoadManager::roadMap_t& roadMap);

private:
    roadMap_t               roadMap;
    stageRoadChunkListMap_t stageRoadChunkListMap;
    roadRoadChunkSet_t      visibleRoadChunkSet;
    visualRoadList_t        visualRoadList;
    visualRoadList_t        visualRoadListNew;

    Road*                   editorRoad;
    int                     editorRadius;
    irr::video::SColor      editorColor;


    friend class Road;
    friend class MenuPageEditor;
    friend class MenuPageEditorRoad;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
};


inline Road* RoadManager::getRoad(const std::string& roadName)
{
    roadMap_t::const_iterator rit = roadMap.find(roadName);
    if (rit == roadMap.end())
    {
        return 0;
    }
    return rit->second;
}

inline const RoadManager::roadMap_t& RoadManager::getRoadMap()
{
    return roadMap;
}

#endif // ROADMANAGER_H
