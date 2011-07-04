
#ifndef VISUALROAD_H
#define VISUALROAD_H

#include <irrlicht.h>
#include "hk.h"
#include <list>
#include "RoadManager.h"

class OffsetObject;

class VisualRoad
{
private:
    typedef std::list<irr::core::vector3df> pointList_t;

public:
    VisualRoad(const RoadRoadChunk& roadRoadChunk);
    ~VisualRoad();

    void switchToVisible();

private:
    void generateRoadNode();
    
private:
    RoadRoadChunk           roadRoadChunk;
    OffsetObject*           offsetObject;
    irr::scene::ISceneNode* roadNode;
    irr::scene::SAnimatedMesh* animatedMesh;

    hkpRigidBody* hkBody;
    hkpShape* hkShape;
};

#endif // VISUALROAD_H

