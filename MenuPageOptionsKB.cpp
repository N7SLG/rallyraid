
#include "MenuPageOptionsKB.h"
#include "MenuPageOptions.h"
#include "MenuPageStage.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"

#include "Settings.h"
#include "EventReceiver.h"
#include <assert.h>


MenuPageOptionsKB* MenuPageOptionsKB::menuPageOptionsKB = 0;

MenuPageOptionsKB::MenuPageOptionsKB()
    : window(0),
      kc(0)
{
    menuPageOptionsKB = this;
    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width/2-62, TheGame::getInstance()->getScreenSize().Height/2-22, TheGame::getInstance()->getScreenSize().Width/2+62, TheGame::getInstance()->getScreenSize().Height/2+22),
        false,
        L"Wait for input",
        0,
        MI_WINDOW);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(2, 22, window->getRelativePosition().getSize().Width/2-1, 42),
        window,
        MI_BUTTONCANCEL,
        L"Cancel");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(window->getRelativePosition().getSize().Width/2+1, 22, window->getRelativePosition().getSize().Width-2, 42),
        window,
        MI_BUTTONCLEAR,
        L"Clear");

    window->setVisible(false);
}

MenuPageOptionsKB::~MenuPageOptionsKB()
{
    //window->remove();
    menuPageOptionsKB = 0;
    close();
}

bool MenuPageOptionsKB::OnEvent(const irr::SEvent &event)
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
                        kc = 0;
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
                    case MI_BUTTONCANCEL:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::cancelbutton::clicked\n");
                        kc = 0;
                        MenuManager::getInstance()->close();
                        return true;
                        break;
                    case MI_BUTTONCLEAR:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::clearbutton::clicked\n");
                        kc = 0;
                        MenuManager::getInstance()->close();
                        MenuPageOptions::menuPageOptions->optionKBClosed();
                        return true;
                        break;
                };
                break;
            }
        };
    }
    return false;
}

void MenuPageOptionsKB::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageOptionsKB::open()\n");
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);

    TheGame::getInstance()->getEventReceiver()->test_kc = 0;
}

void MenuPageOptionsKB::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageOptionsKB::close()\n");
    window->setVisible(false);
}

void MenuPageOptionsKB::keyConfigReceived(KeyConfig* kc)
{
    this->kc = kc;
    MenuManager::getInstance()->close();
    MenuPageOptions::menuPageOptions->optionKBClosed();
}
