
#include <irrlicht.h>
#include <set>
#include <assert.h>

#include "ShadersSM20.h"
#include "TheGame.h"
#include "ConfigFile.h"

/*
#define ADD_TEXTURE_MATRIX_V \
        ptextureMatrix = cgGetNamedParameter(Vertex, "mTextureMatrix"); \
        if (ptextureMatrix) \
        { \
            core::matrix4 textureMatrix; \
            textureMatrix = lightCam->getProjectionMatrix(); \
            textureMatrix *= lightCam->getViewMatrix(); \
            textureMatrix *= driver->getTransform(video::ETS_WORLD); \
        	services->setMatrix(ptextureMatrix,ICGT_MATRIX_IDENTITY,textureMatrix); \
        }
*/
#define ADD_WORLD_VIEW_PROJ_V \
    irr::core::matrix4 cworldViewProj; \
    cworldViewProj = driver->getTransform(irr::video::ETS_PROJECTION); \
    cworldViewProj *= driver->getTransform(irr::video::ETS_VIEW); \
    cworldViewProj *= driver->getTransform(irr::video::ETS_WORLD); \
    services->setVertexShaderConstant("mWorldViewProj",cworldViewProj.pointer(), 16);

#define ADD_WORLD_V \
    irr::core::matrix4 cworld; \
    cworld = driver->getTransform(irr::video::ETS_WORLD); \
    services->setVertexShaderConstant("mWorld",cworld.pointer(), 16);

#define ADD_TRANS_WORLD_V \
    irr::core::matrix4 ctransWorld; \
    ctransWorld = driver->getTransform(irr::video::ETS_WORLD); \
    ctransWorld = ctransWorld.getTransposed(); \
    services->setVertexShaderConstant("mTransWorld",ctransWorld.pointer(), 16);

#define ADD_INV_WORLD_V \
    irr::core::matrix4 cinvWorld = driver->getTransform(irr::video::ETS_WORLD); \
    cinvWorld.makeInverse(); \
    services->setVertexShaderConstant("mInvWorld",cinvWorld.pointer(), 16);

#define ADD_LIGHT_COL_V \
    irr::video::SColorf col(0.98f, 0.98f, 0.98f); \
    services->setVertexShaderConstant("mLightColor",&col.r, 4);
//    irr::video::SColorf col = m_lnode->getLightData().DiffuseColor; \
//    services->setParameter3f(lightColor, col.r, col.g, col.b);

#define ADD_LIGHT_POS_V \
    irr::core::vector3df pos(0.01f, -1.0f, 0.03f); \
    services->setVertexShaderConstant("mLightPos",&pos.X, 3);
//    irr::core::vector3df pos = m_lnode->getLightData().Position; \
//    services->setParameter3f(lightPosition, pos.X, pos.Y, pos.Z);

#define ADD_EYEPOSITION_SHINE_V \
        irr::core::vector3df cameraPos = device->getSceneManager()->getActiveCamera()->getPosition(); \
        services->setVertexShaderConstant("eyePosition",&cameraPos.X, 3); \
        float shininess = 0.7f; \
        services->setVertexShaderConstant("shininess",&shininess, 1);

#define ADD_PARAM_F \
        services->setPixelShaderConstant("param", &material.MaterialTypeParam, 1);

#define ADD_TEX0 \
        float tex0 = 0.f; \
        services->setVertexShaderConstant("tex0", &tex0, 1);

#define ADD_TEX1 \
        float tex1 = 1.f; \
        services->setVertexShaderConstant("tex1", &tex1, 1);

class SM20_ShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    SM20_ShaderCallback()
        : device(TheGame::getInstance()->getDevice()), driver(TheGame::getInstance()->getDriver())
    {
        callbacks.insert(this);
    }

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, int userData)
    {
        //assert(0);
    }

public:
    irr::IrrlichtDevice*        device;
    irr::video::IVideoDriver*   driver;

public:
    static std::set<SM20_ShaderCallback*> callbacks;
};


/*
class SM20_ShaderCallback_quad2d : public SM20_ShaderCallback
{
public:
    //CGparameter tex0;
public:
    SM20_ShaderCallback_quad2d()
    {
    }

    virtual void OnSetConstants(ICgServices* services,const CGeffect& Effect,const CGpass& Pass,const CGprogram& Vertex,const CGprogram& Pixel,const irr::video::SMaterial& Material)
    {
    }

};
*/
class SM20_ShaderCallback_terrain : public SM20_ShaderCallback
{
public:
    SM20_ShaderCallback_terrain()
    {
    }

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, int userData)
    {
        ADD_WORLD_V
        ADD_WORLD_VIEW_PROJ_V
        ADD_INV_WORLD_V
        //ADD_TEXTURE_MATRIX_V
        ADD_LIGHT_COL_V
        ADD_LIGHT_POS_V
        //ADD_TEX0
        //ADD_TEX1
    }

};

class SM20_ShaderCallback_normal : public SM20_ShaderCallback
{
public:
    SM20_ShaderCallback_normal()
    {
    }

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, int userData)
    {
        ADD_WORLD_V
        ADD_WORLD_VIEW_PROJ_V
        ADD_INV_WORLD_V
        //ADD_TEXTURE_MATRIX_V
        ADD_LIGHT_COL_V
        ADD_LIGHT_POS_V
        //ADD_EYEPOSITION_SHINE_V
        //ADD_TEX0
    }

};

