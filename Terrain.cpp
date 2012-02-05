
#include "Terrain.h"
#include "Terrain_defs.h"
#include "TheGame.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "TheEarth.h"
#include <math.h>
#include "Shaders.h"
#include "Settings.h"
#include "stdafx.h"
#include "ShadowRenderer.h"
#include "RaceManager.h"
#include "Stage.h"


irr::video::ITexture* Terrain::textureDetail1 = 0;
irr::video::ITexture* Terrain::textureDetail2 = 0;

/* static */ void Terrain::initialize()
{
    textureDetail1 = TheGame::getInstance()->getDriver()->getTexture("data/earthdata/detailmap_03.png");
    textureDetail2 = TheGame::getInstance()->getDriver()->getTexture("data/earthdata/detailmap_04.png");
}


Terrain::Terrain(const std::string& prefix)
    : terrain(0),
      hkShape(0),
      offsetObject(0),
      visible(false),
      offsetX(0),
      offsetY(0),
      prefix(prefix),
      image(0),
      texture(0)
      /*, loading(false)*/
{
}

Terrain::~Terrain()
{
    if (visible)
    {
        setVisible(false);
    }
    if (hkShape)
    {
        hk::lock();
        hkShape->removeReference();
        hk::unlock();
        hkShape = 0;
    }
    hkpRigidBody* hkBody = offsetObject->getBody();
    if (hkBody)
    {
        hk::lock();
        hkBody->removeReference();
        hk::hkWorld->removeEntity(hkBody);
        hk::unlock();
        hkBody = 0;
        offsetObject->setBody(0);
    }
    if (image)
    {
        image->drop();
        image = 0;
    }
    delete terrain;
    delete offsetObject;
    TheGame::getInstance()->getDriver()->removeTexture(texture);
    texture = 0;
}

void Terrain::postConstruct()
{
}

void Terrain::postLoad()
{
    //terrain->setPosition(terrain->getPosition()-OffsetManager::getInstance()->getOffset());

    terrain->scaleTexture(1.0f, /*TILE_SIZE_F*/(float)TILE_POINTS_NUM);
    //terrain->OnRegisterSceneNode2();
}

void Terrain::setVisible(bool p_visible)
{
    if (visible == p_visible) return;

    visible = p_visible;

    if (visible)
    {
        // must be here DO NOT update in thread
        if (hkShape && offsetObject->getBody() == 0)
        {
#if 1
            //dprintf(MY_DEBUG_NOTE, "create terrain object\n");
            hk::lock();
            hkpRigidBodyCinfo groundInfo;
            groundInfo.m_shape = hkShape;
            groundInfo.m_position.set(terrain->getPosition().X, terrain->getPosition().Y, terrain->getPosition().Z);
            groundInfo.m_motionType = hkpMotion::MOTION_FIXED;
            groundInfo.m_friction = Settings::getInstance()->groundFriction; // 0.8f;
                /* if (RaceManager::getInstance()->getCurrentStage())
                    groundInfo.m_friction = RaceManager::getInstance()->getCurrentStage()->getGroundFriction();
                   else
                    groundInfo.m_friction = 0.8f;
                */
            groundInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(hk::materialType::terrainId);
            hkpRigidBody* hkBody = new hkpRigidBody(groundInfo);
            hkpPropertyValue val(1);
            hkBody->addProperty(hk::materialType::terrainId, val);
            hk::hkWorld->addEntity(hkBody);
            hk::unlock();
            offsetObject->setBody(hkBody);
#endif // 0 v. 1
        }
        //printf("add to manager %s ... \n", prefix.c_str());
        offsetObject->addToManager(/*skipUpdate*/);
        //printf("add to manager %s ... done\n", prefix.c_str());
        if (image)
        {
            char textureMapPartName[255];
            sprintf_s(textureMapPartName, "%s_textureMapPart_%d_%d", prefix.c_str(), offsetX, offsetY);
            TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
            texture = TheGame::getInstance()->getDriver()->addTexture(textureMapPartName, image);
            TheGame::getInstance()->getDriver()->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);
            //printf("texture cnt: %u\n", TheGame::getInstance()->getDriver()->getTextureCount());
            //printf("image found(%p): %s - %u x %u - %u, %u, %u\n",
            //    texture,
            //    prefix.c_str(), image->getDimension().Width, image->getDimension().Height,
            //    image->getPixel(image->getDimension().Width/2,image->getDimension().Height/2).getRed(),
            //    image->getPixel(image->getDimension().Width/2,image->getDimension().Height/2).getGreen(),
            //    image->getPixel(image->getDimension().Width/2,image->getDimension().Height/2).getBlue());
            terrain->setMaterialTexture(0, texture);
            terrain->setMaterialTexture(1, textureDetail1);
            terrain->setMaterialTexture(2, ShadowRenderer::getInstance()->getShadowMap());
            terrain->setMaterialTexture(3, textureDetail2);
            if (Shaders::getInstance()->getSupportedSMVersion() < 2)
            {
                terrain->setMaterialFlag(irr::video::EMF_LIGHTING, Settings::getInstance()->nonshaderLight);
            }
            else
            {
                terrain->setMaterialFlag(irr::video::EMF_LIGHTING, false);
            }
            //terrain->setMaterialFlag(irr::video::EMF_TEXTURE_WRAP, true);
            //terrain->setMaterialFlag(irr::video::EMF_WIREFRAME, true);
            terrain->setMaterialType(Shaders::getInstance()->materialMap["terrain"]);
            //terrain->scaleTexture(1.0f, /*TILE_SIZE_F*/(float)TILE_POINTS_NUM);
            //image->drop();
        }
        else
        {
            dprintf(MY_DEBUG_NOTE, "Terrain::setVisible(): no image\n");
        }
        //printf("add to manager %s ... done 2\n", prefix.c_str());
    }
    else
    {
        hkpRigidBody* hkBody = offsetObject->getBody();
        if (hkBody)
        {
            hk::lock();
            hkBody->removeReference();
            hk::hkWorld->removeEntity(hkBody);
            hk::unlock();
            hkBody = 0;
            offsetObject->setBody(0);
        }
        offsetObject->removeFromManager();
    }
    terrain->setVisible(visible);
    //dprintf(MY_DEBUG_NOTE, "terrain: nodepos: %f, %f, %f, bodypos: %f, %f, %f\n",
    //    offsetObject->getNode()->getPosition().X, offsetObject->getNode()->getPosition().Y, offsetObject->getNode()->getPosition().Z,
    //    offsetObject->getBody()->getPosition()(0), offsetObject->getBody()->getPosition()(1), offsetObject->getBody()->getPosition()(2));
}

/*void Terrain::handleUpdatePos(bool phys)
{
    while (loading) = false;
}*/
