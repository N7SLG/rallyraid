
#ifndef MENUMANAGER_H
#define MENUMANAGER_H


#define MAX_MENU_ITEMS 100

class MenuPageBase;
class EmptyEventReceiver;

class MenuManager
{
public:
    static void initialize();
    static void finalize();
    
    static MenuManager* getInstance() {return menuManager;}

private:
    static MenuManager* menuManager;

public:
    enum MenuPageId
    {
        MP_EDITOR = 0,
        MP_EDITORRACE,
        MP_EDITORDAY,
        MP_EDITORSTAGE,
        MP_EDITORROAD,
        MP_MAIN,
        MP_STAGE,
        MP_INGAME,
        MP_OPTIONS,
        MP_OPTIONSKB,
        MP_LOADSAVE,
        MP_SETUP,
        NUMBER_OF_MENUPAGES
    };

private:
    MenuManager();
    ~MenuManager();

public:
    void refreshEventReceiver();
    void clearEventReceiver();
    bool getMenuInput();    // inline
    irr::IEventReceiver* getCurrentEventReceiver(); // inline

public:
    
    void open(MenuPageId menuPageId);
    void close();
    void closeAll();

    bool isInMenu();        // inline

private:
    MenuPageBase*   currentMenuPage;
    MenuPageBase*   menuPages[NUMBER_OF_MENUPAGES];
    bool            menuInput;
    EmptyEventReceiver* eer;
    irr::IEventReceiver* currentEventReceiver;
};


inline bool MenuManager::isInMenu()
{
    return (currentMenuPage != 0);
}

inline bool MenuManager::getMenuInput()
{
    return menuInput;
}

inline irr::IEventReceiver* MenuManager::getCurrentEventReceiver()
{
    return currentEventReceiver;
}

#endif // MENUMANAGER_H
