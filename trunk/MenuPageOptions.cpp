
#include "MenuPageOptions.h"
#include "MenuPageOptionsKB.h"
#include "MenuPageStage.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"
#include "EventReceiver.h"
#include "Settings.h"
#include "KeyConfig.h"
#include "Player.h"
#include "FontManager.h"
#include "Competitor.h"
#include <assert.h>


#define PADDING         3
#define FTW             240
#define LTW             0

MenuPageOptions* MenuPageOptions::menuPageOptions = 0;

MenuPageOptions::MenuPageOptions()
    : window(0),
      tableKB(0),
      comboBoxDifficulty(0),
      comboBoxDriverType(0),
      comboBoxResolution(0),
      comboBoxDisplayBits(0),
      cbDesktopResolution(0),
      cbFullScreen(0),
      cbVsync(0),
      cbScanJoystick(0),
      cbShowNames(0),
      cbNavigationAssistant(0),
      cbManualShifting(0),
      cbSequentialShifting(0),
      cbLinearSteering(0),
      cbDamage(0),
      resolutionMap(),
      lastKeyName(0),
      primary(true)
{
    menuPageOptions = this;
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();

    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width/2-450, TheGame::getInstance()->getScreenSize().Height/2-300, TheGame::getInstance()->getScreenSize().Width/2+450, TheGame::getInstance()->getScreenSize().Height/2+300),
        false,
        L"Options",
        0,
        MI_WINDOW);

    irr::gui::IGUIStaticText* warningText = TheGame::getInstance()->getEnv()->addStaticText(L"(*) Changing this item only will take effect after save and restart",
        irr::core::recti(irr::core::position2di(PADDING, window->getRelativePosition().getSize().Height-22), irr::core::dimension2di(300, 20)),
        false,
        false,
        window);
    warningText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_BUILTIN));
    warningText->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width-132, window->getRelativePosition().getSize().Height-22), irr::core::dimension2du(130, 20)),
        window,
        MI_BUTTONSAVE,
        L"Save Settings");

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 22), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 46)),
        window,
        true,
        true,
        0);

    // ----------------------------
    // Game
    // ----------------------------
    irr::gui::IGUITab* tabGame = tc->addTab(L"Game", 0);

    unsigned int line = PADDING;
    TheGame::getInstance()->getEnv()->addStaticText(L"Difficulty",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    comboBoxDifficulty = TheGame::getInstance()->getEnv()->addComboBox(
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabGame->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        tabGame,
        MI_COMBOBOXDIFFICULTY);
    comboBoxDifficulty->addItem(L"Very hard");
    comboBoxDifficulty->addItem(L"Hard");
    comboBoxDifficulty->addItem(L"Medium");
    comboBoxDifficulty->addItem(L"Easy");
    comboBoxDifficulty->addItem(L"Very easy");
    
    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Scan joystick (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbScanJoystick = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->scanForJoystick,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBSCANJOYSTICK);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Show Names",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbShowNames = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->showNames,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBSHOWNAMES);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Navigation Assistant",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbNavigationAssistant = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->navigationAssistant,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBNAVIGATIONASSISTANT);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Manual shifting",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbManualShifting = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->manualGearShifting,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBMANUALSHIFTING);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Sequential shifting (if manual)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbSequentialShifting = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->sequentialGearShifting,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBSEQUENTIALSHIFTING);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Linear steering",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbLinearSteering = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->linearSteering,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBLINEARSTEERING);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Damage",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGame);

    cbDamage = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->useDamage,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGame,
        MI_CBDAMAGE);

    // ----------------------------
    // Graphics
    // ----------------------------
    irr::gui::IGUITab* tabGraphics = tc->addTab(L"Graphics", 0);

    line = PADDING;
    TheGame::getInstance()->getEnv()->addStaticText(L"Driver Type (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGraphics);

    comboBoxDriverType = TheGame::getInstance()->getEnv()->addComboBox(
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabGraphics->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        tabGraphics,
        MI_COMBOBOXDRIVERTYPE);
    comboBoxDriverType->addItem(L"Direct 3D 9");
    comboBoxDriverType->addItem(L"OpenGL");
    
    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Resolution (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGraphics);

    comboBoxResolution = TheGame::getInstance()->getEnv()->addComboBox(
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabGraphics->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        tabGraphics,
        MI_COMBOBOXRESOLUTION);
    int j = 0;
    for(int i=0;i<device->getVideoModeList()->getVideoModeCount();i++)
    {
        irr::core::stringw str = L"";
        irr::core::dimension2du res = device->getVideoModeList()->getVideoModeResolution(i);
        int dep = device->getVideoModeList()->getVideoModeDepth(i);
        if (res.Height >= 768)
        {
            str += res.Width;
            str += L"x";
            str += res.Height;
            str += L"x";
            str += dep;
            comboBoxResolution->addItem(str.c_str());
            resolutionMap[j] = i;
            j++;
            //if (auto_resolution==0 && resolutionX==res.Width && resolutionY==res.Height && display_bits==dep) resolution_cbox->setSelected(i+2);
        }
    }

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Same resolution as desktop (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGraphics);

   cbDesktopResolution = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->desktopResolution,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16 + 512, 16)),
        tabGraphics,
        MI_CBDESKTOPRESOLUTION,
        L"(Only if full screen is set. Resolution above will be discarded.)");


    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Full Screen (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGraphics);

    cbFullScreen = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->fullScreen,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGraphics,
        MI_CBFULLSCREEN);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Vsync (*)",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabGraphics);

    cbVsync = TheGame::getInstance()->getEnv()->addCheckBox(Settings::getInstance()->vsync,
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(16, 16)),
        tabGraphics,
        MI_CBVSYNC);


    // ----------------------------
    // Input
    // ----------------------------
    irr::gui::IGUITab* tabKB = tc->addTab(L"Input", 0);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(((tabKB->getRelativePosition().getSize().Width-16)*2)/4-2, 20)),
        tabKB,
        MI_BUTTONCENTER,
        L"Recalibrate Joystick");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(irr::core::position2di(((tabKB->getRelativePosition().getSize().Width-16)*2)/4, 0), irr::core::dimension2di((tabKB->getRelativePosition().getSize().Width-16)/4-2, 20)),
        tabKB,
        MI_BUTTONPRIMARY,
        L"Set Primary");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(irr::core::position2di(((tabKB->getRelativePosition().getSize().Width-16)*3)/4, 0), irr::core::dimension2di((tabKB->getRelativePosition().getSize().Width-16)/4-2, 20)),
        tabKB,
        MI_BUTTONSECONDARY,
        L"Set Alternative");

    tableKB = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(tabKB->getRelativePosition().getSize().Width, tabKB->getRelativePosition().getSize().Height-22)),
        tabKB,
        MI_TABLEKB,
        true);

    tableKB->addColumn(L"Action");
    tableKB->setColumnWidth(0, (tableKB->getRelativePosition().getSize().Width-16)/2);
    tableKB->setColumnOrdering(0, irr::gui::EGCO_NONE);
    tableKB->addColumn(L"Primary");
    tableKB->setColumnWidth(1, (tableKB->getRelativePosition().getSize().Width-16)/4);
    tableKB->setColumnOrdering(1, irr::gui::EGCO_NONE);
    tableKB->addColumn(L"Alternative");
    tableKB->setColumnWidth(2, (tableKB->getRelativePosition().getSize().Width-16)/4);
    tableKB->setColumnOrdering(2, irr::gui::EGCO_NONE);


    window->setVisible(false);
}

