
#include "MenuPageStage.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"

#include "TheEarth.h"
#include "ObjectPool.h"
#include "ObjectPoolManager.h"
#include "RaceManager.h"
#include "Race.h"
#include "Settings.h"
#include "RoadManager.h"
#include "Road.h"
#include "Day.h"
#include "Stage.h"
#include "RoadType.h"
#include "ObjectWireGlobalObject.h"
#include "Competitor.h"
#include "VehicleTypeManager.h"
#include "VehicleType.h"
#include "GamePlay.h"
#include "Hud.h"
#include "VehicleManager.h"
#include "ScreenQuad.h"
#include "Shaders.h"
#include "ItinerPoint.h"
#include <assert.h>
#include "FontManager.h"


#define ROADBOOKBG_SIZE_X       (768)
#define ROADBOOKBG_SIZE_Y       (128)
#define ROADBOOKBG_HSIZE_X      (ROADBOOKBG_SIZE_X / 2)

#define ROADBOOKENTRY_NUM_SIZE_X    (30) // 32
#define ROADBOOKENTRY_NUM_SIZE_Y    (16)
#define ROADBOOKENTRY_GD_SIZE_X     (92) // 96
#define ROADBOOKENTRY_GD_SIZE_Y     (32)
#define ROADBOOKENTRY_LD_SIZE_X     (92) // 96
#define ROADBOOKENTRY_LD_SIZE_Y     (24)
#define ROADBOOKENTRY_ITINER_SIZE   (58)
#define ROADBOOKENTRY_ITINER2_SIZE  (58) // 44
#define ROADBOOKENTRY_NOTE_SIZE_X   (62)
#define ROADBOOKENTRY_NOTE_SIZE_Y   (31) // 62

#define ROADBOOKENTRY_NUM_POS_X(num)    (num*ROADBOOKBG_SIZE_Y+0)
#define ROADBOOKENTRY_NUM_POS_Y         (2)
#define ROADBOOKENTRY_GD_POS_X(num)     (num*ROADBOOKBG_SIZE_Y+32)
#define ROADBOOKENTRY_GD_POS_Y          (2)
#define ROADBOOKENTRY_LD_POS_X(num)     (num*ROADBOOKBG_SIZE_Y+5) // +32
#define ROADBOOKENTRY_LD_POS_Y          (36)
#define ROADBOOKENTRY_ITINER_POS_X(num) (num*ROADBOOKBG_SIZE_Y+3)
#define ROADBOOKENTRY_ITINER_POS_Y      (67)
#define ROADBOOKENTRY_ITINER2_POS_X(num) (num*ROADBOOKBG_SIZE_Y+67)
#define ROADBOOKENTRY_ITINER2_POS_Y     (67)
#define ROADBOOKENTRY_NOTE_POS_X(num)   (num*ROADBOOKBG_SIZE_Y+64)
#define ROADBOOKENTRY_NOTE_POS_Y        (64+32) // 64

#define ROADBOOKBG_POS_Y                (600)
#define STAGEIMAGE_POS_Y                (140)
#define STAGEIMAGE_SIZE_Y               (ROADBOOKBG_POS_Y-STAGEIMAGE_POS_Y)


MenuPageStage* MenuPageStage::menuPageStage = 0;

