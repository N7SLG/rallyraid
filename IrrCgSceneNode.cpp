// Copyright (C) 2007-2009 Patryk Nadrowski
// This file is part of the "irrCg".
// For conditions of distribution and use, see copyright notice in License.txt

#ifndef IRR_CG_7

#include "irrlicht.h"

#if IRRLICHT_VERSION_MINOR < 7
namespace irr { namespace scene {
//! Typedef for list of scene nodes
typedef irr::core::list<irr::scene::ISceneNode*> ISceneNodeList;
//! Typedef for list of scene node animators
typedef irr::core::list<irr::scene::ISceneNodeAnimator*> ISceneNodeAnimatorList;
} }
#endif // IRRLICHT_VERSION_MINOR < 7

#include <Cg/cg.h>

#include "IrrCg.h"
#include "IrrCgSceneNode.h"

namespace IrrCg
{
    ICgSceneNode::ICgSceneNode(irr::scene::ISceneNode* vNode, irr::scene::ISceneNode* vParent, irr::scene::ISceneManager* vSMGR, irr::s32 vID) :
                irr::scene::ISceneNode(vParent, vSMGR, vID), Node(0), Active(1)
    {
        setNode(vNode);

        // We set culling here, because current Irrlicht ver (1.7) doesn't support getAutomaticCulling function as virtual.
        setAutomaticCulling(vNode->getAutomaticCulling());
    }

    void ICgSceneNode::OnRegisterSceneNode()
    {
        if(IsVisible)
        {
            irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

            int transparentCount = 0;
            int solidCount = 0;

            // Count transparent and solid materials in this scene node.
            for (irr::u32 i=0; i<Node->getMaterialCount(); ++i)
            {
                irr::video::IMaterialRenderer* rnd = driver->getMaterialRenderer(Node->getMaterial(i).MaterialType);

                if (rnd && rnd->isTransparent())
                    ++transparentCount;
                else
                    ++solidCount;

                if (solidCount && transparentCount)
                    break;
            }

            // Register according to material types counted.

            if (solidCount)
                SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);

            if (transparentCount)
                SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_TRANSPARENT);

            // Childrens.
            irr::scene::ISceneNodeList::Iterator it = Children.begin();
            for (; it != Children.end(); ++it)
            {
                if((*it) != Node)
                (*it)->OnRegisterSceneNode();
            }
        }
    }

    void ICgSceneNode::OnAnimate(irr::u32 timeMs)
    {
        if (IsVisible)
        {
            // Animators
            irr::scene::ISceneNodeAnimatorList::Iterator ait = Animators.begin();
            while (ait != Animators.end())
            {
                irr::scene::ISceneNodeAnimator* anim = *ait;
                ++ait;
                anim->animateNode(this, timeMs);
            }

            // Reset current matrix.
            AbsoluteTransformation.makeIdentity();

            // Process assigned Node.
            Node->OnAnimate(timeMs);

            // Update absolute position.
            updateAbsolutePosition();

            // Childrens.
            irr::scene::ISceneNodeList::Iterator it = Children.begin();
            for (; it != Children.end(); ++it)
                if((*it) != Node)
                (*it)->OnAnimate(timeMs);
        }
    }

    void ICgSceneNode::render()
    {
        if(Active)
        {
            irr::video::IVideoDriver* Driver = SceneManager->getVideoDriver();

            bool NeedPass = 0;

            for(int c = 0; c < MaterialIDsList.size(); ++c)
            {
                ICgMaterialRenderer* mRenderer = (ICgMaterialRenderer*)Driver->getMaterialRenderer(MaterialIDsList[c]);
                mRenderer->Pass = cgGetFirstPass(mRenderer->Technique);

                if(mRenderer->Pass)
                NeedPass = 1;
            }

            while(NeedPass)
            {
                Node->render();

                NeedPass = 1; // Need the same count of passes, because Irrlicht doesn't support disable material renderer function etc.

                for(irr::u32 c = 0; c < MaterialIDsList.size() && NeedPass; ++c)
                {
                    ICgMaterialRenderer* mRenderer = (ICgMaterialRenderer*)Driver->getMaterialRenderer(MaterialIDsList[c]);

                    if(mRenderer->Pass)
                    {
                        cgResetPassState(mRenderer->Pass);
                        mRenderer->Pass = cgGetNextPass(mRenderer->Pass);
                    }

                    if(!mRenderer->Pass)
                    NeedPass = 0;
                }
            }
        }
        else
        Node->render();
    }

    // it isn't virtual in current Irrlicht ver (1.7), so we need manualy set culling or leave state from constructor process.
    /*irr::scene::E_CULLING_TYPE*/irr::u32 ICgSceneNode::getAutomaticCulling() const
    {
        return Node->getAutomaticCulling();
    }

    const irr::core::aabbox3d<irr::f32>& ICgSceneNode::getBoundingBox() const
    {
        return Node->getBoundingBox();
    }

    irr::core::matrix4 ICgSceneNode::getRelativeTransformation() const
    {
        return Node->getRelativeTransformation();
    }

    irr::scene::ISceneNode* ICgSceneNode::getNode()
    {
        return Node;
    }

    void ICgSceneNode::setNode(irr::scene::ISceneNode* vNode, bool vRequestCheckMaterials)
    {
        if(Node)
        removeChild(Node);

        addChild(vNode);
        Node = vNode;

        if(vRequestCheckMaterials)
        checkMaterials();

        Active = 1;
    }

    void ICgSceneNode::checkMaterials()
    {
        MaterialIDsList.clear();

        irr::video::IVideoDriver* Driver = SceneManager->getVideoDriver();

        for(irr::u32 c = 0; c < Node->getMaterialCount(); ++c)
        {
            irr::video::IMaterialRenderer* tRenderer = Driver->getMaterialRenderer(Node->getMaterial(c).MaterialType);
            ICgMaterialRenderer* mRenderer = dynamic_cast<ICgMaterialRenderer*> (tRenderer);

            if(mRenderer)
            {
                bool Passed = 1;

                for(int n = 0; n < MaterialIDsList.size() && Passed; ++n)
                {
                    if(MaterialIDsList[n] == Node->getMaterial(c).MaterialType)
                    Passed = 0;
                }

                if(Passed)
                MaterialIDsList.push_back(Node->getMaterial(c).MaterialType);
            }
        }
    }

    void ICgSceneNode::setActive(bool vActive)
    {
        Active = vActive;
    }
}
#endif // ! IRR_CG_7