class SM20_ShaderCallback_normal_no_light : public SM20_ShaderCallback
{
public:
    SM20_ShaderCallback_normal_no_light()
    {
    }

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, int userData)
    {
        ADD_WORLD_V
        ADD_WORLD_VIEW_PROJ_V
        //ADD_INV_WORLD_V
        //ADD_TEXTURE_MATRIX_V
        //ADD_LIGHT_COL_V
        //ADD_LIGHT_POS_V
        //ADD_EYEPOSITION_SHINE_V
        //ADD_TEX0
        //ADD_PARAM_F
    }

};

class SM20_ShaderCallback_vehicle : public SM20_ShaderCallback
{
public:
    SM20_ShaderCallback_vehicle()
    {
    }

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, int userData)
    {
        ADD_WORLD_V
        ADD_WORLD_VIEW_PROJ_V
        ADD_INV_WORLD_V
        //ADD_TEXTURE_MATRIX_V
        ADD_LIGHT_COL_V
        ADD_LIGHT_POS_V
        ADD_EYEPOSITION_SHINE_V
        //ADD_TEX0
    }

};

std::set<SM20_ShaderCallback*> SM20_ShaderCallback::callbacks;

ShadersSM20::ShadersSM20()
    : Shaders()
{
    if (supportedSMVersion < 2)
    {
        return;
    }
    
    loadSM20Materials();
}

ShadersSM20::~ShadersSM20()
{
    for (std::set<SM20_ShaderCallback*>::iterator it = SM20_ShaderCallback::callbacks.begin();
         it != SM20_ShaderCallback::callbacks.end();
         it++)
    {
        delete *it;
    }
}

void ShadersSM20::loadSM20Materials()
{
    // Load resource paths from config file
    //materialMap.clear();

    const irr::c8* ogl_vs_version = "arbvp1";
    const irr::c8* ogl_ps_version = "arbfp1";
    const irr::c8* d3d_vs_version = "vs_2_0";
    const irr::c8* d3d_ps_version = "ps_2_0";
    const irr::video::E_VERTEX_SHADER_TYPE vs_version = irr::video::EVST_VS_2_0;
    const irr::video::E_PIXEL_SHADER_TYPE  ps_version = irr::video::EPST_PS_2_0;
    bool doAssert = false;
    bool doSleep = false;
    irr::video::E_GPU_SHADING_LANGUAGE shadingLanguage = irr::video::EGSL_CG;

    ConfigFile cf;
    cf.load("data/materials/sm20.cfg");

    dprintf(MY_DEBUG_NOTE, "Read SM20 materials file:\n");
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string materialName, keyName, typeName, shaderFile;
    while (seci.hasMoreElements())
    {
        shaderFile = "";
        materialName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tMaterial: %s\n", materialName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        if (materialName == "assert")
        {
            doAssert = true;
            continue;
        }
        if (materialName == "sleep")
        {
            doSleep = true;
            continue;
        }
        for (ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            typeName = i->second;

            if (keyName == "shader")
            {
                shaderFile = typeName;
            }
        }
        
        
        if (materialName != "" && shaderFile != "")
        {
            irr::video::E_MATERIAL_TYPE baseMaterial = irr::video::EMT_SOLID;
            materialMap_t::const_iterator it = materialMap.find(materialName);
            if (it != materialMap.end())
            {
                baseMaterial = it->second;
            }
            if (materialName=="terrain")
            {
                // use specific shader CB
                materialMap[materialName] = (irr::video::E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterialFromFiles(
                    shaderFile.c_str(), "main_v", vs_version,
                    shaderFile.c_str(), "main_f", ps_version,
                    new SM20_ShaderCallback_terrain(), irr::video::EMT_SOLID/*irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL baseMaterial*/, 0, shadingLanguage);
            }
            else if (materialName=="normal" || materialName=="normal_t")
            {
                // use specific shader CB
                materialMap[materialName] = (irr::video::E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterialFromFiles(
                    shaderFile.c_str(), "main_v", vs_version,
                    shaderFile.c_str(), "main_f", ps_version,
                    new SM20_ShaderCallback_normal(), /*irr::video::EMT_SOLIDirr::video::EMT_TRANSPARENT_ALPHA_CHANNEL*/baseMaterial, 0, shadingLanguage);
            }
            else if (materialName=="normal_no_light" || materialName=="normal_no_light_t")
            {
                // use specific shader CB
                materialMap[materialName] = (irr::video::E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterialFromFiles(
                    shaderFile.c_str(), "main_v", vs_version,
                    shaderFile.c_str(), "main_f", ps_version,
                    new SM20_ShaderCallback_normal_no_light(), /*irr::video::EMT_SOLIDirr::video::EMT_TRANSPARENT_ALPHA_CHANNEL*/baseMaterial, 0, shadingLanguage);
            }
            else if (materialName=="vehicle")
            {
                // use specific shader CB
                materialMap[materialName] = (irr::video::E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterialFromFiles(
                    shaderFile.c_str(), "main_v", vs_version,
                    shaderFile.c_str(), "main_f", ps_version,
                    new SM20_ShaderCallback_vehicle(), /*irr::video::EMT_SOLIDirr::video::EMT_TRANSPARENT_ALPHA_CHANNEL*/baseMaterial, 0, shadingLanguage);
            }
            else
            {
                materialMap[materialName] = (irr::video::E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterialFromFiles(
                    shaderFile.c_str(), "main_v", vs_version,
                    shaderFile.c_str(), "main_f", ps_version,
                    new SM20_ShaderCallback(), /*irr::video::EMT_SOLIDirr::video::EMT_TRANSPARENT_ALPHA_CHANNEL*/baseMaterial, 0 , shadingLanguage);
            }
        }
    }
    if (doAssert)
    {
        assert(0);
    }
    if (doSleep)
    {
        TheGame::getInstance()->getDevice()->sleep(30000);
    }
}

