
#include "ObjectPool.h"
#include "OffsetObject.h"
#include "ObjectPoolManager.h"
#include "TheGame.h"
#include "Settings.h"
#include "Shaders.h"
#include "stdafx.h"
#include "CTreeSceneNode.h"
#include "CTreeGenerator.h"
#include "ShadowRenderer.h"


ObjectPool::ObjectPool(const std::string& name, 
                       const std::string& meshFilename, const std::string& textureFilename,
                       const std::string& texture2Filename,
                       bool physics, ObjectType objectType,
                       const std::string& materialName, const std::string& material2Name,
                       unsigned int num, unsigned int category, float friction, float mass,
                       const irr::core::vector3df& center,
                       bool _near)
    : name(name),
      objectList(),
      objectMesh(0),
      treeGenerator(0),
      hkShape(0),
      category(category),
      texture(0),
      texture2(0),
      objectType(objectType),
      material(irr::video::EMT_SOLID),
      material2(irr::video::EMT_SOLID),
      friction(friction),
      mass(mass),
      center(center),
      num(num),
      inUse(0),
      receiveShadow(false),
      _near(_near)
{
    if (textureFilename != "")
    {
        texture = TheGame::getInstance()->getDriver()->getTexture(textureFilename.c_str());
    }
    if (texture2Filename != "")
    {
        texture2 = TheGame::getInstance()->getDriver()->getTexture(texture2Filename.c_str());
    }
    if (materialName != "")
    {
        material = Shaders::getInstance()->materialMap[materialName];
    }
    if (material2Name != "")
    {
        material2 = Shaders::getInstance()->materialMap[material2Name];
    }
    if (materialName == "vehicle" || materialName == "normal_shadow" || materialName == "normal_shadow_t")
    {
        receiveShadow = true;
    }

    switch (objectType)
    {
        case Grass:
            objectMesh = generateGrassMesh();
            break;
        case Tree:
        {
            treeGenerator = new irr::scene::CTreeGenerator(TheGame::getInstance()->getSmgr());
            irr::io::IXMLReader* xml = TheGame::getInstance()->getDevice()->getFileSystem()->createXMLReader(meshFilename.c_str());
            treeGenerator->loadFromXML(xml);
            xml->drop();
            break;
        }
        default:
            if (meshFilename.rfind(".mso") != std::string::npos)
            {
                objectMesh = readMySimpleObject(meshFilename, (objectType!=Vehicle)?mass:1.0f);
            }
            else
            {
                objectMesh = TheGame::getInstance()->getSmgr()->getMesh(meshFilename.c_str());
            }
            break;
    }

    if (physics)
    {
        hkShape = calculateCollisionMesh(objectMesh, objectType);
    }


    if (Settings::getInstance()->preloadObjects)
    {
        for (unsigned int i = 0; i < num; i++)
        {
            OffsetObject* offsetObject = createNewInstance();
            objectList.push_back(offsetObject);
        }
    }
}

ObjectPool::~ObjectPool()
{
    for (objectList_t::iterator it = objectList.begin();
         it != objectList.end();
         it++)
    {
        //printf("drop node: %p\n", (*it)->getNode());
        //(*it)->getNode()->drop();
        (*it)->getNode()->remove();
        delete *it;
    }
    objectList.clear();
    if (hkShape)
    {
        hk::lock();
        hkShape->removeReference();
        hk::unlock();
        hkShape = 0;
    }
    if (objectMesh)
    {
        //objectMesh->drop(); // commented out because
        objectMesh = 0;
    }
}