MenuPageOptions::~MenuPageOptions()
{
    //window->remove();
    menuPageOptions = 0;
    close();
}

bool MenuPageOptions::OnEvent(const irr::SEvent &event)
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
                    case MI_BUTTONSAVE:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::savebutton::clicked\n");
                        Settings::getInstance()->write();
                        TheGame::getInstance()->getEventReceiver()->saveKeyMapping();
                        //MenuManager::getInstance()->close();
                        return true;
                        break;
                    case MI_BUTTONCENTER:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::recalibratebutton::clicked\n");
                        TheGame::getInstance()->getEventReceiver()->recalibrate();
                        //MenuManager::getInstance()->close();
                        return true;
                        break;
                    case MI_BUTTONPRIMARY:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::primarybutton::clicked\n");
                        primary = true;
                        lastKeyName = (int)tableKB->getCellData(tableKB->getSelected(), 0);
                        MenuManager::getInstance()->open(MenuManager::MP_OPTIONSKB);
                        return true;
                        break;
                    case MI_BUTTONSECONDARY:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::secondarybutton::clicked\n");
                        primary = false;
                        lastKeyName = (int)tableKB->getCellData(tableKB->getSelected(), 0);
                        MenuManager::getInstance()->open(MenuManager::MP_OPTIONSKB);
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_TABLE_CHANGED:
            {
                switch (id)
                {
                    case MI_TABLEKB:
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_COMBO_BOX_CHANGED:
            {
                switch (id)
                {
                    case MI_COMBOBOXDRIVERTYPE:
                        if (comboBoxDriverType->getSelected() == 0)
                        {
                            Settings::getInstance()->driverType = "d3d9";
                        }
                        else
                        {
                            Settings::getInstance()->driverType = "opengl";
                        }
                        return true;
                        break;
                    case MI_COMBOBOXRESOLUTION:
                    {
                        int pos = resolutionMap[comboBoxResolution->getSelected()];
                        irr::core::dimension2du res = TheGame::getInstance()->getDevice()->getVideoModeList()->getVideoModeResolution(pos);
                        int dep = TheGame::getInstance()->getDevice()->getVideoModeList()->getVideoModeDepth(pos);
                        Settings::getInstance()->resolutionX = res.Width;
                        Settings::getInstance()->resolutionY = res.Height;
                        Settings::getInstance()->displayBits = dep;
                        return true;
                        break;
                    }
                    case MI_COMBOBOXDIFFICULTY:
                        Settings::getInstance()->difficulty = comboBoxDifficulty->getSelected();
                };
                break;
            }
            case irr::gui::EGET_CHECKBOX_CHANGED:
            {
                switch (id)
                {
                    case MI_CBDESKTOPRESOLUTION:
                        Settings::getInstance()->desktopResolution = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBFULLSCREEN:
                        Settings::getInstance()->fullScreen = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBVSYNC:
                        Settings::getInstance()->vsync = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBSCANJOYSTICK:
                        Settings::getInstance()->scanForJoystick = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBSHOWNAMES:
                        Settings::getInstance()->showNames = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBNAVIGATIONASSISTANT:
                        Settings::getInstance()->navigationAssistant = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBMANUALSHIFTING:
                        Settings::getInstance()->manualGearShifting = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        if (Player::getInstance()->getVehicle())
                        {
                            Player::getInstance()->getVehicle()->setGearShifting(Settings::getInstance()->manualGearShifting, Settings::getInstance()->sequentialGearShifting);
                        }
                        return true;
                        break;
                    case MI_CBSEQUENTIALSHIFTING:
                        Settings::getInstance()->sequentialGearShifting = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        if (Player::getInstance()->getVehicle())
                        {
                            Player::getInstance()->getVehicle()->setGearShifting(Settings::getInstance()->manualGearShifting, Settings::getInstance()->sequentialGearShifting);
                        }
                        return true;
                        break;
                    case MI_CBLINEARSTEERING:
                        Settings::getInstance()->linearSteering = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                    case MI_CBDAMAGE:
                        Settings::getInstance()->useDamage = ((irr::gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                        return true;
                        break;
                };
                break;
            }
        };
    }
    return false;
}

void MenuPageOptions::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageOptions::open()\n");
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageOptions::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageOptions::close()\n");
    window->setVisible(false);
}

