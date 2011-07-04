// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

// You have to define IrrCgWindows, if You compile on Windows. Require Windows PlatformSDK.
// You have to define IrrCgLinux, if You compile on Linux.

// You have to define IrrCgD3D9, if You need Cg support for Direct3D program. Require DirectX SDK.
// You have to define IrrCgOGL, if You need Cg support for OpenGL program.

// You have to define it, if You compile on Windows. Require Windows PlatformSDK.
#ifndef __linux__
#define _IRR_COMPILE_WITH_DIRECT3D_9_
#define IrrCgWindows

// You have to define it, if You compile on Linux.
// #define IrrCgLinux

// You have to define it, if You use Irrlicht Linux Shared Library (*.so)
// #define IrrCgSharedLib

// You have to define it, if You need Cg support for Direct3D program. Require DirectX SDK.
#define IrrCgD3D9
#else
#define IrrCgLinux
#endif

// You have to define it, if You need Cg support for OpenGL program.
#define IrrCgOGL

// Irrlicht Header
#include "irrlicht.h"

#ifdef IrrCgWindows
#include <windows.h>
#endif

#include <Cg/cg.h>

#ifdef IrrCgOGL
#define GL_GLEXT_LEGACY 1
#include <GL/gl.h>
#include <Cg/cgGL.h>
#endif

#ifdef IrrCgD3D9
#include <d3d9.h>
#include <Cg/cgD3D9.h>
#endif

// Irrlicht Source Textures Headers
#ifdef IrrCgD3D9
#include "CD3D9Texture.h"
#endif
#ifdef IrrCgOGL
#if IRRLICHT_VERSION_MINOR < 7
#include "COpenGLDriver.h"
#else
#include "glext.h"
#endif
#include "COpenGLTexture.h"
#endif

// IrrCg header
#include "IrrCgExtensionHandler.h"
#include "IrrCgMaterial.h"
#include "IrrCg.h"

// String header
#include <string>

// Used namespaces
using namespace irr;
using namespace core;
using namespace io;
using namespace scene;
using namespace video;
using namespace std;
using namespace IrrCg;

#ifdef IrrCgWindows

// Get OpenGL Texture Name. Like standard Irrlicht getOpenGLTextureName().
#ifdef IrrCgOGL
GLuint irr::video::COpenGLTexture::getOpenGLTextureName() const
{
    return TextureName;
}

#if IRRLICHT_VERSION_MINOR < 7
// Set OpenGL Active Texture. Like standard Irrlicht OpenGL setActiveTexture().
bool irr::video::COpenGLDriver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= MaxTextureUnits)
		return false;

	if (CurrentTexture[stage]==texture)
		return true;

	if (MultiTextureExtension)
		extGlActiveTexture(GL_TEXTURE0_ARB + stage);

	CurrentTexture[stage]=texture;

	if (!texture)
	{
		glDisable(GL_TEXTURE_2D);
		return true;
	}
	else
	{
		if (texture->getDriverType() != EDT_OPENGL && texture->getDriverType() != EDT_OPENGL3)
		{
			glDisable(GL_TEXTURE_2D);
			printf("Fatal Error: Tried to set a texture not owned by this driver.");
			return false;
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,
			static_cast<const COpenGLTexture*>(texture)->getOpenGLTextureName());
	}
	return true;
}
#endif

#endif // IrrCgOGL

// Get Direct3D Texture Name. Like standard Irrlicht getDX9Texture().
#ifdef IrrCgD3D9
IDirect3DBaseTexture9* irr::video::CD3D9Texture::getDX9Texture() const
{
    return Texture;
}
#endif // IrrCgD3D9

#endif // IrrCgWindows