OffsetObject* ObjectPool::getObject(const irr::core::vector3df& apos, const irr::core::vector3df& scale, const irr::core::vector3df& rot, bool addToOffsetManager)
{
    //dprintf(MY_DEBUG_NOTE, "ObjectPool::getObject(): %s\n", name.c_str());
    OffsetObject* offsetObject = 0;
    if (!objectList.empty())
    {
        offsetObject = objectList.front(); // *objectList.begin();
        objectList.erase(objectList.begin());
    }
    else
    {
        offsetObject = createNewInstance();
    }

    offsetObject->setUpdateCB(0);
    //offsetObject->setPos(apos);
    offsetObject->getNode()->setPosition(apos);
    offsetObject->getNode()->setScale(scale);
    offsetObject->getNode()->setRotation(rot);
    offsetObject->getNode()->setMaterialType(material);
    if (Shaders::getInstance()->getSupportedSMVersion() < 2)
    {
        offsetObject->getNode()->setMaterialFlag(irr::video::EMF_LIGHTING, Settings::getInstance()->nonshaderLight);
    }
    else
    {
        offsetObject->getNode()->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }
    //printf("-------------- texture: %p\n", texture);
    offsetObject->getNode()->setMaterialTexture(0, texture);

    if (hkShape)
    {
    
        hk::lock();
        hkpRigidBodyCinfo groundInfo;
        groundInfo.m_shape = hkShape;
        groundInfo.m_position.set(apos.X, apos.Y, apos.Z);
        if (rot != irr::core::vector3df())
        {
            irr::core::vector3df rotRad = rot * irr::core::DEGTORAD;
            irr::core::quaternion rotQuat(rotRad);
            groundInfo.m_rotation = hkQuaternion(rotQuat.X, rotQuat.Y, rotQuat.Z, rotQuat.W);
        }
        if (objectType == Vehicle)
        {
            groundInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
            groundInfo.m_mass = mass;
            // TODO
            //groundInfo.m_position.set(0.0f, 0.0f, 0.0f);
            groundInfo.m_inertiaTensor.setDiagonal(1.0f, 1.0f, 1.0f);
            groundInfo.m_centerOfMass.set(center.X, center.Y, center.Z);
            groundInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(hk::materialType::vehicleId);
        }
        else
        {
            groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
            groundInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(hk::materialType::treeId);
        }
        groundInfo.m_friction = friction;
        hkpRigidBody* hkBody = new hkpRigidBody(groundInfo);
        if (objectType != Vehicle)
        {
            hkpPropertyValue val(1);
            hkBody->addProperty(hk::materialType::treeId, val);
            hk::hkWorld->addEntity(hkBody);
        }
        else
        {
            hkpPropertyValue val(1);
            hkBody->addProperty(hk::materialType::vehicleId, val);
        }
        hk::unlock();
        offsetObject->setBody(hkBody);
        //hkBody->activate();
    }

    offsetObject->setPool(this);
    inUse++;

    if (addToOffsetManager)
    {
        offsetObject->addToManager();
    }
    offsetObject->getNode()->setVisible(true);
    switch (objectType)
    {
    case Grass:
    case MyTree:
        break;
    default:
        ObjectPoolManager::getInstance()->addShadowNode(offsetObject->getNode());
        break;
    }
    return offsetObject;
}

void ObjectPool::putObject(OffsetObject* object)
{
    //dprintf(MY_DEBUG_NOTE, "ObjectPool::putObject(): %s\n", name.c_str());
    switch (objectType)
    {
    case Grass:
    case MyTree:
        break;
    default:
        ObjectPoolManager::getInstance()->removeShadowNode(object->getNode());
        break;
    }
    object->getNode()->setVisible(false);
    hkpRigidBody* hkBody = object->getBody();
    if (hkBody)
    {
        hk::lock();
        hkBody->removeReference();
        if (objectType != Vehicle)
        {
            hk::hkWorld->removeEntity(hkBody);
        }
        hk::unlock();
        hkBody = 0;
        object->setBody(0);
        object->setPool(0);
    }
    object->removeFromManager();
    object->setUpdateCB(0);
    if (Settings::getInstance()->cacheObjects)
    {
        objectList.push_back(object);
    }
    else
    {
        object->getNode()->remove();
        object->setNode(0);
        delete object;
    }
    inUse--;
}

