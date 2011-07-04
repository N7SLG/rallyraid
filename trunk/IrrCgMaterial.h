// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

#ifndef __IRR_CG_MATERIAL_H_INCLUDED__
#define __IRR_CG_MATERIAL_H_INCLUDED__

namespace IrrCg
{
    //! ICgServices Type enumeration.
    enum IRR_CG_TYPE { ICGT_EMPTY = 0, ICGT_OPENGL, ICGT_DIRECT3D9 };

    //! Matrix Type enumeration for Matrix operations.
    enum IRR_CG_TRANSFORM { ICGT_MATRIX_IDENTITY = 0, ICGT_MATRIX_TRANSPOSE, ICGT_MATRIX_INVERSE, ICGT_MATRIX_INVERSE_TRANSPOSE };

    class ICgServices;
    class ICgMaterialConstantSetCallBack;
    class ICgMaterialRenderer;

    //! Prototype parameter in Cg Material file.
    class ICgMaterialParameter
    {
    public:
        //! Set this value.
        virtual void setValue() = 0;
    };

    //! Texture in Cg Material file.
    class ICgMaterialTexture
    {
    public:
        //! Constructor.
        ICgMaterialTexture(ICgServices* services, CGprogram prog, const char* name, int texid);

        //! Destructor.
        ~ICgMaterialTexture(){};

        //! Set this value.
        virtual void setValue(const irr::video::SMaterial& Material);

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Texture ID.
        int TextureID;
    };

    //! Parameter1d value in Cg Material file.
    class ICgMaterialParameter1d: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter1d(ICgServices* services, CGprogram prog, const char* name, double v);

        //! Destructor.
        ~ICgMaterialParameter1d(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Value.
        double valuev;
    };

    //! Parameter1f value in Cg Material file.
    class ICgMaterialParameter1f: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter1f(ICgServices* services, CGprogram prog, const char* name, float v);

        //! Destructor.
        ~ICgMaterialParameter1f(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Value.
        float valuev;
    };

    //! Parameter1i value in Cg Material file.
    class ICgMaterialParameter1i: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter1i(ICgServices* services, CGprogram prog, const char* name, int v);

        //! Destructor.
        ~ICgMaterialParameter1i(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Value.
        int valuev;
    };

    //! Parameter2d value in Cg Material file.
    class ICgMaterialParameter2d: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter2d(ICgServices* services, CGprogram prog, const char* name, double x, double y);

        //! Destructor.
        ~ICgMaterialParameter2d(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        double valuex;

        //! Y value.
        double valuey;
    };

    //! Parameter2f value in Cg Material file.
    class ICgMaterialParameter2f: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter2f(ICgServices* services, CGprogram prog, const char* name, float x, float y);

        //! Destructor.
        ~ICgMaterialParameter2f(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        float valuex;

        //! Y value.
        float valuey;
    };

    //! Parameter2d value in Cg Material file.
    class ICgMaterialParameter2i: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter2i(ICgServices* services, CGprogram prog, const char* name, int x, int y);

        //! Destructor.
        ~ICgMaterialParameter2i(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        int valuex;

        //! Y value.
        int valuey;
    };

    //! Parameter3d value in Cg Material file.
    class ICgMaterialParameter3d: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter3d(ICgServices* services, CGprogram prog, const char* name, double x, double y, double z);

        //! Destructor.
        ~ICgMaterialParameter3d(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        double valuex;

        //! Y value.
        double valuey;

        //! Z value.
        double valuez;
    };

    //! Parameter3f value in Cg Material file.
    class ICgMaterialParameter3f: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter3f(ICgServices* services, CGprogram prog, const char* name, float x, float y, float z);

        //! Destructor.
        ~ICgMaterialParameter3f(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        float valuex;

        //! Y value.
        float valuey;

        //! Z value.
        float valuez;
    };

    //! Parameter3i value in Cg Material file.
    class ICgMaterialParameter3i: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter3i(ICgServices* services, CGprogram prog, const char* name, int x, int y, int z);

