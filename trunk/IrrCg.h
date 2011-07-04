// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

#ifndef __IRR_CG_H_INCLUDED__
#define __IRR_CG_H_INCLUDED__

#include <iostream>
#include <vector>

#include "IrrCgMaterial.h"
#include "IrrCgSceneNode.h"

/*! \mainpage irrCg 0.7 documentation
 *
 * \section intro Description
 *
 * Welcome to the irrCg documentation.
 * Here you'll find any information you'll need to develop applications with
 * the irrCg.
 *
 * irrCg provides quick and easy integration shaders writed
 * in Cg shading language with Irrlicht Engine.
 *
 * If you have any questions or
 * suggestions, just send an email to the author of this wrapper,
 * Patryk Nadrowski (nadro-linux (at) wp.pl).
 */

//! irrCg namespace.
namespace IrrCg
{
    //! IrrCg Extension Handler class.
    class IrrCgExtensionHandler;

    //! Default class with empty services use in Cg programs in Irrlicht.
    class ICgServices
    {
    public:
        //! Destructor.
        virtual ~ICgServices() {};

        //! Registers graphics pass states for CgFX files.
        virtual void registerStates() = 0;

        //! Get the manage texture parameters flag.
        virtual bool getManageTextureParameters() = 0;

        //! Set the manage texture parameters flag.
        virtual void setManageTextureParameters( bool flag ) = 0;

        //! Determine if a profile is supported.
        virtual bool isProfileSupported( const char * profile_string ) = 0;

        //! Get the profile enumerant from a OpenGL and Direct3D profiles name.
        virtual void getProfile( CGprofile &profile, const char * gl_string, const char * d3d_string ) = 0;

        //! Prepares a program for binding.
        virtual void LoadProgram( CGprogram program,bool paramShadowing = 0 ) = 0;

        //! Enable shader with profile.
        virtual void EnableShader( CGprogram program,CGprofile profile ) = 0;

        //! Disable shader.
        virtual void DisableShader( CGprofile profile ) = 0;

        //! Disable effect.
        virtual void DisableEffect() = 0;

        //! Enable texture for shader.
        virtual void EnableTexture( CGparameter param,irr::video::ITexture* Tex2D ) = 0;
        virtual void EnableTextureSM( CGparameter param,irr::video::ITexture* Tex2D ) = 0;

        //! Set texture for shader.
        virtual void setTexture( CGparameter param,const irr::video::SMaterialLayer &TextureLayer,int Stage ) = 0;

        //! Set 1 double parameter for shader.
        virtual void setParameter1d( CGparameter param,double v ) = 0;

        //! Set 1 float parameter for shader.
        virtual void setParameter1f( CGparameter param,float v ) = 0;

        //! Set 1 integer parameter for shader.
        virtual void setParameter1i( CGparameter param,int v ) = 0;

        //! Set 2 double parameters for shader.
        virtual void setParameter2d( CGparameter param,double x,double y ) = 0;

        //! Set 2 float parameters for shader.
        virtual void setParameter2f( CGparameter param,float x,float y ) = 0;

        //! Set 2 integer parameters for shader.
        virtual void setParameter2i( CGparameter param,int x,int y ) = 0;

        //! Set 3 double parameters for shader.
        virtual void setParameter3d( CGparameter param,double x,double y,double z ) = 0;

        //! Set 3 float parameters for shader.
        virtual void setParameter3f( CGparameter param,float x,float y,float z ) = 0;

        //! Set 3 integer parameters for shader.
        virtual void setParameter3i( CGparameter param,int x,int y,int z ) = 0;

        //! Set 4 double parameters for shader.
        virtual void setParameter4d( CGparameter param,double x,double y,double z,double w ) = 0;

        //! Set 4 float parameters for shader.
        virtual void setParameter4f( CGparameter param,float x,float y,float z,float w ) = 0;

        //! Set 4 integer parameters for shader.
        virtual void setParameter4i( CGparameter param,int x,int y,int z,int w ) = 0;

        //! Set World View Projection Matrix with Transformation and Node informations.
        virtual void setWorldViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set World View Matrix with Transformation and Node informations.
        virtual void setWorldViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set World Matrix with Transformation and Node informations.
        virtual void setWorldMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set View Matrix with Transformation and Node informations.
        virtual void setViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set View Projection Matrix with Transformation and Node informations.
        virtual void setViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set Projection Matrix with Transformation and Node informations.
        virtual void setProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans ) = 0;

        //! Set Your custom Matrix with Transformation.
        virtual void setMatrix( CGparameter param,IRR_CG_TRANSFORM trans,const irr::core::matrix4& mat ) = 0;