OffsetObject* ObjectPool::createNewInstance()
{
    //irr::scene::IAnimatedMeshSceneNode* objectNode = 0;
    irr::scene::ISceneNode* objectNode = 0;
    switch (objectType)
    {
        case Tree:
        {
            irr::scene::CTreeSceneNode* treeNode = new irr::scene::CTreeSceneNode(TheGame::getInstance()->getSmgr()->getRootSceneNode(),
                TheGame::getInstance()->getSmgr());
            treeNode->setup(treeGenerator, rand()%0xffff, 0);
            
            if (treeNode->getLeafNode())
            {
                treeNode->getLeafNode()->getMaterial(0).TextureLayer[0].AnisotropicFilter = true;
                treeNode->getLeafNode()->getMaterial(0).TextureLayer[0].BilinearFilter = false;
                //treeNode->getLeafNode()->getMaterial(0).MaterialTypeParam = 0.5f;
                treeNode->getLeafNode()->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);
                
                treeNode->getLeafNode()->setMaterialTexture(0, texture2);
                treeNode->getLeafNode()->setMaterialType(material2);
            }

            treeNode->setMaterialTexture(0, texture);
            treeNode->setMaterialType(material);

            //if (globalLight)
            //{
            //    treeNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
            //}
            objectNode = treeNode;
            break;
        }
        case MyTree:
        {
            objectNode = TheGame::getInstance()->getSmgr()->addAnimatedMeshSceneNode(objectMesh);
            
            irr::scene::IBillboardSceneNode* leaf = TheGame::getInstance()->getSmgr()->addBillboardSceneNode(objectNode, irr::core::dimension2df(2.5f*mass, 2.5f*mass), irr::core::vector3df(0.f, 3.f*mass, 0.f));
            objectNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            leaf->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            leaf->setMaterialFlag(irr::video::EMF_TEXTURE_WRAP, true);
            leaf->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);

            objectNode->setMaterialTexture(0, texture);
            objectNode->setMaterialType(material);

            leaf->setMaterialTexture(0, texture2);
            leaf->setMaterialType(material2);
            leaf->getMaterial(0).MaterialTypeParam = 0.1f;
            break;
        }
        default:
            objectNode = TheGame::getInstance()->getSmgr()->addAnimatedMeshSceneNode(objectMesh);
            objectNode->setMaterialType(material);
            if (objectType == Grass)
            {
                objectNode->setMaterialFlag(irr::video::EMF_TEXTURE_WRAP, true);
                objectNode->getMaterial(0).UseMipMaps = false;
            }
            objectNode->setMaterialFlag(irr::video::EMF_BLEND_OPERATION, true);
            objectNode->setMaterialTexture(0, texture);
            if (receiveShadow)
            {
                if (texture2 == 0)
                {
                    objectNode->setMaterialTexture(1, ShadowRenderer::getInstance()->getShadowMap());
                }
                else
                {
                    objectNode->setMaterialTexture(1, texture2);
                    objectNode->setMaterialTexture(2, ShadowRenderer::getInstance()->getShadowMap());
                }
            }
            else
            {
                objectNode->setMaterialTexture(1, texture2);
            }
            break;
    }
    
    assert(objectNode);
    
    objectNode->setVisible(false);
    // TODO
    OffsetObject* offsetObject = new OffsetObject(objectNode, objectType == Vehicle);

    return offsetObject;
}

