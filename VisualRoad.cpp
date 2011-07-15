
#include "VisualRoad.h"
#include "Settings.h"
#include <assert.h>
#include "TheEarth.h"
#include "TheGame.h"
#include "RoadManager.h"
#include "Road.h"
#include "RoadType.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "ObjectPool.h"
#include "stdafx.h"
#include "Shaders.h"
#include "ShadowRenderer.h"

//#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
//#include <Physics/Collide/Shape/Compound/Collection/Mesh/hkpMeshMaterial.h>
//#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.inl>


VisualRoad::VisualRoad(const RoadRoadChunk& roadRoadChunk)
    : roadRoadChunk(roadRoadChunk),
      offsetObject(0),
      roadNode(0),
      animatedMesh(0),
      hkBody(0),
      hkShape(0)
{
    dprintf(MY_DEBUG_NOTE, "VisualRoad::VisualRoad(): %s: %u - %u\n", roadRoadChunk.road->getRoadType()->getName().c_str(), roadRoadChunk.roadChunk.first, roadRoadChunk.roadChunk.second);
    //assert(0);
    generateRoadNode();
}

VisualRoad::~VisualRoad()
{
    if (roadNode)
    {
        roadNode->setVisible(false);
        roadNode->remove();
    }
    if (hkBody)
    {
        hkBody->removeReference();
        hk::hkWorld->removeEntity(hkBody);
    }
    hkBody = 0;
    if (hkShape)
    {
        hkShape->removeReference();
    }
    hkShape = 0;
    if (offsetObject)
    {
        offsetObject->setNode(0);
        offsetObject->setBody(0);
        offsetObject->removeFromManager();
        delete offsetObject;
        offsetObject = 0;
    }
}