namespace IrrCg
{
    // class ICgOGLServices
    #ifdef IrrCgOGL
    ICgOGLServices::ICgOGLServices(irr::IrrlichtDevice* vDevice) : Device(vDevice)
    {
        Context = cgCreateContext();
        Type = ICGT_OPENGL;

        ExtensionHandler = new IrrCgExtensionHandler();
    }
    ICgOGLServices::~ICgOGLServices()
    {
        delete ExtensionHandler;

        if(Context)
        cgDestroyContext(Context);
    }
    void ICgOGLServices::registerStates()
    {
        cgGLRegisterStates(Context);
    }
    bool ICgOGLServices::getManageTextureParameters()
    {
        return cgGLGetManageTextureParameters(Context);
    }
    void ICgOGLServices::setManageTextureParameters( bool flag )
    {
        cgGLSetManageTextureParameters(Context, flag);
    }
    bool ICgOGLServices::isProfileSupported( const char * profile_string )
    {
        return cgGLIsProfileSupported(cgGetProfile(profile_string));
    }
    void ICgOGLServices::getProfile( CGprofile &profile, const char * gl_string, const char * d3d_string )
    {
        stringc tProfile = gl_string;

        if(tProfile == "LatestVertexProfile")
        profile = cgGLGetLatestProfile(CG_GL_VERTEX);
        else
        if(tProfile == "LatestPixelProfile")
        profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        else
        profile = cgGetProfile(gl_string);

        if (profile == CG_PROFILE_UNKNOWN)
        printf("Invalid profile type\n");
    }
    void ICgOGLServices::LoadProgram( CGprogram program,bool paramShadowing )
    {
        cgGLLoadProgram( program );
    }
    void ICgOGLServices::EnableShader( CGprogram program,CGprofile profile )
    {
        cgGLEnableProfile( profile );
        cgGLBindProgram( program );
    }
    void ICgOGLServices::DisableShader( CGprofile profile )
    {
        cgGLDisableProfile( profile );
    }
    void ICgOGLServices::DisableEffect()
    {
    }
    // Textures Functions.
    void ICgOGLServices::EnableTexture( CGparameter param,irr::video::ITexture* Tex2D )
    {
        if(Tex2D)
        {
            int num = reinterpret_cast<irr::video::COpenGLTexture*>(Tex2D)->getOpenGLTextureName();

            cgGLSetTextureParameter(param, num);
            cgGLEnableTextureParameter( param );
        }
    }
    // MY_MOD
    void ICgOGLServices::EnableTextureSM( CGparameter param,irr::video::ITexture* Tex2D )
    {
        EnableTexture(param,Tex2D);
    }
    void ICgOGLServices::setTexture( CGparameter param,const irr::video::SMaterialLayer &TextureLayer,int Stage )
    {
        if(Stage >= 0 && TextureLayer.Texture)
        {
            int Texture = ((const irr::video::COpenGLTexture*)TextureLayer.Texture)->getOpenGLTextureName();

            ExtensionHandler->extGlActiveTexture(GL_TEXTURE0_ARB + Stage);
            glBindTexture(GL_TEXTURE_2D, Texture);

            #if IRRLICHT_VERSION_MINOR < 7

            GLint mode = GL_REPEAT;
            switch (TextureLayer.TextureWrap)
            {
                case irr::video::ETC_REPEAT:
                    mode=GL_REPEAT;
                    break;
                case irr::video::ETC_CLAMP:
                    mode=GL_CLAMP;
                    break;
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=GL_CLAMP_TO_EDGE;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=GL_CLAMP_TO_BORDER_ARB;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=GL_MIRRORED_REPEAT_ARB;
                    break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

            #else

            GLint mode = GL_REPEAT;
            switch (TextureLayer.TextureWrapU)
            {
                case irr::video::ETC_REPEAT:
                    mode=GL_REPEAT;
                    break;
                case irr::video::ETC_CLAMP:
                    mode=GL_CLAMP;
                    break;
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=GL_CLAMP_TO_EDGE;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=GL_CLAMP_TO_BORDER_ARB;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=GL_MIRRORED_REPEAT_ARB;
                    break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);

            mode = GL_REPEAT;
            switch (TextureLayer.TextureWrapV)
            {
                case irr::video::ETC_REPEAT:
                    mode=GL_REPEAT;
                    break;
                case irr::video::ETC_CLAMP:
                    mode=GL_CLAMP;
                    break;
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=GL_CLAMP_TO_EDGE;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=GL_CLAMP_TO_BORDER_ARB;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=GL_MIRRORED_REPEAT_ARB;
                    break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);

            #endif

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (TextureLayer.BilinearFilter || TextureLayer.TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

            if(TextureLayer.Texture->hasMipMaps())
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureLayer.TrilinearFilter ? GL_LINEAR_MIPMAP_LINEAR : TextureLayer.BilinearFilter ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST );
            else
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (TextureLayer.BilinearFilter || TextureLayer.TrilinearFilter) ? GL_LINEAR : GL_NEAREST);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, TextureLayer.AnisotropicFilter );

            cgGLSetTextureParameter(param, Texture);
            cgGLEnableTextureParameter( param );
        }
    }
    // setParameter Functions.
    void ICgOGLServices::setParameter1d( CGparameter param,double v )
    {
        cgSetParameter1d( param,v );
    }
    void ICgOGLServices::setParameter1f( CGparameter param,float v )
    {
        cgSetParameter1f( param,v );
    }
    void ICgOGLServices::setParameter1i( CGparameter param,int v )
    {
        cgSetParameter1i( param,v );
    }
    void ICgOGLServices::setParameter2d( CGparameter param,double x,double y )
    {
        cgSetParameter2d( param,x,y );
    }
    void ICgOGLServices::setParameter2f( CGparameter param,float x,float y )
    {
        cgSetParameter2f( param,x,y );
    }
    void ICgOGLServices::setParameter2i( CGparameter param,int x,int y )
    {
        cgSetParameter2i( param,x,y );
    }
    void ICgOGLServices::setParameter3d( CGparameter param,double x,double y,double z )
    {
        cgSetParameter3d( param,x,y,z );
    }
    void ICgOGLServices::setParameter3f( CGparameter param,float x,float y,float z )
    {
        cgSetParameter3f( param,x,y,z );
    }
    void ICgOGLServices::setParameter3i( CGparameter param,int x,int y,int z )
    {
        cgSetParameter3i( param,x,y,z );
    }
    void ICgOGLServices::setParameter4d( CGparameter param,double x,double y,double z,double w )
    {
        cgSetParameter4d( param,x,y,z,w );
    }
    void ICgOGLServices::setParameter4f( CGparameter param,float x,float y,float z,float w )
    {
        cgSetParameter4f( param,x,y,z,w );
    }
    void ICgOGLServices::setParameter4i( CGparameter param,int x,int y,int z,int w )
    {
        cgSetParameter4i( param,x,y,z,w );
    }
    // Matrix Functions.
    void ICgOGLServices::setWorldViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 WorldViewProjection;
        WorldViewProjection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
        WorldViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
        WorldViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        WorldViewProjection = WorldViewProjection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        WorldViewProjection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            WorldViewProjection.makeInverse();
            WorldViewProjection = WorldViewProjection.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,WorldViewProjection.pointer());
    }
    void ICgOGLServices::setWorldViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 WorldView;
        WorldView = Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
        WorldView *= Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        WorldView = WorldView.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        WorldView.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            WorldView.makeInverse();
            WorldView = WorldView.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,WorldView.pointer());
    }
    void ICgOGLServices::setWorldMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 World;
        World = Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        World = World.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        World.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            World.makeInverse();
            World = World.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,World.pointer());
    }
    void ICgOGLServices::setViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 View;
        View = Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        View = View.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        View.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            View.makeInverse();
            View = View.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,View.pointer());
    }
    void ICgOGLServices::setViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 ViewProjection;
        ViewProjection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
        ViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        ViewProjection = ViewProjection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        ViewProjection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            ViewProjection.makeInverse();
            ViewProjection = ViewProjection.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,ViewProjection.pointer());
    }
    void ICgOGLServices::setProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 Projection;
        Projection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        Projection = Projection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        Projection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            Projection.makeInverse();
            Projection = Projection.getTransposed();
        }
        cgGLSetMatrixParameterfc(param,Projection.pointer());
    }
    void ICgOGLServices::setMatrix( CGparameter param,IRR_CG_TRANSFORM trans,const irr::core::matrix4& mat )
    {
        /*
        if(trans == ICGT_MATRIX_TRANSPOSE)
        mat = mat.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        mat.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            mat.makeInverse();
            mat = mat.getTransposed();
        }
        */
        cgGLSetMatrixParameterfc(param,mat.pointer());
    }
    irr::IrrlichtDevice* ICgOGLServices::getDevice()
    {
        return Device;
    }
    CGcontext ICgOGLServices::getContext()
    {
        return Context;
    }
    IRR_CG_TYPE ICgOGLServices::getType()
    {
        return Type;
    }
    #endif
    // End of class ICgOGLServices

    // class ICgD3D9Services
    #ifdef IrrCgD3D9
    ICgD3D9Services::ICgD3D9Services(irr::IrrlichtDevice* vDevice) : Device(vDevice)
    {
        Context = cgCreateContext();
        Type = ICGT_DIRECT3D9;
        cgD3D9SetDevice(Device->getVideoDriver()->getExposedVideoData().D3D9.D3DDev9);
    }
    ICgD3D9Services::~ICgD3D9Services()
    {
        cgD3D9SetDevice(0);

        if(Context)
        cgDestroyContext(Context);
    }
    void ICgD3D9Services::registerStates()
    {
        cgD3D9RegisterStates(Context);
    }
    bool ICgD3D9Services::getManageTextureParameters()
    {
        return cgD3D9GetManageTextureParameters(Context);
    }
    void ICgD3D9Services::setManageTextureParameters( bool flag )
    {
        cgD3D9SetManageTextureParameters(Context, flag);
    }
    bool ICgD3D9Services::isProfileSupported( const char * profile_string )
    {
        return cgD3D9IsProfileSupported(cgGetProfile(profile_string));
    }
    void ICgD3D9Services::getProfile( CGprofile &profile, const char * gl_string, const char * d3d_string )
    {
        stringc tProfile = d3d_string;

        if(tProfile == "LatestVertexProfile")
        profile = cgD3D9GetLatestVertexProfile();
        else
        if(tProfile == "LatestPixelProfile")
        profile = cgD3D9GetLatestPixelProfile();
        else
        profile = cgGetProfile(d3d_string);

        if (profile == CG_PROFILE_UNKNOWN)
        printf("Invalid profile type\n");
    }
    void ICgD3D9Services::LoadProgram( CGprogram program,bool paramShadowing )
    {
        cgD3D9LoadProgram( program,paramShadowing,0 );
    }
    void ICgD3D9Services::EnableShader( CGprogram program,CGprofile profile )
    {
        cgD3D9BindProgram( program );
    }
    void ICgD3D9Services::DisableShader( CGprofile profile )
    {
        irr::video::SExposedVideoData VideoData = Device->getVideoDriver()->getExposedVideoData();

        VideoData.D3D9.D3DDev9->SetVertexShader(NULL);
        VideoData.D3D9.D3DDev9->SetPixelShader(NULL);
    }
    void ICgD3D9Services::DisableEffect()
    {
        irr::video::SExposedVideoData VideoData = Device->getVideoDriver()->getExposedVideoData();

        VideoData.D3D9.D3DDev9->SetVertexShader(NULL);
        VideoData.D3D9.D3DDev9->SetPixelShader(NULL);
    }
    // Textures Functions.
    void ICgD3D9Services::EnableTexture( CGparameter param,irr::video::ITexture* Tex2D )
    {
        if(Tex2D)
        {
            IDirect3DBaseTexture9* Texture = ((const irr::video::CD3D9Texture*)Tex2D)->getDX9Texture();

            cgD3D9SetTextureParameter( param,Texture );
        }
    }
    // MY_MOD
    void ICgD3D9Services::EnableTextureSM( CGparameter param,irr::video::ITexture* Tex2D )
    {
        if(Tex2D)
        {
            IDirect3DBaseTexture9* Texture = ((const irr::video::CD3D9Texture*)Tex2D)->getDX9Texture();

            //cgD3D9SetTextureParameter( param,Texture );
            cgD3D9SetTexture( param,Texture ); // MY_MOD
            //cgD3D9EnableTextureParameter( param );
        }
    }
    void ICgD3D9Services::setTexture( CGparameter param,const irr::video::SMaterialLayer &TextureLayer,int Stage )
    {
        if(Stage >= 0 && TextureLayer.Texture)
        {
            IDirect3DBaseTexture9* Texture = ((const irr::video::CD3D9Texture*)TextureLayer.Texture)->getDX9Texture();

            irr::video::SExposedVideoData VideoData = Device->getVideoDriver()->getExposedVideoData();

            VideoData.D3D9.D3DDev9->SetTexture(Stage, Texture);

            #if IRRLICHT_VERSION_MINOR < 7

            irr::u32 mode = D3DTADDRESS_WRAP;
            switch (TextureLayer.TextureWrap)
            {
                case irr::video::ETC_REPEAT:
                    mode=D3DTADDRESS_WRAP;
                    break;
                case irr::video::ETC_CLAMP:
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=D3DTADDRESS_CLAMP;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=D3DTADDRESS_MIRROR;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=D3DTADDRESS_BORDER;
                    break;
            }

            VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_ADDRESSU, mode );
            VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_ADDRESSV, mode );

            #else

            irr::u32 mode = D3DTADDRESS_WRAP;
            switch (TextureLayer.TextureWrapU)
            {
                case irr::video::ETC_REPEAT:
                    mode=D3DTADDRESS_WRAP;
                    break;
                case irr::video::ETC_CLAMP:
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=D3DTADDRESS_CLAMP;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=D3DTADDRESS_MIRROR;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=D3DTADDRESS_BORDER;
                    break;
            }

            VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_ADDRESSU, mode );

            mode = D3DTADDRESS_WRAP;
            switch (TextureLayer.TextureWrapV)
            {
                case irr::video::ETC_REPEAT:
                    mode=D3DTADDRESS_WRAP;
                    break;
                case irr::video::ETC_CLAMP:
                case irr::video::ETC_CLAMP_TO_EDGE:
                    mode=D3DTADDRESS_CLAMP;
                    break;
                case irr::video::ETC_MIRROR:
                    mode=D3DTADDRESS_MIRROR;
                    break;
                case irr::video::ETC_CLAMP_TO_BORDER:
                    mode=D3DTADDRESS_BORDER;
                    break;
            }

            VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_ADDRESSV, mode );

            #endif

            if (TextureLayer.BilinearFilter || TextureLayer.TrilinearFilter || TextureLayer.AnisotropicFilter)
            {
                D3DTEXTUREFILTERTYPE tftMag = TextureLayer.AnisotropicFilter ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
                D3DTEXTUREFILTERTYPE tftMin = TextureLayer.AnisotropicFilter ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
                D3DTEXTUREFILTERTYPE tftMip = TextureLayer.TrilinearFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT;

                if (tftMag==D3DTEXF_ANISOTROPIC || tftMin == D3DTEXF_ANISOTROPIC)
				VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MAXANISOTROPY, TextureLayer.AnisotropicFilter);

                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MAGFILTER, tftMag);
                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MINFILTER, tftMin);
                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MIPFILTER, tftMip);
            }
            else
            {
                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                VideoData.D3D9.D3DDev9->SetSamplerState(Stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
            }

            cgD3D9SetTextureParameter( param,Texture );
        }
    }
    // setParameter Functions.
    void ICgD3D9Services::setParameter1d( CGparameter param,double v )
    {
        cgSetParameter1d( param,v );
    }
    void ICgD3D9Services::setParameter1f( CGparameter param,float v )
    {
        cgSetParameter1f( param,v );
    }
    void ICgD3D9Services::setParameter1i( CGparameter param,int v )
    {
        cgSetParameter1i( param,v );
    }
    void ICgD3D9Services::setParameter2d( CGparameter param,double x,double y )
    {
        cgSetParameter2d( param,x,y );
    }
    void ICgD3D9Services::setParameter2f( CGparameter param,float x,float y )
    {
        cgSetParameter2f( param,x,y );
    }
    void ICgD3D9Services::setParameter2i( CGparameter param,int x,int y )
    {
        cgSetParameter2i( param,x,y );
    }
    void ICgD3D9Services::setParameter3d( CGparameter param,double x,double y,double z )
    {
        cgSetParameter3d( param,x,y,z );
    }
    void ICgD3D9Services::setParameter3f( CGparameter param,float x,float y,float z )
    {
        cgSetParameter3f( param,x,y,z );
    }
    void ICgD3D9Services::setParameter3i( CGparameter param,int x,int y,int z )
    {
        cgSetParameter3i( param,x,y,z );
    }
    void ICgD3D9Services::setParameter4d( CGparameter param,double x,double y,double z,double w )
    {
        cgSetParameter4d( param,x,y,z,w );
    }
    void ICgD3D9Services::setParameter4f( CGparameter param,float x,float y,float z,float w )
    {
        cgSetParameter4f( param,x,y,z,w );
    }
    void ICgD3D9Services::setParameter4i( CGparameter param,int x,int y,int z,int w )
    {
        cgSetParameter4i( param,x,y,z,w );
    }
    // Matrix Functions.
    void ICgD3D9Services::setWorldViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 WorldViewProjection;
        WorldViewProjection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
        WorldViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
        WorldViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        WorldViewProjection = WorldViewProjection.getTransposed();

        if(trans == ICGT_MATRIX_TRANSPOSE)
        WorldViewProjection = WorldViewProjection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        WorldViewProjection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            WorldViewProjection.makeInverse();
            WorldViewProjection = WorldViewProjection.getTransposed();
        }
        cgD3D9SetUniform(param,WorldViewProjection.pointer());
    }
    void ICgD3D9Services::setWorldViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 WorldView;
        WorldView = Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
        WorldView *= Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        WorldView = WorldView.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        WorldView.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            WorldView.makeInverse();
            WorldView = WorldView.getTransposed();
        }
        cgD3D9SetUniform(param,WorldView.pointer());
    }
    void ICgD3D9Services::setWorldMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 World;
        World = Device->getVideoDriver()->getTransform(irr::video::ETS_WORLD);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        World = World.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        World.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            World.makeInverse();
            World = World.getTransposed();
        }
        cgD3D9SetUniform(param,World.pointer());
    }
    void ICgD3D9Services::setViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 View;
        View = Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        View = View.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        View.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            View.makeInverse();
            View = View.getTransposed();
        }
        cgD3D9SetUniform(param,View.pointer());
    }
    void ICgD3D9Services::setViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 ViewProjection;
        ViewProjection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
        ViewProjection *= Device->getVideoDriver()->getTransform(irr::video::ETS_VIEW);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        ViewProjection = ViewProjection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        ViewProjection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            ViewProjection.makeInverse();
            ViewProjection = ViewProjection.getTransposed();
        }
        cgD3D9SetUniform(param,ViewProjection.pointer());
    }
    void ICgD3D9Services::setProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans )
    {
        irr::core::matrix4 Projection;
        Projection = Device->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);

        if(trans == ICGT_MATRIX_TRANSPOSE)
        Projection = Projection.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        Projection.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            Projection.makeInverse();
            Projection = Projection.getTransposed();
        }
        cgD3D9SetUniform(param,Projection.pointer());
    }
    void ICgD3D9Services::setMatrix( CGparameter param,IRR_CG_TRANSFORM trans,const irr::core::matrix4& mat )
    {
        irr::core::matrix4 mat2;
        mat.getTransposed(mat2);
        /*
        if(trans == ICGT_MATRIX_TRANSPOSE)
        mat = mat.getTransposed();
        if(trans == ICGT_MATRIX_INVERSE)
        mat.makeInverse();
        if(trans == ICGT_MATRIX_INVERSE_TRANSPOSE)
        {
            mat.makeInverse();
            mat = mat.getTransposed();
        }
        */
        cgD3D9SetUniform(param,mat2.pointer());
    }
    irr::IrrlichtDevice* ICgD3D9Services::getDevice()
    {
        return Device;
    }
    CGcontext ICgD3D9Services::getContext()
    {
        return Context;
    }
    IRR_CG_TYPE ICgD3D9Services::getType()
    {
        return Type;
    }
    #endif
    // End of class ICgD3D9Services

    // class ICgMaterialConstantSetCallBack
    // Constructor.
    ICgMaterialConstantSetCallBack::ICgMaterialConstantSetCallBack(ICgShaderConstantSetCallBack* callback)
    {
        CgCallback = callback;
    }

    // Destructor.
    ICgMaterialConstantSetCallBack::~ICgMaterialConstantSetCallBack()
    {
        for(int current = 0; current < Parameter.size(); current++)
        {
            delete Parameter[current];
        }

        for(int current = 0; current < Texture.size(); current++)
        {
            delete Texture[current];
        }
    }
    void ICgMaterialConstantSetCallBack::OnSetConstants( ICgServices* services,const CGeffect& Effect,const CGpass& Pass,const CGprogram& Vertex,const CGprogram& Fragment,const irr::video::SMaterial& Material )
    {
        for(int current = 0; current < Parameter.size(); current++)
        {
            Parameter[current]->setValue();
        }

        for(int current = 0; current < Texture.size(); current++)
        {
            Texture[current]->setValue(Material);
        }

        if(CgCallback)
        CgCallback->OnSetConstants( services,Effect,Pass,Vertex,Fragment,Material );
    }
    // End of class ICgMaterialConstantSetCallBack

    // class ICgProgrammingServices
    // Constructor.
    ICgProgrammingServices::ICgProgrammingServices(irr::IrrlichtDevice* Device, bool RegisterStates, bool ManageTextureParameters, CGenum ParameterSettingMode) : CgServices(0)
    {
        if(Device->getVideoDriver()->getDriverType() == irr::video::EDT_OPENGL /*||
           Device->getVideoDriver()->getDriverType() == irr::video::EDT_OPENGL3*/)
        {
            #ifdef IrrCgOGL
            CgServices = new ICgOGLServices(Device);

            if(RegisterStates)
            CgServices->registerStates();

            if(ManageTextureParameters)
            CgServices->setManageTextureParameters(true);

            cgSetParameterSettingMode(CgServices->getContext(), ParameterSettingMode);
            #else
            printf("IrrCg Error! Cg OpenGL Support was not compiled. Try another one.\n");
            #endif
        }
        else
        if(Device->getVideoDriver()->getDriverType() == irr::video::EDT_DIRECT3D9)
        {
            #ifdef IrrCgD3D9
            CgServices = new ICgD3D9Services(Device);

            if(RegisterStates)
            CgServices->registerStates();

            if(ManageTextureParameters)
            CgServices->setManageTextureParameters(true);

            cgSetParameterSettingMode(CgServices->getContext(), ParameterSettingMode);
            #else
            printf("IrrCg Error! Cg D3D9 Support was not compiled. Try another one.\n");
            #endif
        }
        else
        {
            printf("IrrCg Error! Services not found!\n");
        }
    }

    // Destructor.
    ICgProgrammingServices::~ICgProgrammingServices()
    {
        for(int c = 0; c < ProgramsList.size(); ++c)
        {
            if(ProgramsList[c])
            cgDestroyProgram(*ProgramsList[c]);
        }

        for(int c = 0; c < EffectsList.size(); ++c)
        {
            if(EffectsList[c])
            cgDestroyEffect(*EffectsList[c]);
        }

        ProgramsList.clear();
        EffectsList.clear();

        if(CgServices)
        delete CgServices;
    }

    // Function useful for add Cg Shader Material to Your program.
    irr::s32 ICgProgrammingServices::addCgShaderMaterial( CGenum program_type,
    const irr::c8* VSProgram,const irr::c8* VSEntryPointName,const irr::c8* VSProfileGL,const irr::c8* VSProfileDX,
    const irr::c8* PSProgram,const irr::c8* PSEntryPointName,const irr::c8* PSProfileGL,const irr::c8* PSProfileDX,
    ICgShaderConstantSetCallBack* callback,irr::video::E_MATERIAL_TYPE baseMaterial,const char ** args )
    {
        if(!CgServices)
        {
            printf("IrrCg Error! Services not found!\n");
            return irr::video::EMT_SOLID;
        }

        CGerror Error;
        ICgMaterialRenderer* mRenderer = new ICgMaterialRenderer(CgServices,CgServices->getDevice()->getVideoDriver()->getMaterialRenderer(baseMaterial),callback, false);
        irr::s32 ICgMaterialType = CgServices->getDevice()->getVideoDriver()->addMaterialRenderer(mRenderer);

        if(VSProgram)
        {
            FILE* file = fopen(VSProgram, "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Vertex Shader:\nFile \"%s\" not found.\n", VSProgram);

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->VertexProfile,VSProfileGL,VSProfileDX);
            //cout << "IrrCg Info: Vertex Profile is \"" << cgGetProfileString(mRenderer->VertexProfile) << "\"\n";

            mRenderer->VertexProgram = cgCreateProgram(CgServices->getContext(), program_type, VSProgram, mRenderer->VertexProfile, VSEntryPointName, args);

            if (mRenderer->VertexProgram == NULL)
            {
                printf("IrrCg Error! Vertex Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }
            else
            {
                CgServices->LoadProgram(mRenderer->VertexProgram);
                ProgramsList.push_back(&mRenderer->VertexProgram);
            }
        }

        if(PSProgram)
        {
            FILE* file = fopen(PSProgram, "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Pixel Shader:\nFile \"%s\" not found.\n", PSProgram);

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->PixelProfile,PSProfileGL,PSProfileDX);
            //cout << "IrrCg Info: Pixel Profile is \"" << cgGetProfileString(mRenderer->PixelProfile) << "\"\n";

            mRenderer->PixelProgram = cgCreateProgram(CgServices->getContext(), program_type, PSProgram, mRenderer->PixelProfile, PSEntryPointName, args);

            if (mRenderer->PixelProgram == NULL)
            {
                printf("IrrCg Error! Pixel Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }
            else
            {
                CgServices->LoadProgram(mRenderer->PixelProgram);
                ProgramsList.push_back(&mRenderer->PixelProgram);
            }
        }

        mRenderer->drop();

        return ICgMaterialType;
    }

    // Function useful for add Cg Shader Material from files to Your program.
    irr::s32 ICgProgrammingServices::addCgShaderMaterialFromFiles( CGenum program_type,
    const irr::c8* VSProgram,const irr::c8* VSEntryPointName,const irr::c8* VSProfileGL,const irr::c8* VSProfileDX,
    const irr::c8* PSProgram,const irr::c8* PSEntryPointName,const irr::c8* PSProfileGL,const irr::c8* PSProfileDX,
    ICgShaderConstantSetCallBack* callback,irr::video::E_MATERIAL_TYPE baseMaterial,const char ** args )
    {
        if(!CgServices)
        {
            printf("IrrCg Error! Services not found!\n");
            return irr::video::EMT_SOLID;
        }

        CGerror Error;
        ICgMaterialRenderer* mRenderer = new ICgMaterialRenderer(CgServices,CgServices->getDevice()->getVideoDriver()->getMaterialRenderer(baseMaterial),callback, false);
        irr::s32 ICgMaterialType = CgServices->getDevice()->getVideoDriver()->addMaterialRenderer(mRenderer);

        if(VSProgram)
        {
            FILE* file = fopen(VSProgram, "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Vertex Shader:\nFile \"%s\" not found.\n", VSProgram);

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->VertexProfile,VSProfileGL,VSProfileDX);
            //cout << "IrrCg Info: Vertex Profile is \"" << cgGetProfileString(mRenderer->VertexProfile) << "\"\n";

            mRenderer->VertexProgram = cgCreateProgramFromFile(CgServices->getContext(), program_type, VSProgram, mRenderer->VertexProfile, VSEntryPointName, args);

            if (mRenderer->VertexProgram == NULL)
            {
                printf("IrrCg Error! Vertex Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }
            else
            {
                CgServices->LoadProgram(mRenderer->VertexProgram);
                ProgramsList.push_back(&mRenderer->VertexProgram);
            }
        }

        if(PSProgram)
        {
            FILE* file = fopen(PSProgram, "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Pixel Shader:\nFile \"%s\" not found.\n", PSProgram);

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            CgServices->getProfile(mRenderer->PixelProfile,PSProfileGL,PSProfileDX);
            //cout << "IrrCg Info: Pixel Profile is \"" << cgGetProfileString(mRenderer->PixelProfile) << "\"\n";

            mRenderer->PixelProgram = cgCreateProgramFromFile(CgServices->getContext(), program_type, PSProgram, mRenderer->PixelProfile, PSEntryPointName, args);

            if (mRenderer->PixelProgram == NULL)
            {
                printf("IrrCg Error! Pixel Shader:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }
            else
            {
                CgServices->LoadProgram(mRenderer->PixelProgram);
                ProgramsList.push_back(&mRenderer->PixelProgram);
            }
        }

        mRenderer->drop();

        return ICgMaterialType;
    }

    // Function useful for add Cg Shader Material from Material file to Your program.
    irr::s32 ICgProgrammingServices::addCgShaderMaterialFromMaterialFile( const irr::c8* Material,ICgMaterialConstantSetCallBack* &mCallback, ICgShaderConstantSetCallBack* Callback, const char ** args )
    {
        if(!CgServices)
        {
            printf("IrrCg Error! Services not found!\n");
            return irr::video::EMT_SOLID;
        }

        if(mCallback)
        delete mCallback;

        mCallback = new ICgMaterialConstantSetCallBack(Callback);
        ICgMaterialRenderer* mRenderer = 0;

        ICgMaterialLoader MaterialLoader(CgServices);

        if(!MaterialLoader.loadMaterial(Material, mCallback, mRenderer, args))
        return irr::video::EMT_SOLID;
        else
        {
            ProgramsList.push_back(&mRenderer->VertexProgram);
            ProgramsList.push_back(&mRenderer->PixelProgram);
        }

        s32 ICgMaterialType = CgServices->getDevice()->getVideoDriver()->addMaterialRenderer(mRenderer);
        mRenderer->drop();

        return ICgMaterialType;
    }

    // Function useful for add CgFX effect from file to Your program.
    irr::s32 ICgProgrammingServices::addCgEffectFromFiles( const irr::c8* EffectProgram,
    ICgShaderConstantSetCallBack* callback,irr::video::E_MATERIAL_TYPE baseMaterial, const char ** args )
    {
        if(!CgServices)
        {
            printf("IrrCg Error! Services not found!\n");
            return irr::video::EMT_SOLID;
        }

        CGerror Error;
        ICgMaterialRenderer* mRenderer = new ICgMaterialRenderer(CgServices,CgServices->getDevice()->getVideoDriver()->getMaterialRenderer(baseMaterial),callback, true);
        irr::s32 ICgMaterialType = CgServices->getDevice()->getVideoDriver()->addMaterialRenderer(mRenderer);

        if(EffectProgram)
        {
            FILE* file = fopen(EffectProgram, "rt");

            if(file)
            fclose(file);
            else
            {
                printf("IrrCg Error! Effect:\nFile \"%s\" not found.\n", EffectProgram);

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }

            mRenderer->Effect = cgCreateEffectFromFile(CgServices->getContext(), EffectProgram, args);

            if (mRenderer->Effect == NULL)
            {
                printf("IrrCg Error! Effect:\n");
                Error = cgGetError();
                std::cout << cgGetErrorString(Error) << std::endl;
                std::cout << cgGetLastListing(CgServices->getContext()) << std::endl;

                mRenderer->drop();

                return irr::video::EMT_SOLID;
            }
            else
            {
                EffectsList.push_back(&mRenderer->Effect);

                mRenderer->Technique = cgGetFirstTechnique(mRenderer->Effect);

                while (mRenderer->Technique && cgValidateTechnique(mRenderer->Technique) == CG_FALSE)
                mRenderer->Technique = cgGetNextTechnique(mRenderer->Technique);

                if(!mRenderer->Technique)
                {
                    printf("IrrCg Error! No valid technique in effect: \"%s\"\n", EffectProgram);

                    mRenderer->drop();

                    return irr::video::EMT_SOLID;
                }
            }
        }

        mRenderer->drop();

        return ICgMaterialType;
    }

    ICgServices* ICgProgrammingServices::getCgServices()
    {
        return CgServices;
    }

    // End of class ICgProgrammingServices

    // class ICgMaterialRenderer
    ICgMaterialRenderer::ICgMaterialRenderer(ICgServices* vCgServices, irr::video::IMaterialRenderer* vBaseMaterial, ICgShaderConstantSetCallBack* vCallback, bool vIsEffect) :
                        CgServices(vCgServices), BaseMaterial(vBaseMaterial), Callback(vCallback), IsEffect(vIsEffect), RendererServices(0),
                         VertexProgram(0), PixelProgram(0), Effect(0), Technique(0), Pass(0)
    {
    }

    ICgMaterialRenderer::~ICgMaterialRenderer()
    {
    }

    void ICgMaterialRenderer::OnSetMaterial(const irr::video::SMaterial& material, const irr::video::SMaterial& lastMaterial,bool resetAllRenderstates, irr::video::IMaterialRendererServices* services)
    {
        Material = material;
        RendererServices = services;

        if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
        {
            if(!IsEffect)
            {
                if(VertexProgram)
                CgServices->EnableShader(VertexProgram,VertexProfile);
                if(PixelProgram)
                CgServices->EnableShader(PixelProgram,PixelProfile);
            }

            if (BaseMaterial)
			BaseMaterial->OnSetMaterial(material, material, true, services);
        }

        if(Callback)
		Callback->OnSetMaterial(material);

        #if defined (IrrCgOGL) && (IRRLICHT_VERSION_MINOR < 7)
        if(CgServices->getType() == ICGT_OPENGL)
        for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
        {
            COpenGLDriver* VideoDriver = (COpenGLDriver*)CgServices->getDevice()->getVideoDriver();
            VideoDriver->setActiveTexture(i, material.getTexture(i));
        }
		#endif

        services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
    }

    bool ICgMaterialRenderer::OnRender(irr::video::IMaterialRendererServices* service, irr::video::E_VERTEX_TYPE vtxtype)
    {
        if(Callback)
        Callback->OnSetConstants(CgServices,Effect,Pass,VertexProgram,PixelProgram,Material);

        if(Effect)
        cgSetPassState(Pass);

        return true;
    };

    void ICgMaterialRenderer::OnUnsetMaterial()
    {
        if(!IsEffect)
        {
            if(VertexProgram)
            CgServices->DisableShader(VertexProfile);
            if(PixelProgram)
            CgServices->DisableShader(PixelProfile);
        }
        else
        {
            if(Effect)
            {
                RendererServices->setBasicRenderStates(Material, Material, true);
                CgServices->DisableEffect();
            }
        }

        if(BaseMaterial)
        BaseMaterial->OnUnsetMaterial();
    }

    bool ICgMaterialRenderer::isTransparent() const
    {
        return BaseMaterial ? BaseMaterial->isTransparent() : false;
    }

    // End of class ICgMaterialRenderer
}