irr::scene::SAnimatedMesh* ObjectPool::readMySimpleObject(const std::string& meshFilename, float scale)
{
    FILE* f;
    unsigned int numOfVertices, numOfPols;
    float x,y,z,tu,tv;
    irr::u32 r,g,b;
    int ret, index;
    irr::u32 verInd;
    irr::video::S3DVertex vtx;
    vtx.Color.set(255,255,255,255);
    vtx.Normal.set(0,1,0);

#ifdef MSO_DEBUG
    printf("Read my simple object: %s\n", name);
#endif
    
    errno_t error = fopen_s(&f, meshFilename.c_str(), "r");
    
    if (error)
    {
        printf("my simple object file unable to open: %s\n", meshFilename.c_str());
        return 0;
    }
    
    irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();
    irr::scene::SMesh* mesh = new irr::scene::SMesh();
    
#ifdef MSO_DEBUG
    printf("read vertices\n");
#endif    
    ret = fscanf_s(f, "vertices\n%u\n", &numOfVertices);
    if (ret <= 0)
    {
       printf("error reading %s ret %d errno %d\n", meshFilename.c_str(), ret, errno);
       fclose(f);
       return 0;
    }

#ifdef MSO_DEBUG
    printf("vertices: %u\n", numOfVertices);
#endif    

    for (unsigned int ind = 0; ind < numOfVertices; ind++)
    {
#ifdef MSO_DEBUG
    printf("read a vertex\n");
#endif    
        ret = fscanf_s(f, "%d %f %f %f %f %f %u %u %u\n", &index, &x, &y, &z, &tu, &tv, &r, &g, &b);
        if (ret <= 0)
        {
           printf("error reading %s ret %d errno %d\n", meshFilename.c_str(), ret, errno);
           fclose(f);
           return 0;
        }
#ifdef MSO_DEBUG
    printf("vertex read done\n");
#endif    
        vtx.Pos.X = x*scale;
        vtx.Pos.Z = z*scale;
        vtx.Pos.Y = y*scale;
        vtx.TCoords.X = tu;
        vtx.TCoords.Y = tv;
        vtx.Color.set(255,r,g,b);
        buffer->Vertices.push_back(vtx);
    }
#ifdef MSO_DEBUG
    printf("read polygons number\n");
#endif    
    ret = fscanf_s(f, "polygons\n%u\n", &numOfPols);
    if (ret <= 0)
    {
       printf("error reading %s ret %d errno %d\n", meshFilename.c_str(), ret, errno);
       fclose(f);
       return 0;
    }
#ifdef MSO_DEBUG
    printf("polygons: %u\n", numOfPols);
#endif    
    for (unsigned int ind = 0; ind < numOfPols*3; ind++)
    {
#ifdef MSO_DEBUG
    printf("read a poly part\n");
#endif    
        ret = fscanf_s(f, "%u\n", &verInd);
        if (ret <= 0)
        {
           printf("error reading %s ret %d errno %d\n", meshFilename.c_str(), ret, errno);
           fclose(f);
           return 0;
        }
#ifdef MSO_DEBUG
    printf("read a poly part done\n");
#endif    
        if (verInd >= numOfVertices)
        {
           printf("!!!!! verInd >= numOfVertices: %d > %u\n", verInd, numOfVertices);
        }
        buffer->Indices.push_back(verInd);
    }

#ifdef MSO_DEBUG
    printf("renormalize\n");
#endif    
    for (irr::s32 ind=0; ind<(irr::s32)buffer->Indices.size(); ind+=3)
    {
        irr::core::plane3d<irr::f32> p(
            buffer->Vertices[buffer->Indices[ind+0]].Pos,
            buffer->Vertices[buffer->Indices[ind+1]].Pos,
            buffer->Vertices[buffer->Indices[ind+2]].Pos);
        p.Normal.normalize();

        buffer->Vertices[buffer->Indices[ind+0]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+1]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+2]].Normal = p.Normal;
    }
#ifdef MSO_DEBUG
    printf("renormalize done\n");
#endif    
   
    buffer->recalculateBoundingBox();

    irr::scene::SAnimatedMesh* animatedMesh = new irr::scene::SAnimatedMesh();
    mesh->addMeshBuffer(buffer);
    mesh->recalculateBoundingBox();
    animatedMesh->addMesh(mesh);
    animatedMesh->recalculateBoundingBox();

    mesh->drop();
    buffer->drop();

    fclose(f);
#ifdef MSO_DEBUG
    printf("read done return %p\n", animatedMesh);
#endif    

    return animatedMesh;
}