void VisualRoad::generateRoadNode()
{
    assert(roadNode == 0);
    /*
    if (roadNode)
    {
        roadNode->drop();
    }
    roadNode = 0;

    if (hkBody)
    {
        hkBody->removeReference();
        hk::hkWorld->removeEntity(hkBody);
    }
    hkBody = 0;
    if (hkShape)
    {
        hkShape->removeReference();
    }
    hkShape = 0;

    if (offsetObject)
    {
        offsetObject->setBody(0, 0);
        offsetObject->setNode(0);
        offsetManager->removeObject(offsetObject);
    }
    */
    if (roadRoadChunk.roadChunk.first >= roadRoadChunk.roadChunk.second)
    {
        return;
    }
    
    OffsetManager* offsetManager = OffsetManager::getInstance();
    irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();
    irr::scene::SMesh* mesh = new irr::scene::SMesh();

    float ty = 0.f;
    int vertexCount = 0;
    const Road::roadPointVector_t& basePoints = roadRoadChunk.road->getRoadPointVector();
    RoadType* roadType = roadRoadChunk.road->getRoadType();
    vector3dd basePoint(basePoints[roadRoadChunk.roadChunk.first].p.X, 0.f, basePoints[roadRoadChunk.roadChunk.first].p.Z);
    for (unsigned int i = roadRoadChunk.roadChunk.first; i <= roadRoadChunk.roadChunk.second; i++)
    {
        irr::core::vector2df normal;
        if (i == roadRoadChunk.roadChunk.first)
        {
            normal = irr::core::vector2df((float)(basePoints[i+1].p.X - basePoints[i].p.X), (float)(basePoints[i+1].p.Z - basePoints[i].p.Z));
        }
        else if (i == roadRoadChunk.roadChunk.second)
        {
            normal = irr::core::vector2df((float)(basePoints[i].p.X - basePoints[i-1].p.X), (float)(basePoints[i].p.Z - basePoints[i-1].p.Z));
        }
        else
        {
            normal = irr::core::vector2df((float)(basePoints[i+1].p.X - basePoints[i-1].p.X), (float)(basePoints[i+1].p.Z - basePoints[i-1].p.Z));
        }
        normal.normalize();
        normal.rotateBy(-90.f);
        
        //float h0 = p_smallTerrain->terrain->getHeight((roadType->slicePoints[0].X*normal.X)+basePoints[i].X+(offsetManager->getOffset().X*(float)regenerate), (roadType->slicePoints[0].X*normal.Y)+basePoints[i].Z+(offsetManager->getOffset().Z*(float)regenerate));
        //float hb = p_smallTerrain->terrain->getHeight(basePoints[i].X+(offsetManager->getOffset().X*(float)regenerate), basePoints[i].Z+(offsetManager->getOffset().Z*(float)regenerate));
        //float hl = p_smallTerrain->terrain->getHeight((roadType->slicePoints[roadType->slicePoints.size()-1].X*normal.X)+basePoints[i].X+(offsetManager->getOffset().X*(float)regenerate), (roadType->slicePoints[roadType->slicePoints.size()-1].X*normal.Y)+basePoints[i].Z+(offsetManager->getOffset().Z*(float)regenerate));

        float tx = 0.f;
        for (unsigned int j = 0; j < roadType->slicePoints.size(); j++)
        {
            irr::video::S3DVertex vtx;
#if 0
            if (j == 0 && /*roadType->slicePoints.size() > 3*/roadType->friction_multi > 0.01f)
            {
                vtx.Pos = vector3df(((roadType->slicePoints[j+1].X-((roadType->slicePoints[j+1].X-roadType->slicePoints[j].X)*10.f))*normal.X)+basePoints[i].X,
                                    (roadType->slicePoints[j].Y*10.f)+basePoints[i].Y+addHeight,
                                    ((roadType->slicePoints[j+1].X-((roadType->slicePoints[j+1].X-roadType->slicePoints[j].X)*10.f))*normal.Y)+basePoints[i].Z);
            }
            else
            if (j == roadType->slicePoints.size()-1 && /*roadType->slicePoints.size() > 3*/roadType->friction_multi > 0.01f)
            {
                vtx.Pos = vector3df(((roadType->slicePoints[j-1].X+((roadType->slicePoints[j].X-roadType->slicePoints[j-1].X)*10.f))*normal.X)+basePoints[i].X,
                                    (roadType->slicePoints[j].Y*10.f)+basePoints[i].Y+addHeight,
                                    ((roadType->slicePoints[j-1].X+((roadType->slicePoints[j].X-roadType->slicePoints[j-1].X)*10.f))*normal.Y)+basePoints[i].Z);
            }
            else
#endif // 0
            {
                vector3dd ppos((double)(roadType->slicePoints[j].X*normal.X)+basePoints[i].p.X,
                    (double)roadType->slicePoints[j].Y,
                    (double)(roadType->slicePoints[j].X*normal.Y)+basePoints[i].p.Z);
                vtx.Pos.X = (float)(ppos.X - basePoint.X);
                vtx.Pos.Y = (float)(ppos.Y - basePoint.Y);
                vtx.Pos.Z = (float)(ppos.Z - basePoint.Z);
#if 0
                if (roadType->slicePoints.size() == 2)
                {
                    if (j == 0)
                    {
                        if (h0 > 0.01f)
                            vtx.Pos.Y = roadType->slicePoints[j].Y+basePoints[i].Y+h0;
                    }
                    else
                    {
                        if (hl > 0.01f)
                            vtx.Pos.Y = roadType->slicePoints[j].Y+basePoints[i].Y+hl;
                    }
                }
                else
#endif // 0
                {
                    float hc = TheEarth::getInstance()->getNewHeight((float)ppos.X, (float)ppos.Z/*-offsetManager->getOffset()*/);
                    //printf("%f x %f - %f\n", ppos.X-offsetManager->getOffset().X, ppos.Z-offsetManager->getOffset().Z, hc);
                    if (hc > 0.01f)
                        vtx.Pos.Y = roadType->slicePoints[j].Y+hc;
                }
            }
            vtx.TCoords = irr::core::vector2df(tx, ty);
            
            if (j < roadType->slicePoints.size() - 1) tx += (roadType->slicePoints[j+1] - roadType->slicePoints[j]).getLength()/roadType->tRate;

            buffer->Vertices.push_back(vtx);
        }
        if (i < roadRoadChunk.roadChunk.second)
        {
            ty += (float)(basePoints[i+1].p - basePoints[i].p).getLength()/roadType->tRate;
            for (unsigned int k = 0; k < roadType->sliceIndices.size(); k++)
            {
                buffer->Indices.push_back(vertexCount+roadType->sliceIndices[k]);
            }
            vertexCount += roadType->slicePoints.size();
        }
    }

    for (int ind=0; ind<(int)buffer->Indices.size(); ind+=3)
    {
        irr::core::plane3d<float> p(
            buffer->Vertices[buffer->Indices[ind+0]].Pos,
            buffer->Vertices[buffer->Indices[ind+1]].Pos,
            buffer->Vertices[buffer->Indices[ind+2]].Pos);
        p.Normal.normalize();
        //printf("%f %f %f\n", p.Normal.X, p.Normal.Y, p.Normal.Z);
        buffer->Vertices[buffer->Indices[ind+0]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+1]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+2]].Normal = p.Normal;
    }

    buffer->recalculateBoundingBox();

    animatedMesh = new irr::scene::SAnimatedMesh();
    mesh->addMeshBuffer(buffer);
    mesh->recalculateBoundingBox();
    animatedMesh->addMesh(mesh);
    animatedMesh->recalculateBoundingBox();

    mesh->drop();
    buffer->drop();
   
}