MenuPageStage::MenuPageStage()
    : window(0),
      staticTextStageName(0),
      staticTextStageDescription(0),
      bgQuad(0),
      willOpenOtherWindow(false),
      visible(true),
      selectedRace(0),
      selectedStage(0),
      selectedVehicleType(0),
      itinerIt(),
      roadBookBGQuad(0),
      stageImageQuad(0),
      roadBookScrollBar(0)
{
    menuPageStage = this;
    window = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(0, 0, TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        0,
        MI_WINDOW);
    window->setScaleImage(true);
    //window->setColor(irr::video::SColor(255,0,0,0));
    window->setUseAlphaChannel(true);
    window->setImage(TheGame::getInstance()->getDriver()->getTexture("data/bg/transp.png"));

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,60,90,80),
        window,
        MI_BUTTONSTART,
        L"Start The Game");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,90,90,110),
        window,
        MI_BUTTONBACK,
        L"Back To Main");

    staticTextStageName = TheGame::getInstance()->getEnv()->addStaticText(L"",
        irr::core::recti(120,54,1200,88),
        false, false, window, 0, false);
    staticTextStageName->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL18));
    staticTextStageName->setOverrideColor(irr::video::SColor(255, 255, 255, 255));

    staticTextStageDescription = TheGame::getInstance()->getEnv()->addStaticText(L"",
        irr::core::recti(120,88,1200,128),
        false, true, window, 0, false);
    staticTextStageDescription->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_NORMAL));
    //staticTextStageDescription->setOverrideColor(irr::video::SColor(255, 255, 255, 255));

    bgQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        window->getRelativePosition().UpperLeftCorner,
        irr::core::dimension2du(window->getRelativePosition().getSize().Width, window->getRelativePosition().getSize().Height),
        false);
    bgQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    bgQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));

    stageImageQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X,
        STAGEIMAGE_POS_Y-10/*(window->getRelativePosition().getSize().Height/4)*/),
        irr::core::dimension2du(ROADBOOKBG_SIZE_X, STAGEIMAGE_SIZE_Y/*window->getRelativePosition().getSize().Height/2*/), false);
    stageImageQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    stageImageQuad->getMaterial().setTexture(0, 0);


    // ----------------------------
    // RoadBook
    // ----------------------------
    roadBookBGQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X,
        ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/),
        irr::core::dimension2du(ROADBOOKBG_SIZE_X-1, ROADBOOKBG_SIZE_Y-1), false);
    roadBookBGQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
    roadBookBGQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
    roadBookBGQuad->getMaterial().UseMipMaps = false;
    roadBookBGQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/hud/roadbookbg_menu.png"));

    for (unsigned int i = 0; i < 6; i++)
    {
        roadBookEntries[i].numText = TheGame::getInstance()->getEnv()->addStaticText(L"0",
            irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_NUM_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_NUM_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_NUM_SIZE_X, ROADBOOKENTRY_NUM_SIZE_Y)),
            false, false, window, -1, false);
        roadBookEntries[i].numText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SMALL));
        roadBookEntries[i].numText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        //if (i!=1) roadBookEntries[i].numText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].globalDistanceText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_GD_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_GD_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_GD_SIZE_X, ROADBOOKENTRY_GD_SIZE_Y)),
            false, false, window, -1, false);
        roadBookEntries[i].globalDistanceText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_EXTRALARGEBOLD));
        roadBookEntries[i].globalDistanceText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        //if (i!=1) roadBookEntries[i].globalDistanceText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].localDistanceText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_LD_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_LD_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_LD_SIZE_X, ROADBOOKENTRY_LD_SIZE_Y)),
            false, false, window, -1, false);
        roadBookEntries[i].localDistanceText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_LARGEBOLD));
        //roadBookEntries[i].localDistanceText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);
        //if (i!=1) roadBookEntries[i].localDistanceText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].noteText = TheGame::getInstance()->getEnv()->addStaticText(L"0,00",
            irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_NOTE_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_NOTE_POS_Y),
            irr::core::dimension2di(ROADBOOKENTRY_NOTE_SIZE_X, ROADBOOKENTRY_NOTE_SIZE_Y)),
            false, true, window, -1, false);
        roadBookEntries[i].noteText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SMALLBOLD));
        roadBookEntries[i].noteText->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
        //if (i!=1) roadBookEntries[i].noteText->setOverrideColor(irr::video::SColor(255, 127, 127, 127));

        roadBookEntries[i].itinerQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
            irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_ITINER_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_ITINER_POS_Y),
            irr::core::dimension2du(ROADBOOKENTRY_ITINER_SIZE, ROADBOOKENTRY_ITINER_SIZE), false);
        roadBookEntries[i].itinerQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
        //roadBookEntries[i].itinerQuad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
        //roadBookEntries[i].itinerQuad->getMaterial().UseMipMaps = false;
        roadBookEntries[i].itinerQuad->getMaterial().setTexture(0, 0);

        roadBookEntries[i].itiner2Quad = new ScreenQuad(TheGame::getInstance()->getDriver(),
            irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X + ROADBOOKENTRY_ITINER2_POS_X(i),
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKENTRY_ITINER2_POS_Y),
            irr::core::dimension2du(ROADBOOKENTRY_ITINER2_SIZE, ROADBOOKENTRY_ITINER2_SIZE), false);
        roadBookEntries[i].itiner2Quad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_ANTI_ALIASING, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_BILINEAR_FILTER, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().setFlag(irr::video::EMF_TRILINEAR_FILTER, false);
        //roadBookEntries[i].itiner2Quad->getMaterial().UseMipMaps = false;
        roadBookEntries[i].itiner2Quad->getMaterial().setTexture(0, 0);
    }

    roadBookScrollBar = TheGame::getInstance()->getEnv()->addScrollBar(true, 
            irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2 - ROADBOOKBG_HSIZE_X,
            ROADBOOKBG_POS_Y/*(window->getRelativePosition().getSize().Height*3/4) + 10*/ + ROADBOOKBG_SIZE_Y),
            irr::core::dimension2di(ROADBOOKBG_SIZE_X, 20)),
            window,
            MI_SCROLLROADBOOK);
    roadBookScrollBar->setMin(0);
    roadBookScrollBar->setMax(0);
    roadBookScrollBar->setLargeStep(5);
    roadBookScrollBar->setSmallStep(1);

    setVisible(false);
}

MenuPageStage::~MenuPageStage()
{
    //window->remove();
    menuPageStage = 0;
    close();
    if (bgQuad)
    {
        delete bgQuad;
        bgQuad = 0;
    }
    if (roadBookBGQuad)
    {
        delete roadBookBGQuad;
        roadBookBGQuad = 0;
    }
    if (stageImageQuad)
    {
        delete stageImageQuad;
        stageImageQuad = 0;
    }
}

