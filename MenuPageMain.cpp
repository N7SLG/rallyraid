
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
      selectedRace(0),
      selectedVehicleType(0),
      willOpenOtherWindow(false)
{
    menuPageMain = this;
    window = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(0, 0, TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        0,
        MI_WINDOW);
    window->setScaleImage(true);
    window->setImage(TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,60,90,80),
        window,
        MI_BUTTONSTART,
        L"Start New Game");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,90,90,110),
        window,
        MI_BUTTONLOAD,
        L"Load Game");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,120,90,140),
        window,
        MI_BUTTONOPTIONS,
        L"Options");

    checkBoxEditor = TheGame::getInstance()->getEnv()->addCheckBox(TheGame::getInstance()->getEditorMode(),
        irr::core::recti(10,window->getRelativePosition().getSize().Height-30,150,window->getRelativePosition().getSize().Height-10),
        window,
        MI_CBEDITORMODE,
        L"Editor Mode");

    staticTextGameName = TheGame::getInstance()->getEnv()->addStaticText(L"Rally Raid 2012",
        irr::core::recti(window->getRelativePosition().getSize().Width/2 - 400,54,window->getRelativePosition().getSize().Width/2 + 400,88),
        false, false, window, 0, false);
    staticTextGameName->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL18));
    staticTextGameName->setOverrideColor(irr::video::SColor(255, 255, 255, 255));
    staticTextGameName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);

    irr::video::ITexture* havok_logo = TheGame::getInstance()->getDriver()->getTexture("data/menu_textures/havok_logo_1_128.png");
    irr::gui::IGUIImage* havok_image = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width - 10 - havok_logo->getOriginalSize().Width, window->getRelativePosition().getSize().Height - 20 - havok_logo->getOriginalSize().Height), havok_logo->getOriginalSize()),
        window);
    havok_image->setScaleImage(false);
    havok_image->setUseAlphaChannel(true);
    havok_image->setImage(havok_logo);

    TheGame::getInstance()->getEnv()->addStaticText(L"Build: 140",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width - 60, window->getRelativePosition().getSize().Height - 20), havok_logo->getOriginalSize()),
        false, false, window, 0, false);

    // ----------------------------
    // Races
    // ----------------------------
    tableRaces = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/4, (window->getRelativePosition().getSize().Height*2)/3), irr::core::dimension2di(window->getRelativePosition().getSize().Width/4-2,(window->getRelativePosition().getSize().Height)/3-2)),
        window,
        MI_TABLERACES,
        true);

    tableRaces->addColumn(L"Race");
    tableRaces->setColumnWidth(0, tableRaces->getRelativePosition().getSize().Width-16);

    staticTextRaceData = TheGame::getInstance()->getEnv()->addStaticText(L"-",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/4, (window->getRelativePosition().getSize().Height)/2), irr::core::dimension2di(window->getRelativePosition().getSize().Width/4-2,window->getRelativePosition().getSize().Height/6-4)),
        true,
        true,
        window,
        MI_STRACEDATA,
        true);
    staticTextRaceData->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_NORMAL));

    // ----------------------------
    // Vehicles
    // ----------------------------
    tableVehicles = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2+2, (window->getRelativePosition().getSize().Height*2)/3), irr::core::dimension2di(window->getRelativePosition().getSize().Width/4-2,window->getRelativePosition().getSize().Height/3-2)),
        window,
        MI_TABLEVEHICLES,
        true);

    tableVehicles->addColumn(L"Vehicle");
    tableVehicles->setColumnWidth(0, tableVehicles->getRelativePosition().getSize().Width-16);

    staticTextVehicleData = TheGame::getInstance()->getEnv()->addStaticText(L"-",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/2+2, window->getRelativePosition().getSize().Height/2), irr::core::dimension2di(window->getRelativePosition().getSize().Width/4-2,window->getRelativePosition().getSize().Height/6-4)),
        true,
        true,
        window,
        MI_STRACEDATA,
        true);
    staticTextVehicleData->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_NORMALBOLD));

    window->setVisible(false);
}

MenuPageMain::~MenuPageMain()
{
    //window->remove();
    menuPageMain = 0;
    close();
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
                        TheGame::getInstance()->setEditorMode(checkBoxEditor->isChecked());
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
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(tableRaces);
    
    // stop necessarry elements in the game
    TheGame::getInstance()->setInGame(false);
    Hud::getInstance()->setVisible(false);
}

void MenuPageMain::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageMain::close()\n");
    window->setVisible(false);

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
}

void MenuPageMain::refreshRaceData(Race* race)
{
    irr::core::stringw str = L"";

    str += race->getLongName().c_str();
    str += L"\n\n";
    str += race->getShortDescription().c_str();

    staticTextRaceData->setText(str.c_str());
    selectedRace = race;
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
        str += (int)((vehicleType->maxBrakeForce*10) / VehicleType::maxMaxBrakeForce);
    else
        str += L"0";

    str += L" / 10\nAccelerate: ";
    if ((int)vehicleType->maxTorque)
        str += (int)((vehicleType->maxTorque*10) / VehicleType::maxMaxTorque);
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
}
