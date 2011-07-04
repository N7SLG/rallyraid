#ifndef SCREENQUAD_H
#define SCREENQUAD_H

#include <irrlicht.h>

class ScreenQuad
{
public:
    ScreenQuad(irr::video::IVideoDriver* driver, const irr::core::position2di& pos, const irr::core::dimension2du& size, bool antialiasing = true)
        : driver(driver)
    {
        Material.AntiAliasing = antialiasing;
        Material.Wireframe = false;
        Material.Lighting = false;
        Material.ZWriteEnable = false;
        Material.BackfaceCulling = false;
        Material.ZBuffer = irr::video::ECFN_NEVER/*ECFN_NEVER ECFN_ALWAYS*/;
        Material.setFlag(irr::video::EMF_TEXTURE_WRAP, true);
        //Material.setFlag(EMF_BILINEAR_FILTER, false);
        bool flip_vert = true;
        /*
        if (size == driver->getScreenSize())
        {
            flip_vert = driver->getDriverType() == irr::video::EDT_OPENGL;
        }
        */
        if (flip_vert)
        {
            Vertices[0] = irr::video::S3DVertex(-1.0f,-1.0f,0.0f,0,0,1,irr::video::SColor(0x0),0.0f,0.0f);
            Vertices[1] = irr::video::S3DVertex(-1.0f, 1.0f,0.0f,0,0,1,irr::video::SColor(0x0),0.0f,1.0f);
            Vertices[2] = irr::video::S3DVertex( 1.0f, 1.0f,0.0f,0,0,1,irr::video::SColor(0x0),1.0f,1.0f);
            Vertices[3] = irr::video::S3DVertex( 1.0f,-1.0f,0.0f,0,0,1,irr::video::SColor(0x0),1.0f,0.0f);
        }
        else
        {
            Vertices[0] = irr::video::S3DVertex(-1.0f,-1.0f,0.0f,0,0,1,irr::video::SColor(0x0),0.0f,1.0f);
            Vertices[1] = irr::video::S3DVertex(-1.0f, 1.0f,0.0f,0,0,1,irr::video::SColor(0x0),0.0f,0.0f);
            Vertices[2] = irr::video::S3DVertex( 1.0f, 1.0f,0.0f,0,0,1,irr::video::SColor(0x0),1.0f,0.0f);
            Vertices[3] = irr::video::S3DVertex( 1.0f,-1.0f,0.0f,0,0,1,irr::video::SColor(0x0),1.0f,1.0f);
        }

        origPos[0] = pos;
        origPos[1] = pos; origPos[1].Y += size.Height;
        origPos[2] = pos; origPos[2].Y += size.Height; origPos[2].X += size.Width;
        origPos[3] = pos; origPos[3].X += size.Width;
        centerPos = pos; centerPos.Y += size.Height / 2; centerPos.X += size.Width / 2;
        

        for (unsigned int i = 0; i < 4; i++)
        {
            set2DVertexPos(i, origPos[i]);
        }
    }

    virtual void render()
    {
        irr::u16 indices[6] = {0, 1, 2, 3, 0, 2};
        if (visible)
        {
            driver->setMaterial(Material);
            driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
            driver->drawIndexedTriangleList(&Vertices[0], 4, &indices[0], 2);
            driver->setMaterial(irr::video::SMaterial());
        }
    }

    virtual irr::video::SMaterial& getMaterial()
    {
        return Material;
    }

    void set2DVertexPos(int index, const irr::core::position2d<irr::s32>& newPos)
    {
        irr::core::dimension2d<irr::u32> screenSize = driver->getScreenSize();
        Vertices[index].Pos.X = 2.f * ((float)newPos.X / (float)screenSize.Width) - 1.f;
        Vertices[index].Pos.Y = (1.f - ((float)newPos.Y / (float)screenSize.Height)) * 2.f - 1.f;
    }

    void setVisible(bool p_visible)
    {
        visible = p_visible;
    }

    bool getVisible()
    {
        return visible;
    }

    void rotate(float angle, const irr::core::position2di center)
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            irr::core::vector2di pos = origPos[i];
            pos.rotateBy(angle, center);
            set2DVertexPos(i, pos);
        }
    }

    void rotate(float angle)
    {
        rotate(angle, centerPos);
    }

private:
    irr::video::S3DVertex           Vertices[4];
    irr::video::SMaterial           Material;
    irr::video::IVideoDriver*       driver;
    irr::core::vector2di            origPos[4];
    irr::core::vector2di            centerPos;
    bool                            visible;
};

#endif
