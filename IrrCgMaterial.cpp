// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

#include "irrlicht.h"

#include <Cg/cg.h>

#ifdef IrrCgOGL
#include <Cg/cgGL.h>
#endif

#ifdef IrrCgD3D9
#include <Cg/cgD3D9.h>
#endif

#include "IrrCg.h"
#include "IrrCgMaterial.h"

#include <iostream>

using namespace std;

namespace IrrCg
{
    // class ICgMaterialTexture
    ICgMaterialTexture::ICgMaterialTexture(ICgServices* services, CGprogram prog, const char* name, int texid)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        if(texid > -1 && texid < 4)
        TextureID = texid;
        else
        TextureID = 0;
    }
    void ICgMaterialTexture::setValue(const irr::video::SMaterial& Material)
    {
        CgServices->EnableTexture( param,Material.TextureLayer[TextureID].Texture );
    }
    // End of class ICgMaterialTexture

    // class ICgMaterialParameter1d
    ICgMaterialParameter1d::ICgMaterialParameter1d(ICgServices* services, CGprogram prog, const char* name, double v)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuev = v;
    }
    void ICgMaterialParameter1d::setValue()
    {
        CgServices->setParameter1d(param, valuev);
    }
    // End of class ICgMaterialParameter1d

    // class ICgMaterialParameter1f
    ICgMaterialParameter1f::ICgMaterialParameter1f(ICgServices* services, CGprogram prog, const char* name, float v)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuev = v;
    }
    void ICgMaterialParameter1f::setValue()
    {
        CgServices->setParameter1f(param, valuev);
    }
    // End of class ICgMaterialParameter1f

    // class ICgMaterialParameter1i
    ICgMaterialParameter1i::ICgMaterialParameter1i(ICgServices* services, CGprogram prog, const char* name, int v)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuev = v;
    }
    void ICgMaterialParameter1i::setValue()
    {
        CgServices->setParameter1i(param, valuev);
    }
    // End of class ICgMaterialParameter1i

    // class ICgMaterialParameter2d
    ICgMaterialParameter2d::ICgMaterialParameter2d(ICgServices* services, CGprogram prog, const char* name, double x, double y)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
    }
    void ICgMaterialParameter2d::setValue()
    {
        CgServices->setParameter2d(param, valuex, valuey);
    }
    // End of class ICgMaterialParameter2d

    // class ICgMaterialParameter2f
    ICgMaterialParameter2f::ICgMaterialParameter2f(ICgServices* services, CGprogram prog, const char* name, float x, float y)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
    }
    void ICgMaterialParameter2f::setValue()
    {
        CgServices->setParameter2f(param, valuex, valuey);
    }
    // End of class ICgMaterialParameter2f

    // class ICgMaterialParameter2i
    ICgMaterialParameter2i::ICgMaterialParameter2i(ICgServices* services, CGprogram prog, const char* name, int x, int y)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
    }
    void ICgMaterialParameter2i::setValue()
    {
        CgServices->setParameter2i(param, valuex, valuey);
    }
    // End of class ICgMaterialParameter2i

    // class ICgMaterialParameter3d
    ICgMaterialParameter3d::ICgMaterialParameter3d(ICgServices* services, CGprogram prog, const char* name, double x, double y, double z)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
    }
    void ICgMaterialParameter3d::setValue()
    {
        CgServices->setParameter3d(param, valuex, valuey, valuez);
    }
    // End of class ICgMaterialParameter3d

    // class ICgMaterialParameter3f
    ICgMaterialParameter3f::ICgMaterialParameter3f(ICgServices* services, CGprogram prog, const char* name, float x, float y, float z)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
    }
    void ICgMaterialParameter3f::setValue()
    {
        CgServices->setParameter3f(param, valuex, valuey, valuez);
    }
    // End of class ICgMaterialParameter3f

    // class ICgMaterialParameter3i
    ICgMaterialParameter3i::ICgMaterialParameter3i(ICgServices* services, CGprogram prog, const char* name, int x, int y, int z)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
    }
    void ICgMaterialParameter3i::setValue()
    {
        CgServices->setParameter3i(param, valuex, valuey, valuez);
    }
    // End of class ICgMaterialParameter3i

    // class ICgMaterialParameter4d
    ICgMaterialParameter4d::ICgMaterialParameter4d(ICgServices* services, CGprogram prog, const char* name, double x, double y, double z, double w)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
        valuew = w;
    }
    void ICgMaterialParameter4d::setValue()
    {
        CgServices->setParameter4d(param, valuex, valuey, valuez, valuew);
    }
    // End of class ICgMaterialParameter4d

    // class ICgMaterialParameter4f
    ICgMaterialParameter4f::ICgMaterialParameter4f(ICgServices* services, CGprogram prog, const char* name, float x, float y, float z, float w)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
        valuew = w;
    }
    void ICgMaterialParameter4f::setValue()
    {
        CgServices->setParameter4f(param, valuex, valuey, valuez, valuew);
    }
    // End of class ICgMaterialParameter4f

    // class ICgMaterialParameter4i
    ICgMaterialParameter4i::ICgMaterialParameter4i(ICgServices* services, CGprogram prog, const char* name, int x, int y, int z, int w)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);

        valuex = x;
        valuey = y;
        valuez = z;
        valuew = w;
    }
    void ICgMaterialParameter4i::setValue()
    {
        CgServices->setParameter4i(param, valuex, valuey, valuez, valuew);
    }
    // End of class ICgMaterialParameter4i

    // class ICgMaterialParameterCameraPosition
    ICgMaterialParameterCameraPosition::ICgMaterialParameterCameraPosition(ICgServices* services, CGprogram prog, const char* name)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
    }
    void ICgMaterialParameterCameraPosition::setValue()
    {
        irr::core::vector3df CameraPosition = CgServices->getDevice()->getSceneManager()->getActiveCamera()->getAbsolutePosition();
        CgServices->setParameter3f(param, CameraPosition.X, CameraPosition.Y, CameraPosition.Z);
    }
    // End of class ICgMaterialParameterCameraPosition

    // class ICgMaterialParameterWVPMatrix
    ICgMaterialParameterWVPMatrix::ICgMaterialParameterWVPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterWVPMatrix::setValue()
    {
        CgServices->setWorldViewProjectionMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterWVPMatrix

    // class ICgMaterialParameterWVMatrix
    ICgMaterialParameterWVMatrix::ICgMaterialParameterWVMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterWVMatrix::setValue()
    {
        CgServices->setWorldViewMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterWVMatrix

    // class ICgMaterialParameterWMatrix
    ICgMaterialParameterWMatrix::ICgMaterialParameterWMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterWMatrix::setValue()
    {
        CgServices->setWorldMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterWMatrix

    // class ICgMaterialParameterVMatrix
    ICgMaterialParameterVMatrix::ICgMaterialParameterVMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterVMatrix::setValue()
    {
        CgServices->setViewMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterVMatrix

    // class ICgMaterialParameterVPMatrix
    ICgMaterialParameterVPMatrix::ICgMaterialParameterVPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterVPMatrix::setValue()
    {
        CgServices->setViewProjectionMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterVPMatrix

    // class ICgMaterialParameterPMatrix
    ICgMaterialParameterPMatrix::ICgMaterialParameterPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans)
    {
        CgServices = services;
        param = cgGetNamedParameter(prog, name);
        Transform = trans;
    }
    void ICgMaterialParameterPMatrix::setValue()
    {
        CgServices->setProjectionMatrix(param, Transform);
    }
    // End of class ICgMaterialParameterPMatrix

    // class ICgMaterialLoader
    ICgMaterialLoader::ICgMaterialLoader(ICgServices* vCgServices) : VSProgram(""), VSEntryPointName(""), VSProfileGL(""), VSProfileDX(""),
                        PSProgram(""), PSEntryPointName(""), PSProfileGL(""), PSProfileDX(""), BaseMaterial(irr::video::EMT_SOLID), CgServices(vCgServices)
    {
    }

    ICgMaterialLoader::~ICgMaterialLoader()
    {
    }

    bool ICgMaterialLoader::loadMaterial( const irr::c8* Material,ICgMaterialConstantSetCallBack* mCallback,ICgMaterialRenderer* &mRenderer,const char ** args )
    {
        if(!mCallback)
        {
            printf("IrrCg Error! Material Callback not found!\n");
            return false;
        }

        irr::io::IXMLReader* File = CgServices->getDevice()->getFileSystem()->createXMLReader(Material);

        if(File == NULL)
        {
            printf("IrrCg Error! Material file not found!\n");
            return false;
        }

        while(File && File->read())
        {
            if(irr::core::stringc("Program") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                VSProgram = File->getAttributeValue(L"Vertex");
                PSProgram = File->getAttributeValue(L"Pixel");
            }
            if(irr::core::stringc("EntryPointName") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                VSEntryPointName = File->getAttributeValue(L"Vertex");
                PSEntryPointName = File->getAttributeValue(L"Pixel");
            }
            if(irr::core::stringc("OpenGLProfile") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                VSProfileGL = File->getAttributeValue(L"Vertex");
                PSProfileGL = File->getAttributeValue(L"Pixel");
            }
            if(irr::core::stringc("DirectXProfile") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                VSProfileDX = File->getAttributeValue(L"Vertex");
                PSProfileDX = File->getAttributeValue(L"Pixel");
            }
            if(irr::core::stringc("Material") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Type = File->getAttributeValue(L"Type");

                if(Type == "EMT_SOLID")
                BaseMaterial = irr::video::EMT_SOLID;
                if(Type == "EMT_SOLID_2_LAYER")
                BaseMaterial = irr::video::EMT_SOLID_2_LAYER;
                if(Type == "EMT_LIGHTMAP")
                BaseMaterial = irr::video::EMT_LIGHTMAP;
                if(Type == "EMT_LIGHTMAP_ADD")
                BaseMaterial = irr::video::EMT_LIGHTMAP_ADD;
                if(Type == "EMT_LIGHTMAP_M2")
                BaseMaterial = irr::video::EMT_LIGHTMAP_M2;
                if(Type == "EMT_LIGHTMAP_M4 ")
                BaseMaterial = irr::video::EMT_LIGHTMAP_M4 ;
                if(Type == "EMT_LIGHTMAP_LIGHTING")
                BaseMaterial = irr::video::EMT_LIGHTMAP_LIGHTING;
                if(Type == "EMT_LIGHTMAP_LIGHTING_M2")
                BaseMaterial = irr::video::EMT_LIGHTMAP_LIGHTING_M2;
                if(Type == "EMT_LIGHTMAP_LIGHTING_M4")
                BaseMaterial = irr::video::EMT_LIGHTMAP_LIGHTING_M4;
                if(Type == "EMT_DETAIL_MAP")
                BaseMaterial = irr::video::EMT_DETAIL_MAP;
                if(Type == "EMT_SPHERE_MAP")
                BaseMaterial = irr::video::EMT_SPHERE_MAP;
                if(Type == "EMT_REFLECTION_2_LAYER")
                BaseMaterial = irr::video::EMT_REFLECTION_2_LAYER;
                if(Type == "EMT_TRANSPARENT_ADD_COLOR")
                BaseMaterial = irr::video::EMT_TRANSPARENT_ADD_COLOR;
                if(Type == "EMT_TRANSPARENT_ALPHA_CHANNEL")
                BaseMaterial = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
                if(Type == "EMT_TRANSPARENT_ALPHA_CHANNEL_REF")
                BaseMaterial = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
                if(Type == "EMT_TRANSPARENT_VERTEX_ALPHA")
                BaseMaterial = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;
                if(Type == "EMT_TRANSPARENT_REFLECTION_2_LAYER")
                BaseMaterial = irr::video::EMT_TRANSPARENT_REFLECTION_2_LAYER;
                if(Type == "EMT_NORMAL_MAP_SOLID")
                BaseMaterial = irr::video::EMT_NORMAL_MAP_SOLID;
                if(Type == "EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR")
                BaseMaterial = irr::video::EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR;
                if(Type == "EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA")
                BaseMaterial = irr::video::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA;
                if(Type == "EMT_PARALLAX_MAP_SOLID")
                BaseMaterial = irr::video::EMT_PARALLAX_MAP_SOLID;
                if(Type == "EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR")
                BaseMaterial = irr::video::EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR;
                if(Type == "EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA")
                BaseMaterial = irr::video::EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA;
                if(Type == "EMT_ONETEXTURE_BLEND")
                BaseMaterial = irr::video::EMT_ONETEXTURE_BLEND;
                if(Type == "EMT_FORCE_32BIT")
                BaseMaterial = irr::video::EMT_FORCE_32BIT;
            }
        }

        File->drop();

        if(mRenderer)
        mRenderer->drop();

        mRenderer = new ICgMaterialRenderer(CgServices,CgServices->getDevice()->getVideoDriver()->getMaterialRenderer(BaseMaterial),mCallback, false);

        CGerror Error;

        if(VSProgram != "")
        {
            FILE* file = fopen(VSProgram.c_str(), "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Vertex Shader:\nFile \"%s\" not found.\n", VSProgram.c_str());

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->VertexProfile,VSProfileGL.c_str(),VSProfileDX.c_str());
            //cout << "IrrCg Info: Vertex Profile is \"" << cgGetProfileString(mRenderer->VertexProfile) << "\"\n";

            mRenderer->VertexProgram = cgCreateProgramFromFile(CgServices->getContext(), CG_SOURCE, VSProgram.c_str(), mRenderer->VertexProfile, VSEntryPointName.c_str(), args);

            if (mRenderer->VertexProgram == NULL)
            {
                printf("IrrCg Error! Vertex Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return false;
            }
            else
            CgServices->LoadProgram(mRenderer->VertexProgram);
        }

        if(PSProgram != "")
        {
            FILE* file = fopen(PSProgram.c_str(), "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Pixel Shader:\nFile \"%s\" not found.\n", PSProgram.c_str());

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->PixelProfile,PSProfileGL.c_str(),PSProfileDX.c_str());
            //cout << "IrrCg Info: Pixel Profile is \"" << cgGetProfileString(mRenderer->PixelProfile) << "\"\n";

            mRenderer->PixelProgram = cgCreateProgramFromFile(CgServices->getContext(), CG_SOURCE, PSProgram.c_str(), mRenderer->PixelProfile, PSEntryPointName.c_str(), args);

            if (mRenderer->PixelProgram == NULL)
            {
                printf("IrrCg Error! Pixel Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return false;
            }
            else
            CgServices->LoadProgram(mRenderer->PixelProgram);
        }

        File = CgServices->getDevice()->getFileSystem()->createXMLReader(Material);

        while(File && File->read())
        {
            if(irr::core::stringc("Texture") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc ID = File->getAttributeValue(L"ID");

                int nID = atoi(ID.c_str());

                if(Program == "Vertex")
                mCallback->Texture.push_back(new ICgMaterialTexture(CgServices,mRenderer->VertexProgram,Name.c_str(),nID));
                else
                mCallback->Texture.push_back(new ICgMaterialTexture(CgServices,mRenderer->PixelProgram,Name.c_str(),nID));
            }
            if(irr::core::stringc("Parameter1d") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");

                double nX = atof(X.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter1d(CgServices,mRenderer->VertexProgram,Name.c_str(),nX));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter1d(CgServices,mRenderer->PixelProgram,Name.c_str(),nX));
            }
            if(irr::core::stringc("Parameter1f") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");

                float nX = atof(X.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter1f(CgServices,mRenderer->VertexProgram,Name.c_str(),nX));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter1f(CgServices,mRenderer->PixelProgram,Name.c_str(),nX));
            }
            if(irr::core::stringc("Parameter1i") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");

                int nX = atoi(X.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter1i(CgServices,mRenderer->VertexProgram,Name.c_str(),nX));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter1i(CgServices,mRenderer->PixelProgram,Name.c_str(),nX));
            }
            if(irr::core::stringc("Parameter2d") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");

                double nX = atof(X.c_str());
                double nY = atof(Y.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter2d(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter2d(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY));
            }
            if(irr::core::stringc("Parameter2f") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");

                float nX = atof(X.c_str());
                float nY = atof(Y.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter2f(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter2f(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY));
            }
            if(irr::core::stringc("Parameter2i") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");

                int nX = atoi(X.c_str());
                int nY = atoi(Y.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter2i(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter2i(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY));
            }
            if(irr::core::stringc("Parameter3d") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");

                double nX = atof(X.c_str());
                double nY = atof(Y.c_str());
                double nZ = atof(Z.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter3d(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter3d(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ));
            }
            if(irr::core::stringc("Parameter3f") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");

                float nX = atof(X.c_str());
                float nY = atof(Y.c_str());
                float nZ = atof(Z.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter3f(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter3f(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ));
            }
            if(irr::core::stringc("Parameter3i") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");

                int nX = atoi(X.c_str());
                int nY = atoi(Y.c_str());
                int nZ = atoi(Z.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter3i(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter3i(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ));
            }
            if(irr::core::stringc("Parameter4d") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");
                irr::core::stringc W = File->getAttributeValue(L"W");

                double nX = atof(X.c_str());
                double nY = atof(Y.c_str());
                double nZ = atof(Z.c_str());
                double nW = atof(W.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter4d(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ,nW));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter4d(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ,nW));
            }
            if(irr::core::stringc("Parameter4f") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");
                irr::core::stringc W = File->getAttributeValue(L"W");

                float nX = atof(X.c_str());
                float nY = atof(Y.c_str());
                float nZ = atof(Z.c_str());
                float nW = atof(W.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter4f(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ,nW));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter4f(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ,nW));
            }
            if(irr::core::stringc("Parameter4i") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc X = File->getAttributeValue(L"X");
                irr::core::stringc Y = File->getAttributeValue(L"Y");
                irr::core::stringc Z = File->getAttributeValue(L"Z");
                irr::core::stringc W = File->getAttributeValue(L"W");

                int nX = atoi(X.c_str());
                int nY = atoi(Y.c_str());
                int nZ = atoi(Z.c_str());
                int nW = atoi(W.c_str());

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameter4i(CgServices,mRenderer->VertexProgram,Name.c_str(),nX,nY,nZ,nW));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameter4i(CgServices,mRenderer->PixelProgram,Name.c_str(),nX,nY,nZ,nW));
            }
            if(irr::core::stringc("ParameterCameraPosition") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterCameraPosition(CgServices,mRenderer->VertexProgram,Name.c_str()));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterCameraPosition(CgServices,mRenderer->PixelProgram,Name.c_str()));
            }
            if(irr::core::stringc("ParameterWVPMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterWVPMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterWVPMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
            if(irr::core::stringc("ParameterWVMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterWVMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterWVMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
            if(irr::core::stringc("ParameterWMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterWMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterWMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
            if(irr::core::stringc("ParameterVMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterVMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterVMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
            if(irr::core::stringc("ParameterVPMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterVPMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterVPMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
            if(irr::core::stringc("ParameterPMatrix") == File->getNodeName() && File->getNodeType() == irr::io::EXN_ELEMENT)
            {
                irr::core::stringc Program = File->getAttributeValue(L"Program");
                irr::core::stringc Name = File->getAttributeValue(L"Name");
                irr::core::stringc Transform = File->getAttributeValue(L"Transform");

                IRR_CG_TRANSFORM nTransform = ICGT_MATRIX_IDENTITY;

                if(Transform == "MATRIX_IDENTITY")
                nTransform = ICGT_MATRIX_IDENTITY;
                if(Transform == "MATRIX_TRANSPOSE")
                nTransform = ICGT_MATRIX_TRANSPOSE;
                if(Transform == "MATRIX_INVERSE")
                nTransform = ICGT_MATRIX_INVERSE;
                if(Transform == "MATRIX_INVERSE_TRANSPOSE")
                nTransform = ICGT_MATRIX_INVERSE_TRANSPOSE;

                if(Program == "Vertex")
                mCallback->Parameter.push_back(new ICgMaterialParameterPMatrix(CgServices,mRenderer->VertexProgram,Name.c_str(),nTransform));
                else
                mCallback->Parameter.push_back(new ICgMaterialParameterPMatrix(CgServices,mRenderer->PixelProgram,Name.c_str(),nTransform));
            }
        }

        File->drop();

        return true;
    }
}
