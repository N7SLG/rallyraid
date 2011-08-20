
#include "MenuPageBase.h"
#include "MenuManager.h"
#include "TheGame.h"

#include "MenuPageEditor.h"
#include "MenuPageEditorRace.h"
#include "MenuPageEditorDay.h"
#include "MenuPageEditorStage.h"
#include "MenuPageEditorRoad.h"
#include "MenuPageMain.h"
#include "MenuPageStage.h"
#include "MenuPageInGame.h"
#include "MenuPageOptions.h"
#include "MenuPageOptionsKB.h"
#include "MenuPageLoadSave.h"

#include <string.h>

class EmptyEventReceiver : public irr::IEventReceiver
{
public:
    EmptyEventReceiver()
    {
    }

    virtual ~EmptyEventReceiver()
    {
    }

private:
    virtual bool OnEvent(const irr::SEvent &event)
    {
        //printf("empty event: %u\n", event.EventType);
        if (event.EventType == irr::EET_GUI_EVENT && (int)event.GUIEvent.EventType == 0)
        {
            //if (firstAction)
            //{
            //    firstAction = false;
            //}
            //else
            {
                //printf("ee gui action\n");
                MenuPageEditor::action();
            }
        }
        return true;
    }
    static bool firstAction;
};
bool EmptyEventReceiver::firstAction = true;


MenuManager* MenuManager::menuManager;

void MenuManager::initialize()
{
    if (menuManager == 0)
    {
        menuManager = new MenuManager();
    }
}

void MenuManager::finalize()
{
    if (menuManager)
    {
        delete menuManager;
        menuManager = 0;
    }
}

MenuManager::MenuManager()
    : currentMenuPage(0), menuInput(false), eer(new EmptyEventReceiver())
{
    menuManager = this;
    memset(menuPages, 0, sizeof(menuPages));
    menuPages[MP_EDITOR] = new MenuPageEditor();
    menuPages[MP_EDITORRACE] = new MenuPageEditorRace();
    menuPages[MP_EDITORDAY] = new MenuPageEditorDay();
    menuPages[MP_EDITORSTAGE] = new MenuPageEditorStage();
    menuPages[MP_EDITORROAD] = new MenuPageEditorRoad();
    menuPages[MP_MAIN] = new MenuPageMain();
    menuPages[MP_STAGE] = new MenuPageStage();
    menuPages[MP_INGAME] = new MenuPageInGame();
    menuPages[MP_OPTIONS] = new MenuPageOptions();
    menuPages[MP_OPTIONSKB] = new MenuPageOptionsKB();
    menuPages[MP_LOADSAVE] = new MenuPageLoadSave();
}

MenuManager::~MenuManager()
{
    for (unsigned int i = 0; i < NUMBER_OF_MENUPAGES; i++)
    {
        if (menuPages[i])
        {
            delete menuPages[i];
            menuPages[i] = 0;
        }
    }
}

void MenuManager::open(MenuPageId menuPageId)
{
    dprintf(MY_DEBUG_NOTE, "MenuManager::open(): id: %d, p: %p, opened: %d\n", (int)menuPageId, menuPages[menuPageId], menuPages[menuPageId]?menuPages[menuPageId]->isOpened():-1);
    if (menuPages[menuPageId] && !menuPages[menuPageId]->isOpened())
    {
        if (menuPages[menuPageId]->openMenu(currentMenuPage))
        {
            currentMenuPage = menuPages[menuPageId];
            refreshEventReceiver();
        }
    }
}

void MenuManager::close()
{
    if (currentMenuPage)
    {
        currentMenuPage = currentMenuPage->closeMenu();
        TheGame::getInstance()->getEnv()->setFocus(0);
        refreshEventReceiver();
    }
}

void MenuManager::closeAll()
{
    while (currentMenuPage)
    {
        currentMenuPage = currentMenuPage->closeMenu();
    }
}

void MenuManager::refreshEventReceiver()
{
    TheGame::getInstance()->getEnv()->setUserEventReceiver(currentMenuPage);
    menuInput = (currentMenuPage != 0);
    TheGame::getInstance()->getCamera()->setInputReceiverEnabled(!menuInput);
    //printf("set event receiver: %p, menuInput %d\n", currentMenuPage, menuInput);
}

void MenuManager::clearEventReceiver()
{
    TheGame::getInstance()->getEnv()->setUserEventReceiver(eer);
    TheGame::getInstance()->getDevice()->getCursorControl()->setPosition((int)(TheGame::getInstance()->getScreenSize().Width / 2), (int)(TheGame::getInstance()->getScreenSize().Height / 2));
    menuInput = false;
    TheGame::getInstance()->getCamera()->setInputReceiverEnabled(true);
}
