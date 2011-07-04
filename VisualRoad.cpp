
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
#include "stdafx.h"
#include "Shaders.h"

#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/Mesh/hkpMeshMaterial.h>
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
    if (roadType->frictionMulti > 0.01f)
    {
        roadNode->setMaterialType(Shaders::getInstance()->materialMap["normal"]);
    }
    else
    {
        roadNode->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);
        roadNode->setMaterialType(Shaders::getInstance()->materialMap["normal_no_light_t"]);
    }
//////////////////
#if 0
    if (roadType->frictionMulti > 0.01f)
    {
        hkVector4* hkVertexArray = 0;
        dprintf(MY_DEBUG_NOTE, "road build begin %d\n", animatedMesh->getMeshBufferCount());
        {
            int j;
            int v1i, v2i, v3i;
            //IMeshBuffer *mb;
            float vArray[9]; // vertex array (3*3 floats)
            float maxes[3] = {0.f,0.f,0.f};
            float mines[3] = {100000.f,100000.f,100000.f};
            int pols = 0;
            int step = 1;
            
            irr::scene::IMeshBuffer* mb = animatedMesh->getMeshBuffer(0);
            //assert(mb->getIndexType()==EIT_32BIT);
    
            irr::video::S3DVertex/*2TCoords*/* mb_vertices = (irr::video::S3DVertex2TCoords*)mb->getVertices();

            unsigned short* mb_indices  = mb->getIndices();
            
            //float* my_vertices = new float[3*mb->getIndexCount()];
            hkVertexArray = (hkVector4*)(new char[sizeof(hkVector4)*mb->getVertexCount()]);
            for (j=0; j<mb->getVertexCount(); j++)
            {
                hkVertexArray[j] = hkVector4(mb_vertices[j].Pos.X, mb_vertices[j].Pos.Y, mb_vertices[j].Pos.Z);
            }
            dprintf(MY_DEBUG_NOTE, "index count: %d vertex count %d\n", mb->getIndexCount(), mb->getVertexCount());
            if (mb->getIndexType()==irr::video::EIT_32BIT/*mb->getVertexCount()>= 256*256*/)
            {
                step = 2;
            }
            dprintf(MY_DEBUG_NOTE, "using step = %u index type %u\n", step, mb->getIndexType());
            // add each triangle from the mesh
            for (j=0; j<mb->getIndexCount()*step; j+=3*step)
            {
                //printf("pol: %d\n", j);
                pols++;
                if (step == 1)
                {
                    v1i = mb_indices[j];
                    v2i = mb_indices[j+1];
                    v3i = mb_indices[j+2];
                }
                else
                {
                    v1i = *((int*)&mb_indices[j]);
                    v2i = *((int*)&mb_indices[j+2]);
                    v3i = *((int*)&mb_indices[j+4]);
                }

                vArray[0] = mb_vertices[v1i].Pos.X;
                vArray[1] = mb_vertices[v1i].Pos.Y;
                vArray[2] = mb_vertices[v1i].Pos.Z;
                vArray[3] = mb_vertices[v2i].Pos.X;
                vArray[4] = mb_vertices[v2i].Pos.Y;
                vArray[5] = mb_vertices[v2i].Pos.Z;
                vArray[6] = mb_vertices[v3i].Pos.X;
                vArray[7] = mb_vertices[v3i].Pos.Y;
                vArray[8] = mb_vertices[v3i].Pos.Z;
                if (vArray[0]>maxes[0]) maxes[0] = vArray[0];
                if (vArray[1]>maxes[1]) maxes[1] = vArray[1];
                if (vArray[2]>maxes[2]) maxes[2] = vArray[2];
                if (vArray[3]>maxes[0]) maxes[0] = vArray[3];
                if (vArray[4]>maxes[1]) maxes[1] = vArray[4];
                if (vArray[5]>maxes[2]) maxes[2] = vArray[5];
                if (vArray[6]>maxes[0]) maxes[0] = vArray[6];
                if (vArray[7]>maxes[1]) maxes[1] = vArray[7];
                if (vArray[8]>maxes[2]) maxes[2] = vArray[8];
    
                if (vArray[0]<mines[0]) mines[0] = vArray[0];
                if (vArray[1]<mines[1]) mines[1] = vArray[1];
                if (vArray[2]<mines[2]) mines[2] = vArray[2];
                if (vArray[3]<mines[0]) mines[0] = vArray[3];
                if (vArray[4]<mines[1]) mines[1] = vArray[4];
                if (vArray[5]<mines[2]) mines[2] = vArray[5];
                if (vArray[6]<mines[0]) mines[0] = vArray[6];
                if (vArray[7]<mines[1]) mines[1] = vArray[7];
                if (vArray[8]<mines[2]) mines[2] = vArray[8];
                //dprintf(printf("v1i %d: %f %f %f\n", v1i, vArray[0], vArray[1], vArray[2]);)
                //dprintf(printf("v2i %d: %f %f %f\n", v2i, vArray[3], vArray[4], vArray[5]);)
                //dprintf(printf("v3i %d: %f %f %f\n", v3i, vArray[6], vArray[7], vArray[8]);)
            }
            dprintf(MY_DEBUG_NOTE, "road pols: %d mines: %f %f %f\n", pols, mines[0], mines[1], mines[2]);
            dprintf(MY_DEBUG_NOTE, "road pols: %d maxes: %f %f %f\n", pols, maxes[0], maxes[1], maxes[2]);
            //mb->drop();
        }
        dprintf(MY_DEBUG_NOTE, "collisionendbuild\n");
        dprintf(MY_DEBUG_NOTE, "road build end\n");
    
        /*
        dprintf(MY_DEBUG_NOTE, "1 - %u\n", animatedMesh->getMeshBuffer(0)->getVertexCount());
        hkShape = new hkpExtendedMeshShape();
        hkpExtendedMeshShape::TrianglesSubpart part;
        //animatedMesh->getMeshBuffer(0)->getVertices()
        part.m_vertexBase = reinterpret_cast<hkReal*>(hkVertexArray);
        part.m_vertexStriding = sizeof(hkVector4);
        part.m_numVertices = animatedMesh->getMeshBuffer(0)->getVertexCount();

        part.m_indexBase = animatedMesh->getMeshBuffer(0)->getIndices();
        part.m_stridingType = animatedMesh->getMeshBuffer(0)->getIndexType()==EIT_32BIT ?
            hkpExtendedMeshShape::INDICES_INT32 : hkpExtendedMeshShape::INDICES_INT16;
        part.m_indexStriding = animatedMesh->getMeshBuffer(0)->getIndexType()==EIT_32BIT ?
            3*sizeof(hkInt32) : 3*sizeof(hkInt16);
        part.m_numTriangleShapes = animatedMesh->getMeshBuffer(0)->getIndexCount();

        part.m_materialIndexStridingType = hkpExtendedMeshShape::MATERIAL_INDICES_INT8;
        void* tmpv = (void*)new hkUint8[part.m_numTriangleShapes];
        memset(tmpv, 0, sizeof(hkUint8) * part.m_numTriangleShapes);
        part.m_materialIndexBase = tmpv;
        part.m_materialIndexStriding = sizeof(hkUint8);

        part.m_materialBase = new hkpMeshMaterial();
        part.m_materialStriding = sizeof(hkpMeshMaterial);
        part.m_numMaterials = 1;

        dprintf(MY_DEBUG_NOTE, "2\n");

        ((hkpExtendedMeshShape*)hkShape)->addTrianglesSubpart(part);

        dprintf(MY_DEBUG_NOTE, "3\n");

        hkpRigidBodyCinfo rbCi;
        rbCi.m_shape = hkShape;
        rbCi.m_motionType = hkpMotion::MOTION_FIXED;
        hkBody = new hkpRigidBody(rbCi);
        hkpPropertyValue val(1);
        hkBody->addProperty(roadID, val);
        hkWorld->addEntity(hkBody);

        delete [] (char*)hkVertexArray;
        delete [] tmpv;
        delete part.m_materialBase;
        */
    }
#endif // 0 or 1
////////////////////////
    roadNode->setPosition(basePoint);
    assert(offsetObject == 0);
    offsetObject = new OffsetObject(roadNode, hkBody);
    offsetObject->addToManager();
}
