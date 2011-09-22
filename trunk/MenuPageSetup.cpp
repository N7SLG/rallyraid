
#include "MenuPageSetup.h"
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

MenuPageSetup* MenuPageSetup::menuPageSetup = 0;

MenuPageSetup::MenuPageSetup()
    : window(0),
      scrollSuspensionSpring(0),
      scrollSuspensionDamper(0),
      scrollBrakeBalance(0),
      editBoxPlayerName(0),
      editBoxPlayerCoName(0),
      editBoxPlayerTeamName(0)
{
    menuPageSetup = this;
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();

    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width/2-350, TheGame::getInstance()->getScreenSize().Height/2-300, TheGame::getInstance()->getScreenSize().Width/2+350, TheGame::getInstance()->getScreenSize().Height/2+300),
        false,
        L"Setup",
        0,
        MI_WINDOW);

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 22), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 24)),
        window,
        true,
        true,
        0);

    // ----------------------------
    // Vehicle
    // ----------------------------
    irr::gui::IGUITab* tabVehicle = tc->addTab(L"Vehicle", 0);
    const int frontRearSize = 40;

    unsigned int line = PADDING;
    TheGame::getInstance()->getEnv()->addStaticText(L"Suspension spring",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabVehicle);

    irr::gui::IGUIStaticText* s = TheGame::getInstance()->getEnv()->addStaticText(L"soft",
        irr::core::recti(irr::core::position2di((PADDING*2)+FTW, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);
    s->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);

    TheGame::getInstance()->getEnv()->addStaticText(L"hard",
        irr::core::recti(irr::core::position2di(tabVehicle->getRelativePosition().getSize().Width-PADDING-frontRearSize, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);

    scrollSuspensionSpring = TheGame::getInstance()->getEnv()->addScrollBar(true, 
        irr::core::recti(irr::core::position2di(FTW+(PADDING*3)+frontRearSize, line), irr::core::dimension2di(tabVehicle->getRelativePosition().getSize().Width-(6*PADDING)-LTW-FTW-(frontRearSize*2), 16)),
        //irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabVehicle->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        tabVehicle,
        MI_SCROLLSUSPENSIONSPRING);
    scrollSuspensionSpring->setMin(-20);
    scrollSuspensionSpring->setMax(20);
    scrollSuspensionSpring->setLargeStep(5);
    scrollSuspensionSpring->setSmallStep(1);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Suspension damping",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabVehicle);

    s = TheGame::getInstance()->getEnv()->addStaticText(L"slow",
        irr::core::recti(irr::core::position2di((PADDING*2)+FTW, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);
    s->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);

    TheGame::getInstance()->getEnv()->addStaticText(L"fast",
        irr::core::recti(irr::core::position2di(tabVehicle->getRelativePosition().getSize().Width-PADDING-frontRearSize, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);

    scrollSuspensionDamper = TheGame::getInstance()->getEnv()->addScrollBar(true, 
        irr::core::recti(irr::core::position2di(FTW+(PADDING*3)+frontRearSize, line), irr::core::dimension2di(tabVehicle->getRelativePosition().getSize().Width-(6*PADDING)-LTW-FTW-(frontRearSize*2), 16)),
        //irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabVehicle->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        tabVehicle,
        MI_SCROLLSUSPENSIONDAMPER);
    scrollSuspensionDamper->setMin(-20);
    scrollSuspensionDamper->setMax(20);
    scrollSuspensionDamper->setLargeStep(5);
    scrollSuspensionDamper->setSmallStep(1);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Brake balance",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabVehicle);

    s = TheGame::getInstance()->getEnv()->addStaticText(L"front",
        irr::core::recti(irr::core::position2di((PADDING*2)+FTW, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);
    s->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT);

    TheGame::getInstance()->getEnv()->addStaticText(L"rear",
        irr::core::recti(irr::core::position2di(tabVehicle->getRelativePosition().getSize().Width-PADDING-frontRearSize, line), irr::core::dimension2di(frontRearSize, 16)),
        false,
        false,
        tabVehicle);

    scrollBrakeBalance = TheGame::getInstance()->getEnv()->addScrollBar(true, 
        irr::core::recti(irr::core::position2di(FTW+(PADDING*3)+frontRearSize, line), irr::core::dimension2di(tabVehicle->getRelativePosition().getSize().Width-(6*PADDING)-LTW-FTW-(frontRearSize*2), 16)),
            tabVehicle,
            MI_SCROLLBRAKEBALANCE);
    scrollBrakeBalance->setMin(0);
    scrollBrakeBalance->setMax(100);
    scrollBrakeBalance->setLargeStep(10);
    scrollBrakeBalance->setSmallStep(1);


    // ----------------------------
    // Player
    // ----------------------------
    irr::gui::IGUITab* tabPlayer = tc->addTab(L"Player", 0);

    line = PADDING;
    TheGame::getInstance()->getEnv()->addStaticText(L"Name",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabPlayer);

    editBoxPlayerName = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabPlayer->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        true,
        tabPlayer,
        MI_EBPLAYERNAME);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Co-pilot name",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabPlayer);

    editBoxPlayerCoName = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabPlayer->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        true,
        tabPlayer,
        MI_EBPLAYERCONAME);

    line += 20;
    TheGame::getInstance()->getEnv()->addStaticText(L"Team name",
        irr::core::recti(irr::core::position2di(PADDING, line), irr::core::dimension2di(FTW, 16)),
        false,
        false,
        tabPlayer);

    editBoxPlayerTeamName = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(FTW+(PADDING*2), line), irr::core::dimension2di(tabPlayer->getRelativePosition().getSize().Width-(4*PADDING)-LTW-FTW, 16)),
        true,
        tabPlayer,
        MI_EBPLAYERTEAMNAME);


    window->setVisible(false);
}

MenuPageSetup::~MenuPageSetup()
{
    //window->remove();
    menuPageSetup = 0;
    close();
}

bool MenuPageSetup::OnEvent(const irr::SEvent &event)
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
            /*
            case irr::gui::EGET_BUTTON_CLICKED:
            {
                switch (id)
                {
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
            */
            case irr::gui::EGET_SCROLL_BAR_CHANGED:
            {
                switch (id)
                {
                    case MI_SCROLLSUSPENSIONSPRING:
                    {
                        int pos = scrollSuspensionSpring->getPos();
                        dprintf(MY_DEBUG_NOTE, "options::scrollbarsuspensionspring::clicked: pos: %d\n", pos);
                        Player::getInstance()->setSuspensionSpringModifier((float)pos);
                        return true;
                        break;
                    }
                    case MI_SCROLLSUSPENSIONDAMPER:
                    {
                        int pos = scrollSuspensionDamper->getPos();
                        dprintf(MY_DEBUG_NOTE, "options::scrollbarsuspensiondamper::clicked: pos: %d\n", pos);
                        Player::getInstance()->setSuspensionDamperModifier((float)pos);
                        return true;
                        break;
                    }
                    case MI_SCROLLBRAKEBALANCE:
                    {
                        int pos = scrollBrakeBalance->getPos();
                        dprintf(MY_DEBUG_NOTE, "options::scrollbrakebalance::clicked: pos: %d\n", pos);
                        Player::getInstance()->setBrakeBalance(((float)pos)/100.f);
                        return true;
                        break;
                    }
                };
                break;
            }
            case irr::gui::EGET_EDITBOX_CHANGED:
            case irr::gui::EGET_EDITBOX_ENTER:
            {
                switch (id)
                {
                    case MI_EBPLAYERNAME:
                    {
                        std::string newName;
                        WStringConverter::toString(editBoxPlayerName->getText(), newName);
                        Player::getInstance()->getCompetitor()->setName(newName);
                        break;
                    }
                    case MI_EBPLAYERCONAME:
                    {
                        std::string newName;
                        WStringConverter::toString(editBoxPlayerCoName->getText(), newName);
                        Player::getInstance()->getCompetitor()->setCoName(newName);
                        break;
                    }
                    case MI_EBPLAYERTEAMNAME:
                    {
                        std::string newName;
                        WStringConverter::toString(editBoxPlayerTeamName->getText(), newName);
                        Player::getInstance()->getCompetitor()->setTeamName(newName);
                        break;
                    }
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageSetup::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageSetup::open()\n");
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageSetup::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageSetup::close()\n");
    window->setVisible(false);
}

void MenuPageSetup::refresh()
{
    refreshVehicle();
    refreshPlayer();
}

void MenuPageSetup::refreshVehicle()
{
    scrollSuspensionSpring->setPos((int)Player::getInstance()->getSuspensionSpringModifier());
    scrollSuspensionDamper->setPos((int)Player::getInstance()->getSuspensionDamperModifier());
    scrollBrakeBalance->setPos((int)(Player::getInstance()->getBrakeBalance()*100.f));
}

void MenuPageSetup::refreshPlayer()
{
    irr::core::stringw str;
    
    str += Player::getInstance()->getCompetitor()->getName().c_str();
    editBoxPlayerName->setText(str.c_str());

    str = L"";    
    str += Player::getInstance()->getCompetitor()->getCoName().c_str();
    editBoxPlayerCoName->setText(str.c_str());

    str = L"";    
    str += Player::getInstance()->getCompetitor()->getTeamName().c_str();
    editBoxPlayerTeamName->setText(str.c_str());
}

