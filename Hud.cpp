
#include "Hud.h"
#include "ScreenQuad.h"
#include "TheGame.h"
#include "TheEarth.h"
#include "ShadersSM20.h"
#include "FontManager.h"
#include "Player.h"
#include "Vehicle.h"
#include "VehicleType.h"
#include "ItinerPoint.h"
#include "WayPointManager.h"
#include "WStringConverter.h"
#include "ShadowRenderer.h"
#include "Settings.h"


// normalize angle between 0 and 360
static float normalizeAngle(float &angle)
{
    while (angle >= 360.f) angle -= 360.f;
    while (angle < 0.f) angle += 360.f;
    return angle;
}

static float normalizeAngle180(float &angle)
{
    while (angle > 180.f) angle -= 360.f;
    while (angle < -180.f) angle += 360.f;
    return angle;
}

#define HUD_PADDING             5

#define COMPASS_SIZE            192 // 128
#define COMPASS_HSIZE           (COMPASS_SIZE / 2)
#define COMPASS_TEXT_SIZE_X     44 // normalbold: 28, largebold: 34, extralargebold: 42
#define COMPASS_TEXT_HSIZE_X    (COMPASS_TEXT_SIZE_X / 2)
#define COMPASS_TEXT_SIZE_Y     26// normalbold: 18, largebold: 22, extralargebold: 26
#define COMPASS_TEXT_HSIZE_Y    (COMPASS_TEXT_SIZE_Y / 2)

#define COMPASS_WP_ARROW_DIFF   30
#define COMPASS_WP_ARROW_HDIFF  (COMPASS_WP_ARROW_DIFF / 2)
#define COMPASS_WP_ARROW_SIZE   (COMPASS_WP_ARROW_DIFF + COMPASS_SIZE)

#define TM_TEXT_X               (122) // 107
#define TM_PART_TEXT_Y          (82)  // 78
#define TM_TOTAL_TEXT_Y         (38)  // 34
#define TM_TEXT_SIZE_X          (80)  // 70
#define TM_TEXT_SIZE_Y          (24)  // 22

#define ROADBOOKBG_SIZE_Y       (128)
#define ROADBOOKBG_SIZE_X       (4*ROADBOOKBG_SIZE_Y)
#define ROADBOOKBG_HSIZE_X      (ROADBOOKBG_SIZE_X / 2)

#define ROADBOOKENTRY_NUM_SIZE_X    (30) // 32
#define ROADBOOKENTRY_NUM_SIZE_Y    (16)
#define ROADBOOKENTRY_GD_SIZE_X     (92) // 96
#define ROADBOOKENTRY_GD_SIZE_Y     (32)
#define ROADBOOKENTRY_LD_SIZE_X     (92) // 96
#define ROADBOOKENTRY_LD_SIZE_Y     (24)
#define ROADBOOKENTRY_ITINER_SIZE   (64) // 58
#define ROADBOOKENTRY_ITINER2_SIZE  (48) // 44, 58
#define ROADBOOKENTRY_NOTE_SIZE_X   (54) // 62
#define ROADBOOKENTRY_NOTE_SIZE_Y   (43) // 62, 31

#define ROADBOOKENTRY_NUM_POS_X(num)    (num*ROADBOOKBG_SIZE_Y+0)
#define ROADBOOKENTRY_NUM_POS_Y         (2)
#define ROADBOOKENTRY_GD_POS_X(num)     (num*ROADBOOKBG_SIZE_Y+32)
#define ROADBOOKENTRY_GD_POS_Y          (2)
#define ROADBOOKENTRY_LD_POS_X(num)     (num*ROADBOOKBG_SIZE_Y+5) // +32,+5
#define ROADBOOKENTRY_LD_POS_Y          (28) // 36
#define ROADBOOKENTRY_ITINER_POS_X(num) (num*ROADBOOKBG_SIZE_Y+4) // +3
#define ROADBOOKENTRY_ITINER_POS_Y      (60) // 67
#define ROADBOOKENTRY_ITINER2_POS_X(num) (num*ROADBOOKBG_SIZE_Y+75) // +67
#define ROADBOOKENTRY_ITINER2_POS_Y     (59) // 67
#define ROADBOOKENTRY_NOTE_POS_X(num)   (num*ROADBOOKBG_SIZE_Y+73) // +64
#define ROADBOOKENTRY_NOTE_POS_Y        (56+28) // (64+32)

