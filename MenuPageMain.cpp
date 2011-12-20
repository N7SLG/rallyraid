
#include "MenuPageMain.h"
#include "MenuPageStage.h"
#include "MenuPageLoadSave.h"
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
#include "FontManager.h"
#include "ScreenQuad.h"
#include "Shaders.h"
#include <assert.h>


MenuPageMain* MenuPageMain::menuPageMain = 0;

MenuPageMain::MenuPageMain()
    : window(0),
      staticTextGameName(0),
      tableRaces(0),
      tableVehicles(0),
      checkBoxEditor(0),
      staticTextRaceData(0),
      staticTextVehicleData(0),
      bgQuad(0),
      buttonLoad(0),
      selectedRace(0),
      selectedVehicleType(0),
      willOpenOtherWindow(false),
      visible(true),
      raceImageQuad(0),
      vehicleImageQuad(0)      
{
    menuPageMain = this;
    window = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(0, 0, TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        0,
        MI_WINDOW);
    window->setScaleImage(true);
    window->setUseAlphaChannel(true);
    window->setImage(TheGame::getInstance()->getDriver()->getTexture("data/bg/transp.png"));

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,60,140,80),
        window,
        MI_BUTTONSTART,
        L"Start New Game");

    buttonLoad = TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,90,140,110),
        window,
        MI_BUTTONLOAD,
        L"Load Game");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,120,140,140),
        window,
        MI_BUTTONOPTIONS,
        L"Options");

