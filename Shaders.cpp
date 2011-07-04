
#include <irrlicht.h>

#include "Shaders.h"
#include "TheGame.h"
#include "ConfigFile.h"
#include "ShadersSM20.h"


Shaders* Shaders::shaders = 0;

void Shaders::initialize()
{
    if (shaders == 0)
    {
        shaders = new ShadersSM20();
    }
}

void Shaders::finalize()
{
    if (shaders)
    {
        delete shaders;
        shaders = 0;
    }
}


Shaders::Shaders()
    : gpu(0),
      supportedSMVersion(0)
{
    gpu = TheGame::getInstance()->getDriver()->getGPUProgrammingServices();

    printf("Shader support:\n");
    printf("hlsl:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_HLSL));
    printf("vs20:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_VERTEX_SHADER_2_0));
    printf("ps20:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_PIXEL_SHADER_2_0));
    printf("vs30:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_VERTEX_SHADER_3_0));
    printf("ps30:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_PIXEL_SHADER_3_0));
    printf("glsl:   %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_GLSL));
    printf("arb_vp: %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_VERTEX_PROGRAM_1));
    printf("arb_fp: %d\n", TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_FRAGMENT_PROGRAM_1));

    irr::video::E_DRIVER_TYPE driverType = TheGame::getInstance()->getDriver()->getDriverType();

    if ((driverType == irr::video::EDT_DIRECT3D9 && TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_HLSL) &&
         TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_PIXEL_SHADER_2_0) && TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_VERTEX_SHADER_2_0)
        ) ||
        ((driverType == irr::video::EDT_OPENGL /*|| driverType == video::EDT_OPENGL3*/) && TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_GLSL) &&
         TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_FRAGMENT_PROGRAM_1) && TheGame::getInstance()->getDriver()->queryFeature(irr::video::EVDF_ARB_VERTEX_PROGRAM_1)
        )
       )
    {
        supportedSMVersion = 2;
    }
    
    loadBaseMaterials();
}

Shaders::~Shaders()
{
    /*if (gpu)
    {
        delete gpu;
        gpu = 0;
    }*/
}

void Shaders::loadBaseMaterials()
{
    // Load resource paths from config file
    materialMap.clear();

    ConfigFile cf;
    cf.load("data/materials/base_materials.cfg");

    dprintf(MY_DEBUG_NOTE, "Read base materials file:\n");
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string materialName, keyName, materialType, typeName;
    while (seci.hasMoreElements())
    {
        materialType = "";
        materialName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tMaterial: %s\n", materialName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        for (ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            typeName = i->second;

            if (keyName == "base_material")
            {
                materialType = typeName;
            }
        }
        
        if (materialName != "")
        {
            materialMap[materialName] = stringToBaseType(materialType);
        }
    }
}

irr::video::E_MATERIAL_TYPE Shaders::stringToBaseType(const std::string& baseMaterialName)
{
    if (baseMaterialName == "alpha" || baseMaterialName == "transparent")
    {
        return irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
    }
    else if (baseMaterialName == "detailmap")
    {
        return irr::video::EMT_DETAIL_MAP;
    }
    else

    {
        return irr::video::EMT_SOLID;
    }
}