#define SPEEDOMETER_SIZE                (192)
#define SPEEDOMETER_HSIZE               (SPEEDOMETER_SIZE/2)
#define SPEEDOMETER_STICK_DIFF          (80)
#define SPEEDOMETER_STICK_HDIFF         (SPEEDOMETER_STICK_DIFF/2)
#define SPEEDOMETER_STICK_SIZE          (SPEEDOMETER_SIZE-SPEEDOMETER_STICK_DIFF)
#define SPEEDOMETER_STICK_HSIZE         (SPEEDOMETER_STICK_SIZE/2)
#define SPEEDOMETER_RPM_STICK_DIFF      (140)
#define SPEEDOMETER_RPM_STICK_HDIFF     (SPEEDOMETER_RPM_STICK_DIFF/2)
#define SPEEDOMETER_RPM_STICK_SIZE      (SPEEDOMETER_SIZE-SPEEDOMETER_RPM_STICK_DIFF)
#define SPEEDOMETER_RPM_STICK_HSIZE     (SPEEDOMETER_RPM_STICK_SIZE/2)

#define SPEEDOMETER_POS_X               (HUD_PADDING)
#define SPEEDOMETER_POS_Y               (TheGame::getInstance()->getDriver()->getScreenSize().Height-(HUD_PADDING*4)-(4*28)-SPEEDOMETER_SIZE) // The time and speed text
#define SPEEDOMETER_STICK_POS_X         (SPEEDOMETER_POS_X+SPEEDOMETER_STICK_HDIFF)
#define SPEEDOMETER_STICK_POS_Y         (SPEEDOMETER_POS_Y+SPEEDOMETER_STICK_HDIFF)
#define SPEEDOMETER_RPM_STICK_POS_X     (SPEEDOMETER_POS_X+SPEEDOMETER_RPM_STICK_HDIFF)
#define SPEEDOMETER_RPM_STICK_POS_Y     (SPEEDOMETER_POS_Y+((479*SPEEDOMETER_SIZE)/512)-SPEEDOMETER_RPM_STICK_HSIZE)

#define GEAR_TEXT_SIZE                  ((64*SPEEDOMETER_SIZE)/512)
#define GEAR_TEXT_POS_X                 (SPEEDOMETER_POS_X+((226*SPEEDOMETER_SIZE)/512))
#define GEAR_TEXT_POS_Y                 (SPEEDOMETER_POS_Y+((308*SPEEDOMETER_SIZE)/512))


Hud* Hud::hud = 0;

void Hud::initialize()
{
    if (hud == 0)
    {
        hud = new Hud();
    }
}

void Hud::finalize()
{
    if (hud)
    {
        delete hud;
        hud = 0;
    }
}
    

Hud::Hud()
    : visible(true),
      miniMapQuad(0),
      compassQuad(0),
      compassWPQuad(0),
      tripMasterQuad(0),
      roadBookBGQuad(0),
      roadBookBGOQuad(0),
      speedometerQuad(0),
      stickQuad(0),
      rpmStickQuad(0),
      compassText(0),
      tmPartText(0),
      tmTotalText(0),
      speedText(0),
      stageTimeText(0),
      gearText(0),
      editorText(0)
{
    miniMapQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(HUD_PADDING, TheGame::getInstance()->getDriver()->getScreenSize().Height - MINIMAP_SIZE - (5*HUD_PADDING) - (4*28) - SPEEDOMETER_SIZE),
        irr::core::dimension2du(MINIMAP_SIZE, MINIMAP_SIZE), false);