        //! Destructor.
        ~ICgMaterialParameter3i(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        int valuex;

        //! Y value.
        int valuey;

        //! Z value.
        int valuez;
    };

    //! Parameter4d value in Cg Material file.
    class ICgMaterialParameter4d: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter4d(ICgServices* services, CGprogram prog, const char* name, double x, double y, double z, double w);

        //! Destructor.
        ~ICgMaterialParameter4d(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        double valuex;

        //! Y value.
        double valuey;

        //! Z value.
        double valuez;

        //! W value.
        double valuew;
    };

    //! Parameter4f value in Cg Material file.
    class ICgMaterialParameter4f: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter4f(ICgServices* services, CGprogram prog, const char* name, float x, float y, float z, float w);

        //! Destructor.
        ~ICgMaterialParameter4f(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        float valuex;

        //! Y value.
        float valuey;

        //! Z value.
        float valuez;

        //! W value.
        float valuew;
    };

    //! Parameter4i value in Cg Material file.
    class ICgMaterialParameter4i: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameter4i(ICgServices* services, CGprogram prog, const char* name, int x, int y, int z, int w);

        //! Destructor.
        ~ICgMaterialParameter4i(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! X value.
        int valuex;

        //! Y value.
        int valuey;

        //! Z value.
        int valuez;

        //! W value.
        int valuew;
    };

    //! ParameterCameraPosition value in Cg Material file.
    class ICgMaterialParameterCameraPosition: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterCameraPosition(ICgServices* services, CGprogram prog, const char* name);

        //! Destructor.
        ~ICgMaterialParameterCameraPosition(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;
    };

    //! WorldViewProjection matrix in Cg Material file.
    class ICgMaterialParameterWVPMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterWVPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterWVPMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! WorldView matrix in Cg Material file.
    class ICgMaterialParameterWVMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterWVMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterWVMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! World matrix in Cg Material file.
    class ICgMaterialParameterWMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterWMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterWMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! View matrix in Cg Material file.
    class ICgMaterialParameterVMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterVMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterVMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! ViewProjection matrix in Cg Material file.
    class ICgMaterialParameterVPMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterVPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterVPMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! Projection matrix in Cg Material file.
    class ICgMaterialParameterPMatrix: public ICgMaterialParameter
    {
    public:
        //! Constructor.
        ICgMaterialParameterPMatrix(ICgServices* services, CGprogram prog, const char* name, IRR_CG_TRANSFORM trans);

        //! Destructor.
        ~ICgMaterialParameterPMatrix(){};

        //! Set this value.
        virtual void setValue();

    private:
        //! This store pointer to ICgServices.
        ICgServices* CgServices;

        //! Cg Paramater name.
        CGparameter	param;

        //! Cg matrix transform type
        IRR_CG_TRANSFORM Transform;
    };

    //! XML Material loader.
    class ICgMaterialLoader
    {
    public:
        //! Constructor.
        ICgMaterialLoader(ICgServices* vCgServices);

        //! Destructor.
        ~ICgMaterialLoader();

        //! Load material from file.
        bool loadMaterial( const irr::c8* Material,ICgMaterialConstantSetCallBack* mCallback,ICgMaterialRenderer* &mRenderer,const char ** args );

    private:
        //! Path to vertex program.
        irr::core::stringc VSProgram;

        //! Entry point name in vertex program.
        irr::core::stringc VSEntryPointName;

        //! OpenGL profile for vertex program.
        irr::core::stringc VSProfileGL;

        //! Direct3D9 profile for vertex program.
        irr::core::stringc VSProfileDX;

        //! Path to fragment program.
        irr::core::stringc PSProgram;

        //! Entry point name in fragment program.
        irr::core::stringc PSEntryPointName;

        //! OpenGL profile for fragment program.
        irr::core::stringc PSProfileGL;

        //! Direct3D9 profile for fragment program.
        irr::core::stringc PSProfileDX;

        //! Base material type.
        irr::video::E_MATERIAL_TYPE BaseMaterial;

        //! Pointer to ICgServices.
        ICgServices* CgServices;
    };
}

#endif