void VisualRoad::switchToVisible()
{
    assert(roadNode == 0);

    OffsetManager* offsetManager = OffsetManager::getInstance();
    RoadType* roadType = roadRoadChunk.road->getRoadType();
    const Road::roadPointVector_t& basePoints = roadRoadChunk.road->getRoadPointVector();
    irr::core::vector3df basePoint((float)basePoints[roadRoadChunk.roadChunk.first].p.X, 0.f, (float)basePoints[roadRoadChunk.roadChunk.first].p.Z);

    roadNode = TheGame::getInstance()->getSmgr()->addAnimatedMeshSceneNode(animatedMesh);
    if (Shaders::getInstance()->getSupportedSMVersion() < 2)
    {
        roadNode->setMaterialFlag(irr::video::EMF_LIGHTING, Settings::getInstance()->nonshaderLight);
    }
    else
    {
        roadNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }
    //roadNode->getMaterial(0).UseMipMaps = false;
    roadNode->setMaterialTexture(0, roadType->texture);
    roadNode->setMaterialTexture(1, ShadowRenderer::getInstance()->getShadowMap());
    if (roadType->frictionMulti > 0.01f)
    {
        roadNode->setMaterialType(Shaders::getInstance()->materialMap["normal_shadow"]);
    }
    else
    {
        roadNode->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);
        roadNode->setMaterialType(Shaders::getInstance()->materialMap["normal_shadow_t"]);
    }
//////////////////
    // turned off the physics because it is extremelly slow
#if 0
    if (roadType->frictionMulti > 0.01f)
    {

        hkShape = ObjectPool::calculateNonConvexCollisionMeshMeshes(animatedMesh);
        if (hkShape) 
        {
            hk::lock();
            hkpRigidBodyCinfo rbCi;
            rbCi.m_shape = hkShape;
            rbCi.m_motionType = hkpMotion::MOTION_FIXED;
            rbCi.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(hk::materialType::roadId);
            hkBody = new hkpRigidBody(rbCi);
            
            hkpPropertyValue val(1);
            hkBody->addProperty(hk::materialType::roadId, val);
            
            hk::hkWorld->addEntity(hkBody);
            hk::unlock();
        }
    }
#endif // 0 or 1
////////////////////////
    roadNode->setPosition(basePoint);
    assert(offsetObject == 0);
    offsetObject = new OffsetObject(roadNode, hkBody);
    offsetObject->addToManager();
}