bool MenuPageStage::OnEvent(const irr::SEvent &event)
{
    //dprintf(MY_DEBUG_NOTE, "MenuPageEditor::OnEvent()\n");
    if (event.EventType == irr::EET_GUI_EVENT)
    {
        int id = event.GUIEvent.Caller->getID();

        switch (event.GUIEvent.EventType)
        {
            case irr::gui::EGET_ELEMENT_CLOSED:
            {
                switch (id)
                {
                    case MI_WINDOW:
                        //dprintf(MY_DEBUG_NOTE, "event on close editor window\n");
                        MenuManager::getInstance()->close();
                        return true;
                        break;
                    default:
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_BUTTON_CLICKED:
            {
                switch (id)
                {
                    case MI_BUTTONSTART:
                        dprintf(MY_DEBUG_NOTE, "stagemenu::startbutton::clicked: selected race: %s, vehicle: %s\n",
                            selectedRace?selectedRace->getName().c_str():"-", selectedVehicleType?selectedVehicleType->getName().c_str():"-");
                        if (selectedRace && selectedVehicleType)
                        {
                            willOpenOtherWindow = false;
                            MenuManager::getInstance()->close();
                            GamePlay::getInstance()->startNewGame(selectedRace, selectedVehicleType);
                        }
                        return true;
                        break;
                    case MI_BUTTONBACK:
                        dprintf(MY_DEBUG_NOTE, "stagemenu::backbutton::clicked: selected race: %s, vehicle: %s\n",
                            selectedRace?selectedRace->getName().c_str():"-", selectedVehicleType?selectedVehicleType->getName().c_str():"-");
                        willOpenOtherWindow = true;
                        MenuManager::getInstance()->close();
                        MenuManager::getInstance()->open(MenuManager::MP_MAIN);
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_SCROLL_BAR_CHANGED:
            {
                switch (id)
                {
                    case MI_SCROLLROADBOOK:
                    {
                        int pos = roadBookScrollBar->getPos();
                        dprintf(MY_DEBUG_NOTE, "stagemenu::scrollbar::clicked: pos: %d\n", pos);
                        if (selectedStage)
                        {
                            itinerIt = selectedStage->getItinerPointList().begin();
                            for (int i = 0; i < pos; i++, itinerIt++);
                            updateRoadBook();
                        }
                        return true;
                        break;
                    }
                };
                break;
            }
        };
    }
    return false;
}

void MenuPageStage::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageStage::open()\n");

    if (selectedStage == 0)
    {
        assert(selectedRace);
        Day* day = 0;

        if (!selectedRace->getDayMap().empty())
        {
            day = selectedRace->getDayMap().begin()->second;
        }
        if (day && !day->getStageMap().empty())
        {
            selectedStage = day->getStageMap().begin()->second;
        }
    }
    else
    {
        selectedRace = selectedStage->getParent()->getParent();
    }
    assert(selectedStage);
    itinerIt = selectedStage->getItinerPointList().begin();
    stageImageQuad->getMaterial().setTexture(0, selectedStage->getImage());
    int size = (int)selectedStage->getItinerPointList().size();
    if (size <= 6)
    {
        roadBookScrollBar->setMax(0);
    }
    else
    {
        roadBookScrollBar->setMax(size-6);
    }
    irr::core::stringw str;
    str += selectedRace->getLongName().c_str();
    str += L" - ";
    str += selectedStage->getParent()->getLongName().c_str();
    str += L" - ";
    str += selectedStage->getLongName().c_str();
    staticTextStageName->setText(str.c_str());
    str = L"";
    str += selectedStage->getShortDescription().c_str();
    staticTextStageDescription->setText(str.c_str());
    refresh();
    setVisible(true);
    //TheGame::getInstance()->getEnv()->setFocus(tableRaces);
    
    // stop necessarry elements in the game
    TheGame::getInstance()->setInGame(false);
    Hud::getInstance()->setVisible(false);
}

void MenuPageStage::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageStage::close()\n");
    setVisible(false);

    // start necessarry elements in the game if returns
    if (!willOpenOtherWindow)
    {
        TheGame::getInstance()->setInGame(true);
        Hud::getInstance()->setVisible(true);
    }
    willOpenOtherWindow = false;
}

void MenuPageStage::refresh()
{
    
}

void MenuPageStage::setVisible(bool visible)
{
    if (visible == this->visible) return;
    
    this->visible = visible;
    
    window->setVisible(visible);
    bgQuad->setVisible(visible);
    roadBookBGQuad->setVisible(visible);
    stageImageQuad->setVisible(visible);
    updateRoadBook();
}

void MenuPageStage::updateRoadBook()
{
    ItinerManager::itinerPointList_t::const_iterator it = itinerIt;
    ItinerPoint* itinerPoint = 0;
    for (unsigned int i = 0; i < 6; i++)
    {
        itinerPoint = 0;
        if (selectedStage && it != selectedStage->getItinerPointList().end())
        {
            itinerPoint = *(it);
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

void MenuPageStage::render()
{
    if (!visible) return;
    
    bgQuad->render();
    roadBookBGQuad->render();
    stageImageQuad->render();
    for (unsigned int i = 0; i < 6; i++)
    {
        roadBookEntries[i].itinerQuad->render();
        roadBookEntries[i].itiner2Quad->render();
    }
}