void MenuPageOptions::refresh()
{
    refreshGame();
    refreshGraphics();
    refreshKB();
}

void MenuPageOptions::refreshGame()
{
    cbScanJoystick->setChecked(Settings::getInstance()->scanForJoystick);
    cbShowNames->setChecked(Settings::getInstance()->showNames);
    cbNavigationAssistant->setChecked(Settings::getInstance()->navigationAssistant);
    cbManualShifting->setChecked(Settings::getInstance()->manualGearShifting);
    cbSequentialShifting->setChecked(Settings::getInstance()->sequentialGearShifting);
    cbLinearSteering->setChecked(Settings::getInstance()->linearSteering);
    cbDamage->setChecked(Settings::getInstance()->useDamage);
}

void MenuPageOptions::refreshGraphics()
{
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();

    if (Settings::getInstance()->driverType == "d3d9")
    {
        comboBoxDriverType->setSelected(0);
    }
    else
    {
        comboBoxDriverType->setSelected(1);
    }

    int j = 0;
    for(int i=0;i<device->getVideoModeList()->getVideoModeCount();i++)
    {
        irr::core::stringw str = L"";
        irr::core::dimension2du res = device->getVideoModeList()->getVideoModeResolution(i);
        int dep = device->getVideoModeList()->getVideoModeDepth(i);
        if (res.Height >= 768)
        {
            if (Settings::getInstance()->resolutionX==res.Width &&
                Settings::getInstance()->resolutionY==res.Height &&
                Settings::getInstance()->displayBits==dep)
            {
                comboBoxResolution->setSelected(j);
                break;
            }
            j++;
        }
    }

    comboBoxDifficulty->setSelected(Settings::getInstance()->difficulty);

    cbDesktopResolution->setChecked(Settings::getInstance()->desktopResolution);
    cbFullScreen->setChecked(Settings::getInstance()->fullScreen);
    cbVsync->setChecked(Settings::getInstance()->vsync);
}