/*    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,150,140,170),
        window,
        MI_BUTTONSETUP,
        L"Setup");
*/
    TheGame::getInstance()->getEnv()->addButton(
        //irr::core::recti(10,180,140,200),
        irr::core::recti(10,150,140,170),
        window,
        MI_BUTTONEXIT,
        L"Exit");

    checkBoxEditor = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->editorMode,
        irr::core::recti(10,window->getRelativePosition().getSize().Height-30,150,window->getRelativePosition().getSize().Height-10),
        window,
        MI_CBEDITORMODE,
        L"Editor Mode");

    staticTextGameName = TheGame::getInstance()->getEnv()->addStaticText(L"Rally Raid",
        irr::core::recti(window->getRelativePosition().getSize().Width/2 - 400,54,window->getRelativePosition().getSize().Width/2 + 400,90),
        false, false, window, 0, false);
    staticTextGameName->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_22PX_BORDER/*SPECIAL18*/));
    staticTextGameName->setOverrideColor(irr::video::SColor(255, 255, 255, 255));
    staticTextGameName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);

    irr::video::ITexture* havok_logo = TheGame::getInstance()->getDriver()->getTexture("data/menu_textures/havok_logo_1_128.png");
    irr::gui::IGUIImage* havok_image = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width - 10 - havok_logo->getOriginalSize().Width, window->getRelativePosition().getSize().Height - 20 - havok_logo->getOriginalSize().Height), havok_logo->getOriginalSize()),
        window);
    havok_image->setScaleImage(false);
    havok_image->setUseAlphaChannel(true);
    havok_image->setImage(havok_logo);

    irr::gui::IGUIStaticText* s = TheGame::getInstance()->getEnv()->addStaticText(L"Version: 1.0 - Build: 167",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width - 110, window->getRelativePosition().getSize().Height - 20), havok_logo->getOriginalSize()),
        false, false, window, 0, false);
    s->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_BUILTIN));

    bgQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        window->getRelativePosition().UpperLeftCorner,
        irr::core::dimension2du(window->getRelativePosition().getSize().Width, window->getRelativePosition().getSize().Height),
        false);
    bgQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d"];
    bgQuad->getMaterial().setTexture(0, TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));


    // ----------------------------
    // Races
    // ----------------------------
    tableRaces = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/6, (window->getRelativePosition().getSize().Height*2)/3), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3-2,(window->getRelativePosition().getSize().Height)/3-2)),
        window,
        MI_TABLERACES,
        true);

    tableRaces->addColumn(L"Race");
    tableRaces->setColumnWidth(0, tableRaces->getRelativePosition().getSize().Width-16);
    tableRaces->setColumnOrdering(0, irr::gui::EGCO_NONE);

    staticTextRaceData = TheGame::getInstance()->getEnv()->addStaticText(L"-",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/6, (window->getRelativePosition().getSize().Height)/2), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3-2,window->getRelativePosition().getSize().Height/6-4)),
        true,
        true,
        window,
        MI_STRACEDATA,
        true);
    staticTextRaceData->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_8PX));

    const int tmpRH = ((window->getRelativePosition().getSize().Height)/2) - 110;
    const int tmpRW = ((window->getRelativePosition().getSize().Width)/3);
    const int raceImageSize = tmpRW < tmpRH ? tmpRW : tmpRH;
    const int raceImageOffset = tmpRW < tmpRH ? 0 : (tmpRW - tmpRH) / 2;
    raceImageQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(window->getRelativePosition().getSize().Width/2 - raceImageSize - 2 - raceImageOffset, window->getRelativePosition().getSize().Height/2 - raceImageSize - 2),
        irr::core::dimension2du(raceImageSize, raceImageSize), false);
    raceImageQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    raceImageQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    raceImageQuad->getMaterial().setTexture(0, 0);

    // ----------------------------
    // Vehicles
    // ----------------------------
    tableVehicles = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2+2, (window->getRelativePosition().getSize().Height*2)/3), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3-2,window->getRelativePosition().getSize().Height/3-2)),
        window,
        MI_TABLEVEHICLES,
        true);

    tableVehicles->addColumn(L"Vehicle");
    tableVehicles->setColumnWidth(0, tableVehicles->getRelativePosition().getSize().Width-16);
    tableVehicles->setColumnOrdering(0, irr::gui::EGCO_NONE);

    staticTextVehicleData = TheGame::getInstance()->getEnv()->addStaticText(L"-",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2+2, window->getRelativePosition().getSize().Height/2), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3-2,window->getRelativePosition().getSize().Height/6-4)),
        true,
        true,
        window,
        MI_STRACEDATA,
        true);
    staticTextVehicleData->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_10PX));

    vehicleImageQuad = new ScreenQuad(TheGame::getInstance()->getDriver(),
        irr::core::position2di(window->getRelativePosition().getSize().Width/2 + 2 + raceImageOffset, window->getRelativePosition().getSize().Height/2 - raceImageSize - 2),
        irr::core::dimension2du(raceImageSize, raceImageSize), false);
    vehicleImageQuad->getMaterial().MaterialType = Shaders::getInstance()->materialMap["quad2d_t"];
    vehicleImageQuad->getMaterial().setFlag(irr::video::EMF_BLEND_OPERATION, true);
    vehicleImageQuad->getMaterial().setTexture(0, 0);

    setVisible(false);
}

MenuPageMain::~MenuPageMain()
{
    //window->remove();
    menuPageMain = 0;
    close();
    if (bgQuad)
    {
        delete bgQuad;
        bgQuad = 0;
    }
    if (raceImageQuad)
    {
        delete raceImageQuad;
        raceImageQuad = 0;
    }

    if (vehicleImageQuad)
    {
        delete vehicleImageQuad;
        vehicleImageQuad = 0;
    }
}