        //! Get pointer to currently use Irrlicht Device.
        virtual irr::IrrlichtDevice* getDevice() = 0;

        //! Get Cg Context.
        virtual CGcontext getContext() = 0;

        //! Get Services Type.
        virtual IRR_CG_TYPE getType() = 0;
    };

    //! Like standard ICgServices, but empty services are replece by CgGL services.
    class ICgOGLServices: public ICgServices
    {
    public:
        ICgOGLServices(irr::IrrlichtDevice* vDevice);
        ~ICgOGLServices();

        void registerStates();
        bool getManageTextureParameters();
        void setManageTextureParameters( bool flag );

        bool isProfileSupported( const char * profile_string );
        void getProfile( CGprofile &profile, const char * gl_string, const char * d3d_string );
        void LoadProgram( CGprogram program,bool paramShadowing = 0 );
        void EnableShader( CGprogram program,CGprofile profile );
        void DisableShader( CGprofile profile );
        void DisableEffect();

        // Textures Functions.
        void EnableTexture( CGparameter param,irr::video::ITexture* Tex2D );
        void EnableTextureSM( CGparameter param,irr::video::ITexture* Tex2D );
        void setTexture( CGparameter param,const irr::video::SMaterialLayer &TextureLayer,int Stage );

        // setParameter Functions.
        void setParameter1d( CGparameter param,double v );
        void setParameter1f( CGparameter param,float v );
        void setParameter1i( CGparameter param,int v );
        void setParameter2d( CGparameter param,double x,double y );
        void setParameter2f( CGparameter param,float x,float y );
        void setParameter2i( CGparameter param,int x,int y );
        void setParameter3d( CGparameter param,double x,double y,double z );
        void setParameter3f( CGparameter param,float x,float y,float z );
        void setParameter3i( CGparameter param,int x,int y,int z );
        void setParameter4d( CGparameter param,double x,double y,double z,double w );
        void setParameter4f( CGparameter param,float x,float y,float z,float w );
        void setParameter4i( CGparameter param,int x,int y,int z,int w );

        // Matrix Functions.
        void setWorldViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setWorldViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setWorldMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setMatrix( CGparameter param,IRR_CG_TRANSFORM trans,const irr::core::matrix4& mat );

        // Core Functions.
        irr::IrrlichtDevice* getDevice();
        CGcontext getContext();
        IRR_CG_TYPE getType();

    private:
        IrrCgExtensionHandler* ExtensionHandler;
        irr::IrrlichtDevice* Device;
        CGcontext Context;
        IRR_CG_TYPE Type;
    };

    //! Like standard ICgServices, but empty services are replece by CgD3D9 services.
    class ICgD3D9Services: public ICgServices
    {
    public:
        ICgD3D9Services(irr::IrrlichtDevice* vDevice);
        ~ICgD3D9Services();

        void registerStates();
        bool getManageTextureParameters();
        void setManageTextureParameters( bool flag );

        bool isProfileSupported( const char * profile_string );
        void getProfile( CGprofile &profile, const char * gl_string, const char * d3d_string );
        void LoadProgram( CGprogram program,bool paramShadowing = 0 );
        void EnableShader( CGprogram program,CGprofile profile );
        void DisableShader( CGprofile profile );
        void DisableEffect();

        // Textures Functions.
        void EnableTexture( CGparameter param,irr::video::ITexture* Tex2D );
        void EnableTextureSM( CGparameter param,irr::video::ITexture* Tex2D );
        void setTexture( CGparameter param,const irr::video::SMaterialLayer &TextureLayer,int Stage );

        // setParameter Functions.
        void setParameter1d( CGparameter param,double v );
        void setParameter1f( CGparameter param,float v );
        void setParameter1i( CGparameter param,int v );
        void setParameter2d( CGparameter param,double x,double y );
        void setParameter2f( CGparameter param,float x,float y );
        void setParameter2i( CGparameter param,int x,int y );
        void setParameter3d( CGparameter param,double x,double y,double z );
        void setParameter3f( CGparameter param,float x,float y,float z );
        void setParameter3i( CGparameter param,int x,int y,int z );
        void setParameter4d( CGparameter param,double x,double y,double z,double w );
        void setParameter4f( CGparameter param,float x,float y,float z,float w );
        void setParameter4i( CGparameter param,int x,int y,int z,int w );

        // Matrix Functions.
        void setWorldViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setWorldViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setWorldMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setViewMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setViewProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setProjectionMatrix( CGparameter param,IRR_CG_TRANSFORM trans );
        void setMatrix( CGparameter param,IRR_CG_TRANSFORM trans,const irr::core::matrix4& mat );

        // Core Functions.
        irr::IrrlichtDevice* getDevice();
        CGcontext getContext();
        IRR_CG_TYPE getType();

    private:
        irr::IrrlichtDevice* Device;
        CGcontext Context;
        IRR_CG_TYPE Type;
    };

    //! Like irr::video::IShaderConstantSetCallBack, but for a Cg programs.
    class ICgShaderConstantSetCallBack
    {
    public:
        //! Destructor.
        virtual ~ICgShaderConstantSetCallBack(){};

        //! Like irr::video::IShaderConstantSetCallBack::OnSetMaterial(), but for a Cg shaders.
        virtual void OnSetMaterial( const irr::video::SMaterial& material ) {};

        //! Like irr::video::IShaderConstantSetCallBack::OnSetConstants(), but for a Cg shaders.
        virtual void OnSetConstants( ICgServices* services,const CGeffect& Effect,const CGpass& Pass,const CGprogram& Vertex,const CGprogram& Fragment,const irr::video::SMaterial& Material ) = 0;
    };

    //! Like ICgShaderConstantSetCallBack, but with special OnSetConstants function for a Cg shaders created from material file.
    class ICgMaterialConstantSetCallBack: public ICgShaderConstantSetCallBack
    {
    public:
        //! Constructor.
        ICgMaterialConstantSetCallBack(ICgShaderConstantSetCallBack* callback);

        //! Destructor.
        ~ICgMaterialConstantSetCallBack();

        //! This store all parameters from material file.
        std::vector<ICgMaterialParameter*> Parameter;

        //! This store all textures from material file.
        std::vector<ICgMaterialTexture*> Texture;

        //! Like irr::video::IShaderConstantSetCallBack::OnSetConstants(), but for a Cg shaders created from material file.
        virtual void OnSetConstants( ICgServices* services,const CGeffect& Effect,const CGpass& Pass,const CGprogram& Vertex,const CGprogram& Fragment,const irr::video::SMaterial& Material );

    private:
        //! This store pointer to ICgShaderConstantSetCallBack.
        ICgShaderConstantSetCallBack* CgCallback;
    };

    //! Interface making it possible to create and use Cg shaders.
    class ICgProgrammingServices
    {
    public:
        //! Constructor.
        ICgProgrammingServices(irr::IrrlichtDevice* Device, bool RegisterStates = false, bool ManageTextureParameters = false, CGenum ParameterSettingMode = CG_IMMEDIATE_PARAMETER_SETTING);

        //! Destructor.
        ~ICgProgrammingServices();

        //! Adds a new material renderer to the VideoDriver, based on a C for Graphics shading
        //! language.
        //! \param program_type: Enumerant describing the contents of the program string.
        //! The following enumerants are allowed: CG_SOURCE and CG_OBJECT.
        //! \param VSProgram: String containing the source of the vertex shader program.
        //! This can be 0 if no vertex program shall be used.
        //! \param VSEntryPointName: Name of the entry function of the vertex shader program
        //! \param VSProfileGL: OpenGL vertex shader profile.
        //! \param VSProfileDX: Direct3D vertex shader profile.
        //! \param PSProgram: String containing the source of the pixel shader program.
        //! This can be 0 if no pixel program shall be used.
        //! \param PSEntryPointName: Name of the entry function of the pixel shader program
        //! \param PSProfileGL: OpenGL pixel shader profile.
        //! \param PSProfileDX: Direct3D pixel shader profile.
        //! \param callback: Pointer to an implementation of ICgShaderConstantSetCallBack in which you
        //! can set the needed vertex and pixel shader program constants. Set this to 0 if you don't need this.
        //! \param baseMaterial: Base material which renderstates will be used to shade the
        //! material.
        //! \param args: List with special parameters for compiler.
        //! \return Returns the number of the
        //! material type which can be set in SMaterial::MaterialType to use the renderer.
        //! 0 - Solid Material is returned if an error occured, e.g. if ICgProgrammingServices
        //! wasn't created properly, a vertex or a pixel shader program could not be compiled etc.
        irr::s32 addCgShaderMaterial( CGenum program_type,
        const irr::c8* VSProgram,const irr::c8* VSEntryPointName = "main",const irr::c8* VSProfileGL = "arbvp1",const irr::c8* VSProfileDX = "vs_2_0",
        const irr::c8* PSProgram = 0,const irr::c8* PSEntryPointName = "main",const irr::c8* PSProfileGL = "arbfp1",const irr::c8* PSProfileDX = "ps_2_0",
        ICgShaderConstantSetCallBack* callback = 0,irr::video::E_MATERIAL_TYPE baseMaterial = irr::video::EMT_SOLID, const char ** args = 0 );

        //! Like ICgProgrammingServices::addCgShaderMaterial(), but tries to load the programs from files.
        irr::s32 addCgShaderMaterialFromFiles( CGenum program_type,
        const irr::c8* VSProgram,const irr::c8* VSEntryPointName = "main",const irr::c8* VSProfileGL = "arbvp1",const irr::c8* VSProfileDX = "vs_2_0",
        const irr::c8* PSProgram = 0,const irr::c8* PSEntryPointName = "main",const irr::c8* PSProfileGL = "arbfp1",const irr::c8* PSProfileDX = "ps_2_0",
        ICgShaderConstantSetCallBack* callback = 0,irr::video::E_MATERIAL_TYPE baseMaterial = irr::video::EMT_SOLID, const char ** args = 0 );

        //! Adds a new material renderer to the VideoDriver, based on a C for Graphics shading
        //! language from material file
        //! \param Material: Path for the material file.
        //! \param mCallback: Reference pointer of ICgMaterialConstantSetCallBack. This is fill by values from
        //! material file, so You can easy delete it from memory, if you don't need already any longer.
        //! \param Callback: Pointer to an implementation of ICgShaderConstantSetCallBack in which you
        //! can set the needed vertex and pixel shader program constants (other than in material file).
        //! Set this to 0 if you don't need this.
        //! \param args: List with special parameters for compiler.
        irr::s32 addCgShaderMaterialFromMaterialFile( const irr::c8* Material,ICgMaterialConstantSetCallBack* &mCallback,ICgShaderConstantSetCallBack* Callback = 0,const char ** args = 0 );

        irr::s32 addCgEffectFromFiles( const irr::c8* EffectProgram, ICgShaderConstantSetCallBack* callback = 0, irr::video::E_MATERIAL_TYPE baseMaterial = irr::video::EMT_SOLID, const char ** args = 0 );

        //! Return pointer to ICgServices.
        ICgServices* getCgServices();

    private:
        //! List with Cg programs.
        std::vector<CGprogram*> ProgramsList;

        //! List with Cg effects.
        std::vector<CGeffect*> EffectsList;

        //! This store pointer to ICgServices.
        ICgServices* CgServices;
    };

    //! Like standard Irrlicht IMaterialRenderer, but for a Cg shaders.
    class ICgMaterialRenderer : public irr::video::IMaterialRenderer
    {
    public:
        //! Constructor.
        ICgMaterialRenderer(ICgServices* vCgServices, irr::video::IMaterialRenderer* vBaseMaterial, ICgShaderConstantSetCallBack* vCallback, bool vIsEffect);

        //! Destructor.
        ~ICgMaterialRenderer();

        //! This store a Cg Vertex Program.
        CGprogram VertexProgram;

        //! This store a Cg Pixel Program.
        CGprogram PixelProgram;

        //! This store a Cg Vertex Profile.
        CGprofile VertexProfile;

        //! This store a Cg Pixel Profile.
        CGprofile PixelProfile;

        //! This store an Effect Program.
        CGeffect Effect;

        //! This store a Technique.
        CGtechnique Technique;

        //! This store a current Pass.
        CGpass Pass;

        //! Like IMaterialRenderer::OnSetMaterial(), but for a Cg shaders.
        virtual void OnSetMaterial(const irr::video::SMaterial& material, const irr::video::SMaterial& lastMaterial,bool resetAllRenderstates, irr::video::IMaterialRendererServices* services);

        //! Like IMaterialRenderer::OnRender(), but for a Cg shaders.
        virtual bool OnRender(irr::video::IMaterialRendererServices* service, irr::video::E_VERTEX_TYPE vtxtype);

        //! Like IMaterialRenderer::OnUnsetMaterial(), but for a Cg shaders.
        virtual void OnUnsetMaterial();

        //! Returns if the material is transparent.
        virtual bool isTransparent() const;

    protected:
        //! This store a pointer to Type of standard Irrlicht Material Renderer eg. Solid.
        irr::video::IMaterialRenderer* BaseMaterial;

        //! This store a pointer to MaterialRenderer services.
        irr::video::IMaterialRendererServices* RendererServices;

        //! This store a pointer to ICgShaderConstantSetCallBack.
        ICgShaderConstantSetCallBack* Callback;

        //! This store a Material.
        irr::video::SMaterial Material;

        //! This store a pointer to ICgServices.
        ICgServices* CgServices;

        //! True, if we use CgFX in this renderer.
        bool IsEffect;
    };
}

#endif
