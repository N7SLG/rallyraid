
#ifndef SHADOWRENDERER_H
#define SHADOWRENDERER_H

#include <irrlicht.h>

class ShadowRenderer
{
public:
    static void initialize();
    static void finalize();
    
    static ShadowRenderer* getInstance() {return shadowRenderer;}

private:
    static ShadowRenderer* shadowRenderer;

private:
    ShadowRenderer();
    ~ShadowRenderer();

public:
    void renderShadowNodes();
    irr::video::ITexture* getShadowMap() const; // inline
    const irr::core::matrix4& getProjectionMatrix() const; // inline
    const irr::core::matrix4& getViewMatrix() const; // inline
    float getMaxShadow() const; // inline

private:
    irr::video::ITexture*       shadowMap;
    irr::video::E_MATERIAL_TYPE shadowMaterial;
    irr::video::E_MATERIAL_TYPE vehicleMaterial;
    irr::core::matrix4          projectionMatrix;
    irr::core::matrix4          viewMatrix;
    float                       maxShadow;
};

inline irr::video::ITexture* ShadowRenderer::getShadowMap() const
{
    return shadowMap;
}

inline const irr::core::matrix4& ShadowRenderer::getProjectionMatrix() const
{
    return projectionMatrix;
}

inline const irr::core::matrix4& ShadowRenderer::getViewMatrix() const
{
    return viewMatrix;
}

inline float ShadowRenderer::getMaxShadow() const
{
    return maxShadow;
}

#endif // SHADOWRENDERER_H