bool MenuPageMain::OnEvent(const irr::SEvent &event)
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
                        dprintf(MY_DEBUG_NOTE, "mainmenu::startbutton::clicked: selected race: %s, vehicle: %s\n",
                            selectedRace?selectedRace->getName().c_str():"-", selectedVehicleType?selectedVehicleType->getName().c_str():"-");
                        if (selectedRace && selectedVehicleType)
                        {
                            willOpenOtherWindow = true;
                            MenuPageStage::menuPageStage->selectedRace = selectedRace;
                            MenuPageStage::menuPageStage->selectedStage = 0;
                            MenuPageStage::menuPageStage->selectedVehicleType = selectedVehicleType;
                            MenuPageStage::menuPageStage->newRace = true;
                            MenuManager::getInstance()->close();
                            MenuManager::getInstance()->open(MenuManager::MP_STAGE);
                            //GamePlay::getInstance()->startNewGame(selectedRace, selectedVehicleType);
                        }
                        return true;
                        break;
                    case MI_BUTTONLOAD:
                        dprintf(MY_DEBUG_NOTE, "mainmenu::loadbutton::clicked\n");
                        willOpenOtherWindow = true;
                        MenuPageLoadSave::menuPageLoadSave->load = true;
                        MenuPageLoadSave::menuPageLoadSave->prevMP = MenuManager::MP_MAIN;
                        MenuManager::getInstance()->close();
                        MenuManager::getInstance()->open(MenuManager::MP_LOADSAVE);
                        return true;
                        break;
                    case MI_BUTTONOPTIONS:
                        dprintf(MY_DEBUG_NOTE, "mainmenu::optionsbutton::clicked\n");
                        willOpenOtherWindow = false;
                        MenuManager::getInstance()->open(MenuManager::MP_OPTIONS);
                        return true;
                        break;
                    case MI_BUTTONSETUP:
                        dprintf(MY_DEBUG_NOTE, "mainmenu::setupbutton::clicked\n");
                        willOpenOtherWindow = false;
                        MenuManager::getInstance()->open(MenuManager::MP_SETUP);
                        return true;
                        break;
                    case MI_BUTTONEXIT:
                        dprintf(MY_DEBUG_NOTE, "mainmenu::exitbutton::clicked\n");
                        TheGame::getInstance()->setTerminate();
                        return true;
                        break;
                };
                break;
            }
            
            case irr::gui::EGET_TABLE_CHANGED:
            {
                switch (id)
                {
                    case MI_TABLERACES:
                        refreshRaceData((Race*)tableRaces->getCellData(tableRaces->getSelected(), 0));
                        return true;
                        break;
                    case MI_TABLEVEHICLES:
                        refreshVehicleData((VehicleType*)tableVehicles->getCellData(tableVehicles->getSelected(), 0));
                        return true;
                        break;
                };
                break;
            }
            /*
            case irr::gui::EGET_TABLE_SELECTED_AGAIN:
            {
                switch (id)
                {
                    case MI_TABLEDAYS:
                        RaceManager::getInstance()->editorDay = (Day*)tableDays->getCellData(tableDays->getSelected(), 0);
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORDAY);
                        return true;
                        break;
                };
                break;
            }
            */
            case irr::gui::EGET_CHECKBOX_CHANGED:
            {
                switch (id)
                {
                    case MI_CBEDITORMODE:
                        dprintf(MY_DEBUG_INFO, "set editor mode to: %u\n", checkBoxEditor->isChecked());
                        Settings::getInstance()->editorMode = checkBoxEditor->isChecked();
                        if (Settings::getInstance()->editorMode)
                        {
                            TheGame::getInstance()->getEnv()->getSkin()->setFont(FontManager::getInstance()->getFont(FontManager::FONT_BUILTIN));
                        }
                        else
                        {
                            TheGame::getInstance()->getEnv()->getSkin()->setFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_10PX));
                        }
                        break;
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageMain::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageMain::open()\n");
    refresh();
    setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(tableRaces);
    
    // stop necessarry elements in the game
    TheGame::getInstance()->setInGame(false);
    Hud::getInstance()->setVisible(false);
}

void MenuPageMain::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageMain::close()\n");
    setVisible(false);

    // start necessarry elements in the game if returns
    if (!willOpenOtherWindow)
    {
        TheGame::getInstance()->setInGame(true);
        Hud::getInstance()->setVisible(true);
    }
    willOpenOtherWindow = false;
}