//        irr::core::position2di(HUD_PADDING, TheGame::getInstance()->getDriver()->getScreenSize().Height - MINIMAP_SIZE*3 - (3*HUD_PADDING) - (2*25)),
//        irr::core::dimension2du(MINIMAP_SIZE*3, MINIMAP_SIZE*3), false);
    miniMapQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];

    tripMasterQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_SIZE - COMPASS_WP_ARROW_HDIFF - HUD_PADDING,
            TheGame::getInstance()->getDriver()->getScreenSize().Height - COMPASS_HSIZE - HUD_PADDING),
        irr::core::dimension2du(COMPASS_SIZE, COMPASS_HSIZE), false);
    tripMasterQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    //compassQuad->getMaterial().MaterialTypeParam = 0.2f;
    //compassQuad->getMaterial().MaterialTypeParam2 = 0.8f;
    tripMasterQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    tripMasterQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    tripMasterQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    tripMasterQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    tripMasterQuad->getMaterial().UseMipMaps = false;
    tripMasterQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/tripmaster2.png"));

    compassQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_SIZE - COMPASS_WP_ARROW_HDIFF - HUD_PADDING,
            TheGame::getInstance()->getDriver()->getScreenSize().Height - COMPASS_SIZE - COMPASS_HSIZE - COMPASS_WP_ARROW_HDIFF - HUD_PADDING*2),
        irr::core::dimension2du(COMPASS_SIZE, COMPASS_SIZE), false);
    compassQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    //compassQuad->getMaterial().MaterialTypeParam = 0.2f;
    //compassQuad->getMaterial().MaterialTypeParam2 = 0.8f;
    compassQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    compassQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    compassQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    compassQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    compassQuad->getMaterial().UseMipMaps = false;
    compassQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/compass.png"));

    compassText = TheGame::getInstance()->getEnv()->addStaticText(L"0",
        irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_HSIZE - COMPASS_WP_ARROW_HDIFF - HUD_PADDING - COMPASS_TEXT_HSIZE_X,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - COMPASS_HSIZE - COMPASS_HSIZE - COMPASS_WP_ARROW_HDIFF - HUD_PADDING*2 - COMPASS_TEXT_HSIZE_Y),
        irr::core::dimension2di(COMPASS_TEXT_SIZE_X, COMPASS_TEXT_SIZE_Y)),
        false, false, 0, -1, false);
    compassText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_16PX/*SPECIAL16*/));

    compassWPQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_WP_ARROW_SIZE - HUD_PADDING,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - COMPASS_WP_ARROW_SIZE - COMPASS_HSIZE - HUD_PADDING*2),
        irr::core::dimension2du(COMPASS_WP_ARROW_SIZE, COMPASS_WP_ARROW_SIZE), false);
    compassWPQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    //compassWPQuad->getMaterial().MaterialTypeParam = 0.2f;
    //compassWPQuad->getMaterial().MaterialTypeParam2 = 0.8f;
    compassWPQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    compassWPQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    compassWPQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    compassWPQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    compassWPQuad->getMaterial().UseMipMaps = false;
    compassWPQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/compass_wp.png"));

    tmPartText = TheGame::getInstance()->getEnv()->addStaticText(L"000.00",
        irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_WP_ARROW_HDIFF - HUD_PADDING - TM_TEXT_X,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - HUD_PADDING - TM_PART_TEXT_Y),
        irr::core::dimension2di(TM_TEXT_SIZE_X, TM_TEXT_SIZE_Y)),
        false, false, 0, -1, false);
    tmPartText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);
    tmPartText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_14PX));

    tmTotalText = TheGame::getInstance()->getEnv()->addStaticText(L"000.00",
        irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width - COMPASS_WP_ARROW_HDIFF - HUD_PADDING - TM_TEXT_X,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - HUD_PADDING - TM_TOTAL_TEXT_Y),
        irr::core::dimension2di(TM_TEXT_SIZE_X, TM_TEXT_SIZE_Y)),
        false, false, 0, -1, false);
    tmTotalText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);
    tmTotalText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_14PX));


    roadBookBGQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING),
        irr::core::dimension2du(ROADBOOKBG_SIZE_X, ROADBOOKBG_SIZE_Y), false);
    roadBookBGQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    roadBookBGQuad->getMaterial().UseMipMaps = false;
    roadBookBGQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/roadbookbg.png"));

    roadBookBGOQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING),
        irr::core::dimension2du(ROADBOOKBG_SIZE_X, ROADBOOKBG_SIZE_Y), false);
    roadBookBGOQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    roadBookBGOQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    roadBookBGOQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    roadBookBGOQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    roadBookBGOQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    roadBookBGOQuad->getMaterial().UseMipMaps = false;
    roadBookBGOQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/roadbookbg_over.png"));

    for (unsigned int i = 0; i < 4; i++)
    {
/*
            irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X,
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING),
            irr::core::dimension2di(ROADBOOKENTRY_GD_SIZE_X, ROADBOOKENTRY_GD_SIZE_Y)),
*/
        roadBookEntries[i].numText = TheGame::getInstance()->getEnv()->addStaticText(L"0",
            irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_NUM_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_NUM_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_NUM_SIZE_X, ROADBOOKENTRY_NUM_SIZE_Y)),
            false, false, 0, -1, false);
        roadBookEntries[i].numText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_8PX));
        roadBookEntries[i].numText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        if (i!=1) roadBookEntries[i].numText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].globalDistanceText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_GD_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_GD_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_GD_SIZE_X, ROADBOOKENTRY_GD_SIZE_Y)),
            false, false, 0, -1, false);
        roadBookEntries[i].globalDistanceText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_14PX));
        roadBookEntries[i].globalDistanceText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        if (i!=1) roadBookEntries[i].globalDistanceText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].localDistanceText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_LD_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_LD_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_LD_SIZE_X, ROADBOOKENTRY_LD_SIZE_Y)),
            false, false, 0, -1, false);
        roadBookEntries[i].localDistanceText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_12PX));
        //roadBookEntries[i].localDistanceText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        if (i!=1) roadBookEntries[i].localDistanceText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].noteText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_NOTE_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_NOTE_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_NOTE_SIZE_X, ROADBOOKENTRY_NOTE_SIZE_Y)),
            false, true, 0, -1, false);
        roadBookEntries[i].noteText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_8PX));
        roadBookEntries[i].noteText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
        if (i!=1) roadBookEntries[i].noteText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].itinerQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
            irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_ITINER_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_ITINER_POS_Y),
            irr::core::dimension2du(ROADBOOKENTRY_ITINER_SIZE, ROADBOOKENTRY_ITINER_SIZE), false);
        roadBookEntries[i].itinerQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
        //roadBookEntries[i].itinerQuad->getMaterial().UseMipMaps = false;
        roadBookEntries[i].itinerQuad->getMaterial().setTexture(0, 0);

        roadBookEntries[i].itiner2Quad = new ScreenQuad(TheGame::getInstance()->getDriver(),
            irr::core::position2di(TheGame::getInstance()->getDriver()->getScreenSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_ITINER2_POS_X(i),
            TheGame::getInstance()->getDriver()->getScreenSize().Height - ROADBOOKBG_SIZE_Y - HUD_PADDING + ROADBOOKENTRY_ITINER2_POS_Y),
            irr::core::dimension2du(ROADBOOKENTRY_ITINER2_SIZE, ROADBOOKENTRY_ITINER2_SIZE), false);
        roadBookEntries[i].itiner2Quad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().UseMipMaps = false;
        roadBookEntries[i].itiner2Quad->getMaterial().setTexture(0, 0);
    }

    speedText = TheGame::getInstance()->getEnv()->addStaticText(L"000.00",
        irr::core::recti(irr::core::position2di(HUD_PADDING,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - (2*HUD_PADDING) - (3*28)),
        irr::core::dimension2di(380, 3*31 /*28*/)),
        false, true, 0, -1, false);
    speedText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL_BIGFONT/*SPECIAL16*/));
    speedText->setOverrideColor(irr::video::SColor(255, 255, 255, 255));

    stageTimeText = TheGame::getInstance()->getEnv()->addStaticText(L"0:00:00",
        irr::core::recti(irr::core::position2di(HUD_PADDING,
        TheGame::getInstance()->getDriver()->getScreenSize().Height - (3*HUD_PADDING) - (4*28)),
        irr::core::dimension2di(380, 28)),
        false, false, 0, -1, false);
    stageTimeText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL_BIGFONT/*SPECIAL16*/));
    stageTimeText->setOverrideColor(irr::video::SColor(255, 255, 255, 255));

    editorText = TheGame::getInstance()->getEnv()->addStaticText(L"", irr::core::recti(10, 10, 790, 30), false, true, 0, -1, true);
    editorText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_BUILTIN));


    speedometerQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(SPEEDOMETER_POS_X, SPEEDOMETER_POS_Y),
        irr::core::dimension2du(SPEEDOMETER_SIZE, SPEEDOMETER_SIZE), false);
    speedometerQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    speedometerQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    speedometerQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    speedometerQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    speedometerQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    speedometerQuad->getMaterial().UseMipMaps = false;
    speedometerQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/speedometer.png"));

    stickQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(SPEEDOMETER_STICK_POS_X, SPEEDOMETER_STICK_POS_Y),
        irr::core::dimension2du(SPEEDOMETER_STICK_SIZE, SPEEDOMETER_STICK_SIZE), false);
    stickQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    stickQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    stickQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    stickQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    stickQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    stickQuad->getMaterial().UseMipMaps = false;
    stickQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/palca.png"));

    rpmStickQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(SPEEDOMETER_RPM_STICK_POS_X, SPEEDOMETER_RPM_STICK_POS_Y),
        irr::core::dimension2du(SPEEDOMETER_RPM_STICK_SIZE, SPEEDOMETER_RPM_STICK_SIZE), false);
    rpmStickQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    rpmStickQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    rpmStickQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    rpmStickQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    rpmStickQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    rpmStickQuad->getMaterial().UseMipMaps = false;
    rpmStickQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/palca_e.png"));

    gearText = TheGame::getInstance()->getEnv()->addStaticText(L"N",
        irr::core::recti(irr::core::position2di(GEAR_TEXT_POS_X, GEAR_TEXT_POS_Y),
        irr::core::dimension2di(GEAR_TEXT_SIZE, GEAR_TEXT_SIZE)),
        false, false, 0, -1, false);
    gearText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_14PX));
    //gearText->setOverrideColor(irr::video::SColor(255, 0, 0, 0));
    gearText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
}