void MenuPageOptions::refreshKB()
{
    // ----------------------------
    // Input - KB
    // ----------------------------
    tableKB->clearRows();

    EventReceiver* er = TheGame::getInstance()->getEventReceiver();
    const EventReceiver::keyNameMap_t& keyNameMap = er->keyNameMap;
    unsigned int i = 0;
    for (EventReceiver::keyNameMap_t::const_iterator it = keyNameMap.begin();
         it != keyNameMap.end();
         it++, i++)
    {
        irr::core::stringw str;
        
        tableKB->addRow(i);

        str = L"";
        str += er->keyMap[it->second].keyLongName.c_str();
        tableKB->setCellText(i, 0, str.c_str());
        tableKB->setCellData(i, 0, (void*)(it->second));

        str = L"";
        if (er->keyMap[it->second].primaryKeyConfig)
        {
            str += er->keyMap[it->second].primaryKeyConfig->getName();
        }
        else
        {
            str += L"<empty>";
        }
        tableKB->setCellText(i, 1, str.c_str());

        str = L"";
        if (er->keyMap[it->second].secondaryKeyConfig)
        {
            str += er->keyMap[it->second].secondaryKeyConfig->getName();
        }
        else
        {
            str += L"<empty>";
        }
        tableKB->setCellText(i, 2, str.c_str());
    }
}

void MenuPageOptions::optionKBClosed()
{
    EventReceiver* er = TheGame::getInstance()->getEventReceiver();

    if (primary)
    {
        if (er->keyMap[lastKeyName].primaryKeyConfig)
        {
            delete er->keyMap[lastKeyName].primaryKeyConfig;
        }
        er->keyMap[lastKeyName].primaryKeyConfig = MenuPageOptionsKB::menuPageOptionsKB->kc;
    }
    else
    {
        if (er->keyMap[lastKeyName].secondaryKeyConfig)
        {
            delete er->keyMap[lastKeyName].secondaryKeyConfig;
        }
        er->keyMap[lastKeyName].secondaryKeyConfig = MenuPageOptionsKB::menuPageOptionsKB->kc;
    }
    refreshKB();
}