hkpShape* ObjectPool::calculateCollisionMesh(irr::scene::IAnimatedMesh* objectMesh, ObjectType objectType, bool /*box*/)
{
    hkpShape* hkShape = 0;
    
    if (objectType == Tree || objectType == MyTree)
    {
        hk::lock();
        hkShape = new hkpBoxShape(hkVector4(1.0f, 20.0f, 1.0f), 0);
        hk::unlock();
        return hkShape;
    }
    
    if (objectMesh == 0) return hkShape;
    
    int sizeOfBuffers = 0;
    for (unsigned int i = 0; i < objectMesh->getMeshBufferCount(); i++)
    {
        if (objectMesh->getMeshBuffer(i)->getVertexType() != irr::video::EVT_STANDARD)
        {
            printf("object %u type mismatch %u\n", i, objectMesh->getMeshBuffer(i)->getVertexType());
            assert(0);
            return hkShape;
        }
            
        sizeOfBuffers += objectMesh->getMeshBuffer(i)->getVertexCount();
    }
        
    float* my_vertices = new float[sizeOfBuffers*3];
    int cursor = 0;
        
    for (unsigned int i = 0; i < objectMesh->getMeshBufferCount();i++)
    {
        irr::scene::IMeshBuffer* mb = objectMesh->getMeshBuffer(i);
        irr::video::S3DVertex* mb_vertices = (irr::video::S3DVertex*)mb->getVertices();
        for (unsigned int j = 0, tmpCounter = cursor*3; j < mb->getVertexCount(); j++, tmpCounter+=3)
        {
            my_vertices[tmpCounter] = mb_vertices[j].Pos.X /* scale.X */;
            my_vertices[tmpCounter+1] = mb_vertices[j].Pos.Y /* scale.Y */;
            my_vertices[tmpCounter+2] = mb_vertices[j].Pos.Z /* scale.Z */;
            //my_vertices[(cursor+j)*4+3] = 0.0f;
        }
        cursor += mb->getVertexCount();
    }
    
    hk::lock();
    hkStridedVertices stridedVerts;
    stridedVerts.m_numVertices = sizeOfBuffers;
    stridedVerts.m_striding = 3*sizeof(float);
    stridedVerts.m_vertices = my_vertices;

    hkShape = new hkpConvexVerticesShape(stridedVerts);
    hk::unlock();

    delete [] my_vertices;

    return hkShape;
}

/* static */ hkpExtendedMeshShape* ObjectPool::calculateNonConvexCollisionMeshMeshes(irr::scene::IAnimatedMesh* objectMesh)
{
    hk::lock();
    hkpExtendedMeshShape* hkShape = new hkpExtendedMeshShape;
    hk::unlock();

    for (irr::u32 i = 0; i < objectMesh->getMeshBufferCount(); i++) 
    {
        irr::scene::IMeshBuffer* mb = objectMesh->getMeshBuffer(i); 
        if (mb->getVertexType() != irr::video::EVT_STANDARD)
        {
            printf("object %u type mismatch %u\n", i, mb->getVertexType());
            assert(0);
            return 0;
        }

        float* vertexBuffer = new float[mb->getVertexCount() * 3];
        unsigned short* indexBuffer = mb->getIndices();

        for (unsigned int x = 0, tmpCounter = 0; x < mb->getVertexCount(); x++, tmpCounter+=3) 
        { 
            const irr::core::vector3df& tmp = mb->getPosition(x); 
            vertexBuffer[tmpCounter] = tmp.X; 
            vertexBuffer[tmpCounter+1] = tmp.Y; 
            vertexBuffer[tmpCounter+2] = tmp.Z; 
        } 

        hkpExtendedMeshShape::TrianglesSubpart part; 

        part.m_vertexBase = vertexBuffer; 
        part.m_vertexStriding = sizeof(float) * 3; 
        part.m_numVertices = mb->getVertexCount(); 

        part.m_indexBase = indexBuffer; 
        part.m_indexStriding = sizeof(unsigned short)*3; 
        part.m_numTriangleShapes = mb->getIndexCount()/3; 
        part.m_stridingType = hkpExtendedMeshShape::INDICES_INT16; 

        hk::lock();
        hkShape->addTrianglesSubpart(part); 
        hk::unlock();
        
        //delete [] vertexBuffer;
    }
    
    return hkShape;
}

