
#include "MenuPageLoadSave.h"
#include "MenuPageStage.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"

#include "RaceManager.h"
#include "Race.h"
#include "Settings.h"
#include "Day.h"
#include "Stage.h"
#include "GamePlay.h"
#include "FontManager.h"
#include "MessageManager.h"
#include "Hud.h"
#include <assert.h>


MenuPageLoadSave* MenuPageLoadSave::menuPageLoadSave = 0;

MenuPageLoadSave::MenuPageLoadSave()
    : window(0),
      staticTextName(0),
      buttonLoadSave(0),
      tableLoadableGames(0),
      editBoxSaveName(0),
      willOpenOtherWindow(false),
      load(true),
      prevMP(MenuManager::MP_MAIN),
      saveName()
{
    menuPageLoadSave = this;
    window = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(0, 0, TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        0,
        MI_WINDOW);
    window->setScaleImage(true);
    window->setImage(TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,60,90,80),
        window,
        MI_BUTTONBACK,
        L"Back");

    buttonLoadSave = TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,90,90,110),
        window,
        MI_BUTTONLOADSAVE,
        load?L"Load Game" : L"Save Game");

    staticTextName = TheGame::getInstance()->getEnv()->addStaticText(load ? L"Load Game" : L"Save Game",
        irr::core::recti(window->getRelativePosition().getSize().Width/2 - 400,54,window->getRelativePosition().getSize().Width/2 + 400,88),
        false, false, window, 0, false);
    staticTextName->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL18));
    staticTextName->setOverrideColor(irr::video::SColor(255, 255, 255, 255));
    staticTextName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);


    // ----------------------------
    // Loadable games
    // ----------------------------
    tableLoadableGames = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/3, (window->getRelativePosition().getSize().Height)/2), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3,(window->getRelativePosition().getSize().Height)/2-2)),
        window,
        MI_TABLELOADABLEGAMES,
        true);

    tableLoadableGames->addColumn(L"Name");
    tableLoadableGames->setColumnWidth(0, ((tableLoadableGames->getRelativePosition().getSize().Width-16))/3);
    tableLoadableGames->addColumn(L"Race");
    tableLoadableGames->setColumnWidth(1, ((tableLoadableGames->getRelativePosition().getSize().Width-16)*2)/3);

    editBoxSaveName = TheGame::getInstance()->getEnv()->addEditBox(L"new save name",
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/3, (window->getRelativePosition().getSize().Height)/2-22), irr::core::dimension2di(window->getRelativePosition().getSize().Width/3, 20)),
        true,
        window,
        MI_EBSAVENAME);
    editBoxSaveName->setVisible(false);

    window->setVisible(false);
}

MenuPageLoadSave::~MenuPageLoadSave()
{
    //window->remove();
    menuPageLoadSave = 0;
    close();
}

bool MenuPageLoadSave::OnEvent(const irr::SEvent &event)
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
                    case MI_BUTTONBACK:
                        dprintf(MY_DEBUG_NOTE, "LoadSavemenu::backbutton::clicked\n");
                        willOpenOtherWindow = true;
                        MenuManager::getInstance()->close();
                        MenuManager::getInstance()->open(prevMP);
                        return true;
                        break;
                    case MI_BUTTONLOADSAVE:
                        dprintf(MY_DEBUG_NOTE, "LoadSavemenu::optionsbutton::clicked\n");
                        if (load)
                        {
                            if (!saveName.empty())
                            {
                                willOpenOtherWindow = false;
                                if (GamePlay::getInstance()->loadGame(saveName))
                                {
                                    MenuManager::getInstance()->close();
                                }
                                else
                                {
                                    MessageManager::getInstance()->addText(L"Failed to load game!", 2);
                                }
                            }
                            else
                            {
                                MessageManager::getInstance()->addText(L"First select a game to load.", 2);
                            }
                        }
                        else
                        {
                            if (!saveName.empty())
                            {
                                if (GamePlay::getInstance()->saveGame(saveName))
                                {
                                    MessageManager::getInstance()->addText(L"Game saved.", 2);
                                    refreshLoadableGames();
                                }
                                else
                                {
                                    MessageManager::getInstance()->addText(L"Failed to save game!", 2);
                                }
                            }
                            else
                            {
                                MessageManager::getInstance()->addText(L"First give a name to save.", 2);
                            }
                        }
                        return true;
                        break;
                };
                break;
            }
            
            case irr::gui::EGET_TABLE_CHANGED:
            {
                switch (id)
                {
                    case MI_TABLELOADABLEGAMES:
                        WStringConverter::toString(tableLoadableGames->getCellText(tableLoadableGames->getSelected(), 0), saveName);
                        refreshEditBoxes();
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_EDITBOX_CHANGED:
            case irr::gui::EGET_EDITBOX_ENTER:
            {
                switch (id)
                {
                    case MI_EBSAVENAME:
                        WStringConverter::toString(editBoxSaveName->getText(), saveName);
                        break;
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageLoadSave::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageLoadSave::open()\n");
    if (load)
    {
        editBoxSaveName->setVisible(false);
        saveName = "";
        buttonLoadSave->setText(L"Load Game");
        staticTextName->setText(L"Load Game");
    }
    else
    {
        
        if (saveName.empty())
        {
            saveName = "new save name";
        }
        
        editBoxSaveName->setVisible(true);
        buttonLoadSave->setText(L"Save Game");
        staticTextName->setText(L"Save Game");
    }
    refresh();
    window->setVisible(true);
    if (load)
    {
        TheGame::getInstance()->getEnv()->setFocus(tableLoadableGames);
    }
    else
    {
        TheGame::getInstance()->getEnv()->setFocus(editBoxSaveName);
    }
    
    // stop necessarry elements in the game
    TheGame::getInstance()->setInGame(false);
    Hud::getInstance()->setVisible(false);
}

void MenuPageLoadSave::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageLoadSave::close()\n");
    window->setVisible(false);

    // start necessarry elements in the game if returns
    if (!willOpenOtherWindow)
    {
        TheGame::getInstance()->setInGame(true);
        Hud::getInstance()->setVisible(true);
    }
    willOpenOtherWindow = false;
}

void MenuPageLoadSave::refresh()
{
    refreshLoadableGames();
    refreshEditBoxes();
}

void MenuPageLoadSave::refreshLoadableGames()
{
    tableLoadableGames->clearRows();

    GamePlay::getInstance()->refreshLoadableGames();
    const GamePlay::loadableGames_t& loadableGames = GamePlay::getInstance()->loadableGames;
    unsigned int i = 0;
    for (GamePlay::loadableGames_t::const_iterator it = loadableGames.begin();
         it != loadableGames.end();
         it++, i++)
    {
        irr::core::stringw str;
        
        tableLoadableGames->addRow(i);

        str = L"";
        str += it->first.c_str();
        tableLoadableGames->setCellText(i, 0, str.c_str());

        str = L"";
        str += it->second.c_str();
        tableLoadableGames->setCellText(i, 1, str.c_str());
    }
}

void MenuPageLoadSave::refreshEditBoxes()
{
    irr::core::stringw str = saveName.c_str();
    editBoxSaveName->setText(str.c_str());
}