Hud::~Hud()
{
    if (miniMapQuad)
    {
        delete miniMapQuad;
        miniMapQuad = 0;
    }
    
    if (compassQuad)
    {
        delete compassQuad;
        compassQuad = 0;
    }

    if (compassWPQuad)
    {
        delete compassWPQuad;
        compassWPQuad = 0;
    }

    if (tripMasterQuad)
    {
        delete tripMasterQuad;
        tripMasterQuad = 0;
    }

    if (roadBookBGQuad)
    {
        delete roadBookBGQuad;
        roadBookBGQuad = 0;
    }

    if (roadBookBGOQuad)
    {
        delete roadBookBGOQuad;
        roadBookBGOQuad = 0;
    }

    if (speedometerQuad)
    {
        delete speedometerQuad;
        speedometerQuad = 0;
    }

    if (stickQuad)
    {
        delete stickQuad;
        stickQuad = 0;
    }

    if (rpmStickQuad)
    {
        delete rpmStickQuad;
        rpmStickQuad = 0;
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        if (roadBookEntries[i].itinerQuad)
        {
            delete roadBookEntries[i].itinerQuad;
            roadBookEntries[i].itinerQuad = 0;
        }
        if (roadBookEntries[i].itiner2Quad)
        {
            delete roadBookEntries[i].itiner2Quad;
            roadBookEntries[i].itiner2Quad = 0;
        }
    }
}
    
