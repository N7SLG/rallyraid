#include "ShadowRenderer.h"
#include "Settings.h"
#include "Shaders.h"
#include "TheGame.h"
#include "ObjectPoolManager.h"
#include "stdafx.h"

ShadowRenderer* ShadowRenderer::shadowRenderer = 0;

void ShadowRenderer::initialize()
{
    if (!shadowRenderer)
    {
        shadowRenderer = new ShadowRenderer();
    }
}

void ShadowRenderer::finalize()
{
    if (shadowRenderer)
    {
        delete shadowRenderer;
        shadowRenderer = 0;
    }
}
    

ShadowRenderer::ShadowRenderer()
    : shadowMap(0),
      shadowMaterial((irr::video::E_MATERIAL_TYPE)0),
      vehicleMaterial((irr::video::E_MATERIAL_TYPE)0),
      projectionMatrix(),
      viewMatrix(),
      maxShadow(0.f)
{
    if (Settings::getInstance()->shadowMapSize > 0)
    {
        shadowMap = TheGame::getInstance()->getDriver()->addRenderTargetTexture(irr::core::dimension2du(Settings::getInstance()->shadowMapSize, Settings::getInstance()->shadowMapSize), "RTT1");
        const float scale = 100.f;//Settings::getInstance()->shadowMapSize * 0.5f;
        projectionMatrix[0] = 1.0f/scale;
        projectionMatrix[5] = 0.0f;
        projectionMatrix[9] = 1.0f/scale;
        projectionMatrix[10] = 0.0f;
        projectionMatrix[14] = 1.0f/3.0f;
    }
    viewMatrix[12] = 0.0f;
    shadowMaterial = Shaders::getInstance()->materialMap["shadow"];
    vehicleMaterial = Shaders::getInstance()->materialMap["vehicle"];
}

ShadowRenderer::~ShadowRenderer()
{
    shadowMap = 0;
}

void ShadowRenderer::renderShadowNodes()
{
    //printf("rsn: %p, ss: %u, objs: %lu\n", shadowMap, Settings::getInstance()->shadowMapSize, ObjectPoolManager::getInstance()->getShadowNodeSet().size());
    if (shadowMap)
    {
        irr::video::IVideoDriver* driver = TheGame::getInstance()->getDriver();
        irr::scene::ICameraSceneNode* camera = TheGame::getInstance()->getCamera();
        
        viewMatrix.setTranslation(irr::core::vector3df(-camera->getPosition().X, 0.0f, -camera->getPosition().Z));
        
        driver->setTransform(irr::video::ETS_VIEW, viewMatrix);
        driver->setTransform(irr::video::ETS_PROJECTION, projectionMatrix); 
        driver->setRenderTarget(shadowMap, true, true, irr::video::SColor(0, 0, 0, 0));

        foreach(it, ObjectPoolManager::getInstance()->getShadowNodeSet(), ObjectPoolManager::sceneNodeSet_t)
        {
            if ((*it)->isVisible() && camera->getPosition().getDistanceFrom((*it)->getPosition())<160.f)
            {
                //printf("render shadow object\n");
                const unsigned int CurrentMaterialCount = (*it)->getMaterialCount();
                irr::core::array<int> BufferMaterialList(CurrentMaterialCount);
                bool cardraw = false;
                BufferMaterialList.set_used(0);
                maxShadow = 0.f;
                        
                for(unsigned int m = 0; m < CurrentMaterialCount; ++m)
                {
                    BufferMaterialList.push_back((*it)->getMaterial(m).MaterialType);
                    if ((*it)->getMaterial(m).MaterialType == vehicleMaterial)
                    {
                        maxShadow = 1.0f;
                        cardraw = true;
                    }
                    (*it)->getMaterial(m).MaterialType = shadowMaterial;
                }
                //printf("6k\n");
                (*it)->OnAnimate(TheGame::getInstance()->getTick());
                //printf("6l\n");
                (*it)->OnRegisterSceneNode();
                //printf("6m cardraw: %u, shadowNodes[i]: %p, i: %u/%u\n", cardraw, shadowNodes[i], i, shadowNodes.size());
                (*it)->render();
                //printf("6n\n");
                //shadowObjs++;
                const unsigned int BufferMaterialListSize = BufferMaterialList.size();
                for(unsigned int m = 0; m < BufferMaterialListSize; ++m)
                {
                    (*it)->getMaterial(m).MaterialType = (irr::video::E_MATERIAL_TYPE)BufferMaterialList[m];
                }
            }
        }
        //viewMatrix.setTranslation(irr::core::vector3df(camera->getPosition().X, 0.0f, camera->getPosition().Z));
    }
}