void MenuPageMain::refresh()
{
    buttonLoad->setEnabled(!GamePlay::getInstance()->loadableGames.empty());
    // ----------------------------
    // Races
    // ----------------------------
    tableRaces->clearRows();

    const RaceManager::raceMap_t& raceMap = RaceManager::getInstance()->getRaceMap();
    unsigned int i = 0;
    for (RaceManager::raceMap_t::const_iterator rit = raceMap.begin();
         rit != raceMap.end();
         rit++, i++)
    {
        irr::core::stringw str;
        
        tableRaces->addRow(i);

        str = L"";
        str += rit->second->getLongName().c_str();
        tableRaces->setCellText(i, 0, str.c_str());
        tableRaces->setCellData(i, 0, (void*)rit->second);
        
        if (i==0)
        {
            selectedRace = rit->second;
        }
    }
    if (!raceMap.empty())
    {
        tableRaces->setSelected(0);
        refreshRaceData((Race*)tableRaces->getCellData(tableRaces->getSelected(), 0));
    }

    // ----------------------------
    // Vehicles
    // ----------------------------
    tableVehicles->clearRows();

    const VehicleTypeManager::vehicleTypeMap_t& vehicleTypeMap = VehicleTypeManager::getInstance()->getVehicleTypeMap();
    i = 0;
    /*
    for (VehicleTypeManager::vehicleTypeMap_t::const_iterator vit = vehicleTypeMap.begin();
         vit != vehicleTypeMap.end();
         vit++, i++)
    */
    foreach(vit, VehicleTypeManager::getInstance()->getVehicleTypeMap(), VehicleTypeManager::vehicleTypeMap_t)
    {
        irr::core::stringw str;
        
        tableVehicles->addRow(i);

        str = L"";
        str += vit->second->getLongName().c_str();
        tableVehicles->setCellText(i, 0, str.c_str());
        tableVehicles->setCellData(i, 0, (void*)vit->second);
        
        if (i==VehicleTypeManager::getInstance()->getVehicleTypeMap().size()-1)
        {
            selectedVehicleType = vit->second;
        }
        i++;
    }
    if (!VehicleTypeManager::getInstance()->getVehicleTypeMap().empty())
    {
        tableVehicles->setSelected(VehicleTypeManager::getInstance()->getVehicleTypeMap().size()-1);
        refreshVehicleData((VehicleType*)tableVehicles->getCellData(tableVehicles->getSelected(), 0));
    }
}

void MenuPageMain::refreshRaceData(Race* race)
{
    irr::core::stringw str = L"";

    str += race->getLongName().c_str();
    str += L"\n\n";
    str += race->getShortDescription().c_str();

    staticTextRaceData->setText(str.c_str());
    selectedRace = race;

    raceImageQuad->getMaterial().setTexture(0, selectedRace->getImage());
}

void MenuPageMain::refreshVehicleData(VehicleType* vehicleType)
{
    irr::core::stringw str = L"";

    str += vehicleType->getLongName().c_str();

    str += L"\n\nSpeed: ";
    if ((int)vehicleType->maxSpeed)
        str += (int)((vehicleType->maxSpeed*10) / VehicleType::maxMaxSpeed);
    else
        str += L"0";

    str += L" / 10\nBrake: ";
    if ((int)vehicleType->maxBrakeForce)
        str += (int)((vehicleType->maxBrakeForce*10/vehicleType->mass) / VehicleType::maxMaxBrakeForce);
    else
        str += L"0";

    str += L" / 10\nAccelerate: ";
    if ((int)vehicleType->maxTorqueRate)
        str += (int)((vehicleType->maxTorqueRate*10/vehicleType->mass) / VehicleType::maxMaxTorqueRate);
    else
        str += L"0";

    str += L" / 10\nSteering: ";
    if ((int)vehicleType->maxSteerAngle)
        str += (int)((vehicleType->maxSteerAngle*10) / VehicleType::maxMaxSteerAngle);
    else
        str += L"0";
    
    str += L" / 10\n";

    staticTextVehicleData->setText(str.c_str());
    selectedVehicleType = vehicleType;

    vehicleImageQuad->getMaterial().setTexture(0, selectedVehicleType->getImage());
}

void MenuPageMain::setVisible(bool visible)
{
    if (visible == this->visible) return;
    
    this->visible = visible;
    
    window->setVisible(visible);
    bgQuad->setVisible(visible);
    raceImageQuad->setVisible(visible);
    vehicleImageQuad->setVisible(visible);
}

void MenuPageMain::render()
{
    if (!visible) return;
    
    bgQuad->render();
    raceImageQuad->render();
    vehicleImageQuad->render();
}