#define GENERATED_GRASS_SIZE 10
irr::scene::SAnimatedMesh* ObjectPool::generateGrassMesh()
{
    irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();
    irr::scene::SMesh* mesh = new irr::scene::SMesh();
    const float width = 3.f;//((float)(rand() % 10) + 1.f) / 5.f;
    const float height = 2.f;//((float)(rand() % 20) + 2.f) / 6.f;
    irr::video::S3DVertex vtx;
    vtx.Color.set(255,255,255,255);
    vtx.Normal.set(0,1,0);
            
    for (int j = 0; j < 7; j++)
    {
        float offx = ((float)(rand() % GENERATED_GRASS_SIZE - (GENERATED_GRASS_SIZE / 2)));
        float offz = ((float)(rand() % GENERATED_GRASS_SIZE - (GENERATED_GRASS_SIZE / 2)));
                        
        vtx.Pos.X = offx-width/2.f;
        vtx.Pos.Z = offz+0.f;
        vtx.Pos.Y = 0.f;
        vtx.TCoords.X = 0.f;
        vtx.TCoords.Y = 1.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx+width/2.f;
        vtx.Pos.Z = offz+0.f;
        vtx.Pos.Y = 0.f;
        vtx.TCoords.X = 1.f/*width/3.f*/;
        vtx.TCoords.Y = 1.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx+width/2.f;
        vtx.Pos.Z = offz+0.f;
        vtx.Pos.Y = height;
        vtx.TCoords.X = 1.f/*width/3.f*/;
        vtx.TCoords.Y = 0.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx-width/2.f;
        vtx.Pos.Z = offz+0.f;
        vtx.Pos.Y = height;
        vtx.TCoords.X = 0.f;
        vtx.TCoords.Y = 0.f;
        buffer->Vertices.push_back(vtx);
                
    // other dir
        vtx.Pos.X = offx+0.f;
        vtx.Pos.Z = offz-width/2.f;
        vtx.Pos.Y = 0.f;
        vtx.TCoords.X = 0.f;
        vtx.TCoords.Y = 1.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx+0.f;
        vtx.Pos.Z = offz+width/2.f;
        vtx.Pos.Y = 0.f;
        vtx.TCoords.X = 1.f/*width/3.f*/;
        vtx.TCoords.Y = 1.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx+0.f;
        vtx.Pos.Z = offz+width/2.f;
        vtx.Pos.Y = height;
        vtx.TCoords.X = 1.f/*width/3.f*/;
        vtx.TCoords.Y = 0.f;
        buffer->Vertices.push_back(vtx);
                
        vtx.Pos.X = offx+0.f;
        vtx.Pos.Z = offz-width/2.f;
        vtx.Pos.Y = height;
        vtx.TCoords.X = 0.f;
        vtx.TCoords.Y = 0.f;
        buffer->Vertices.push_back(vtx);
                
        buffer->Indices.push_back((j*8)+0);
        buffer->Indices.push_back((j*8)+1);
        buffer->Indices.push_back((j*8)+2);
                
        buffer->Indices.push_back((j*8)+0);
        buffer->Indices.push_back((j*8)+2);
        buffer->Indices.push_back((j*8)+3);
                
        buffer->Indices.push_back((j*8)+0);
        buffer->Indices.push_back((j*8)+2);
        buffer->Indices.push_back((j*8)+1);
                
        buffer->Indices.push_back((j*8)+0);
        buffer->Indices.push_back((j*8)+3);
        buffer->Indices.push_back((j*8)+2);
    // other dir
        buffer->Indices.push_back((j*8)+4);
        buffer->Indices.push_back((j*8)+5);
        buffer->Indices.push_back((j*8)+6);
                
        buffer->Indices.push_back((j*8)+4);
        buffer->Indices.push_back((j*8)+6);
        buffer->Indices.push_back((j*8)+7);
                
        buffer->Indices.push_back((j*8)+4);
        buffer->Indices.push_back((j*8)+6);
        buffer->Indices.push_back((j*8)+5);
                
        buffer->Indices.push_back((j*8)+4);
        buffer->Indices.push_back((j*8)+7);
        buffer->Indices.push_back((j*8)+6);
    }
    //printf("debug %d norm start\n", i);
                    
    for (irr::s32 ind=0; ind<(irr::s32)buffer->Indices.size(); ind+=3)
    {
        irr::core::plane3d<irr::f32> p(
            buffer->Vertices[buffer->Indices[ind+0]].Pos,
            buffer->Vertices[buffer->Indices[ind+1]].Pos,
            buffer->Vertices[buffer->Indices[ind+2]].Pos);
        p.Normal.normalize();
        
        buffer->Vertices[buffer->Indices[ind+0]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+1]].Normal = p.Normal;
        buffer->Vertices[buffer->Indices[ind+2]].Normal = p.Normal;
    }
            
    //printf("debug %d norm end\n", i);
                   
    buffer->recalculateBoundingBox();
            
    irr::scene::SAnimatedMesh* animatedMesh = new irr::scene::SAnimatedMesh();
    mesh->addMeshBuffer(buffer);
    mesh->recalculateBoundingBox();
    animatedMesh->addMesh(mesh);
    animatedMesh->recalculateBoundingBox();
            
    mesh->drop();
    buffer->drop();
                    
    return animatedMesh;
}