void Hud::setVisible(bool newVisible)
{
    if (visible == newVisible) return;
    
    visible = newVisible;
    
    miniMapQuad->setVisible(Settings::getInstance()->editorMode && visible);
    compassQuad->setVisible(visible);
    compassWPQuad->setVisible(WayPointManager::getInstance()->getShowCompass() && visible);
    tripMasterQuad->setVisible(visible);
    roadBookBGQuad->setVisible(visible);
    roadBookBGOQuad->setVisible(visible);
    speedometerQuad->setVisible(visible);
    stickQuad->setVisible(visible);
    rpmStickQuad->setVisible(visible);

    compassText->setVisible(visible);
    tmPartText->setVisible(visible);
    tmTotalText->setVisible(visible);
    speedText->setVisible(visible);
    stageTimeText->setVisible(visible);
    gearText->setVisible(visible);
    editorText->setVisible(Settings::getInstance()->editorMode && visible);

    updateRoadBook();
}

void Hud::preRender(float p_angle)
{
    if (!visible) return;

    float angle = -p_angle-90.f;
    irr::core::stringw str;

    miniMapQuad->getMaterial().setTexture(0, TheEarth::getInstance()->getMiniMapTexture()/*ShadowRenderer::getInstance()->getShadowMap()*/);
    compassQuad->rotate(angle);

    str = L"";
    unsigned int angleInt = 360 - (unsigned int)normalizeAngle(angle);
    if (angleInt == 360) angleInt = 0;
    if (angleInt < 10)
    {
        str += L"00";
    }
    else
    if (angleInt < 100)
    {
        str += L"0";
    }
    str += angleInt;
    compassText->setText(str.c_str());

    str = L"";
    unsigned int dist = (unsigned int)(Player::getInstance()->getDistance()/1000.f) % 1000;
    unsigned int distp = (unsigned int)(Player::getInstance()->getDistance()/10.f) % 100;
    if (dist < 10)
    {
        str += L"00";
    }
    else
    if (dist < 100)
    {
        str += L"0";
    }
    str += dist;
    str += L".";
    if (distp < 10)
    {
        str += L"0";
    }
    str += distp;
    tmPartText->setText(str.c_str());

    str = L"";
    dist = (unsigned int)(Player::getInstance()->getVehicle()->getDistance()/1000.f) % 1000;
    distp = (unsigned int)(Player::getInstance()->getVehicle()->getDistance()/10.f) % 100;
    if (dist < 10)
    {
        str += L"00";
    }
    else
    if (dist < 100)
    {
        str += L"0";
    }
    str += dist;
    str += L".";
    if (distp < 10)
    {
        str += L"0";
    }
    str += distp;
    tmTotalText->setText(str.c_str());

    str = L"Speed: ";
    int speed = (int)(Player::getInstance()->getVehicleSpeed());
    int gear = (Player::getInstance()->getVehicleGear());
    int rpm = (int)(Player::getInstance()->getVehicle()->getRPM());
    unsigned int damage= Player::getInstance()->getVehicle()->getDamagePercentage();
    if (speed < 10)
    {
        str += L"00";
    }
    else
    if (speed < 100)
    {
        str += L"0";
    }
    str += speed;
    str += L" Km/h\n  RPM: ";
    str += rpm;
    str += L"\nDamage: ";
    str += damage;
    str += L"%";
    speedText->setText(str.c_str());
    
    if (Player::getInstance()->getStageTime() >= 3600 && Player::getInstance()->getStagePenaltyTime() > 0)
    {
        str = L" Time:";
    }
    else
    {
        str = L" Time: ";
    }
    WStringConverter::addTimeToStr(str, Player::getInstance()->getStageTime());
    if (Player::getInstance()->getStagePenaltyTime())
    {
        str += L"+";
        WStringConverter::addTimeToStr(str, Player::getInstance()->getStagePenaltyTime());
    }
    stageTimeText->setText(str.c_str());

    bool showWPCompass = WayPointManager::getInstance()->getShowCompass();
    if (showWPCompass)
    {
        compassWPQuad->rotate(WayPointManager::getInstance()->getAngle()-p_angle/*-90.f*/);
    }
    compassWPQuad->setVisible(showWPCompass);

    stickQuad->rotate(-90.f+Player::getInstance()->getVehicleSpeed());
    rpmStickQuad->rotate(-15.f+((Player::getInstance()->getVehicle()->getRPM()/Player::getInstance()->getVehicle()->getVehicleType()->getMaxRPM())*120.f));

    str = L"";
    if (gear > 0)
    {
        str += gear;
    }
    else
    if (gear < 0)
    {
        str = L"R";
    }
    else
    {
        str = L"N";
    }
    gearText->setText(str.c_str());

}
void Hud::render()
{
    if (!visible) return;
    
    miniMapQuad->render();
    compassWPQuad->render();
    compassQuad->render();
    tripMasterQuad->render();
    roadBookBGQuad->render();
    for (unsigned int i = 0; i < 4; i++)
    {
        roadBookEntries[i].itinerQuad->render();
        roadBookEntries[i].itiner2Quad->render();
    }
    roadBookBGOQuad->render();

    speedometerQuad->render();
    stickQuad->render();
    rpmStickQuad->render();
}

