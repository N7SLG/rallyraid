// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

#ifndef __IRR_CG_SCENE_NODE_H_INCLUDED__
#define __IRR_CG_SCENE_NODE_H_INCLUDED__

//! irrCg namespace.
namespace IrrCg
{
    //! Special IrrCg class for support multipass effects.
    class ICgSceneNode : public irr::scene::ISceneNode
    {
    public:
        //! Constructor.
        ICgSceneNode(irr::scene::ISceneNode* vNode, irr::scene::ISceneNode* vParent, irr::scene::ISceneManager* vSMGR, irr::s32 vID);

        //! Similar to standart Irrlicht function.
        virtual void OnRegisterSceneNode();

        //! Similar to standart Irrlicht function.
		virtual void OnAnimate(irr::u32 timeMs);

        //! render assigned Scene Node culling in passes.
		virtual void render();

        //! Return assigned Scene Node culling.
		virtual /*irr::scene::E_CULLING_TYPE*/irr::u32 getAutomaticCulling() const;

        //! Return assigned Scene Node bounding box.
		virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        //! Return assigned Scene Node relative transformation.
		virtual irr::core::matrix4 getRelativeTransformation() const;

        //! Return assigned Scene Node.
        virtual irr::scene::ISceneNode* getNode();

        //! Assigne new Scene Node.
        virtual void setNode(irr::scene::ISceneNode* vNode, bool vRequestCheckMaterials = true);

        //! Check for properly materials.
        virtual void checkMaterials();

        //! Activate or Deactiveate rendering with effect, usefull eg. when we need draw node in depth pass with other shader.
        virtual void setActive(bool vActive);

    private:

        //! Assigned Scene Node.
        irr::scene::ISceneNode* Node;

        //! Materials list with Cg shader.
        irr::core::array<irr::s32> MaterialIDsList;

        //! Status (Enable or Disable).
        bool Active;
    };
};

#endif
