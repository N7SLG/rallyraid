
#ifndef ROADTYPEMANAGER_H
#define ROADTYPEMANAGER_H

#include <string>
#include <map>

class RoadType;

class RoadTypeManager
{
public:
    static void initialize();
    static void finalize();
    
    static RoadTypeManager* getInstance() {return roadTypeManager;}

    typedef std::map<std::string, RoadType*> roadTypeMap_t;

private:
    static RoadTypeManager* roadTypeManager;

private:
    RoadTypeManager();
    ~RoadTypeManager();

    void read();

public:
    RoadType* getRoadType(const std::string& roadTypeName); // inline
    const roadTypeMap_t& getRoadTypeMap(); // inline

private:
    roadTypeMap_t   roadTypeMap;

    RoadType*       editorRoadType;


    friend class MenuPageEditor;
    friend class MenuPageEditorRoad;
    friend class MenuPageEditorRoadType;
    friend class MenuPageEditorRace;
    friend class MenuPageEditorDay;
    friend class MenuPageEditorStage;
};


inline RoadType* RoadTypeManager::getRoadType(const std::string& roadTypeName)
{
    roadTypeMap_t::const_iterator rit = roadTypeMap.find(roadTypeName);
    if (rit == roadTypeMap.end())
    {
        return 0;
    }
    return rit->second;
}

inline const RoadTypeManager::roadTypeMap_t& RoadTypeManager::getRoadTypeMap()
{
    return roadTypeMap;
}

#endif // ROADTYPEMANAGER_H