void Hud::updateRoadBook()
{
    ItinerManager::itinerPointList_t::const_iterator it = Player::getInstance()->getCurrItiner();
    ItinerPoint* itinerPoint = 0;
    for (unsigned int i = 0; i < 4; i++)
    {
        itinerPoint = 0;
        if (i == 0 && Player::getInstance()->isPrevItinerValid())
        {
            itinerPoint = *(Player::getInstance()->getPrevItiner());
        }
        else if (i == 1 && Player::getInstance()->isCurrItinerValid())
        {
            itinerPoint = *(Player::getInstance()->getCurrItiner());
            it++;
        }
        else if (i!=0 && i!=1 && Player::getInstance()->isItinerValid(it))
        {
            itinerPoint = *it;
            it++;
        }

        if (itinerPoint)
        {
            irr::core::stringw str;
            int dist;
            int distp;

            str = L"";
            str += itinerPoint->getNum();
            roadBookEntries[i].numText->setText(str.c_str());
            roadBookEntries[i].numText->setVisible(visible);

            str = L"";
            dist = (int)(itinerPoint->getGlobalDistance()/1000.f) % 1000;
            distp = (int)(itinerPoint->getGlobalDistance()/10.f) % 100;
            str += (int)dist;
            str += L",";
            if (distp < 10)
            {
                str += L"0";
            }
            str += distp;
            roadBookEntries[i].globalDistanceText->setText(str.c_str());
            roadBookEntries[i].globalDistanceText->setVisible(visible);

            str = L"";
            dist = (int)(itinerPoint->getLocalDistance()/1000.f) % 1000;
            distp = (int)(itinerPoint->getLocalDistance()/10.f) % 100;
            str += (int)dist;
            str += L",";
            if (distp < 10)
            {
                str += L"0";
            }
            str += distp;
            roadBookEntries[i].localDistanceText->setText(str.c_str());
            roadBookEntries[i].localDistanceText->setVisible(visible);

            str = L"";
            str += itinerPoint->getDescription().c_str();
            roadBookEntries[i].noteText->setText(str.c_str());
            roadBookEntries[i].noteText->setVisible(visible);

            //printf("%u. itiner: %p, num: %u, image: %p, image2: %p\n", i, itinerPoint, itinerPoint->getNum(), itinerPoint->getItinerImage(), itinerPoint->getItinerImage2());

            if (itinerPoint->getItinerImage())
            {
                roadBookEntries[i].itinerQuad->getMaterial().setTexture(0, itinerPoint->getItinerImage());
                roadBookEntries[i].itinerQuad->setVisible(visible);
            }
            else
            {
                roadBookEntries[i].itinerQuad->setVisible(false);
            }

            if (itinerPoint->getItinerImage2())
            {
                roadBookEntries[i].itiner2Quad->getMaterial().setTexture(0, itinerPoint->getItinerImage2());
                roadBookEntries[i].itiner2Quad->setVisible(visible);
            }
            else
            {
                roadBookEntries[i].itiner2Quad->setVisible(false);
            }
        }
        else
        {
            roadBookEntries[i].numText->setVisible(false);
            roadBookEntries[i].globalDistanceText->setVisible(false);
            roadBookEntries[i].localDistanceText->setVisible(false);
            roadBookEntries[i].noteText->setVisible(false);
            roadBookEntries[i].itinerQuad->setVisible(false);
            roadBookEntries[i].itiner2Quad->setVisible(false);
        }
    }
}
