
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
        NUMBER_OF_MENUPAGES
    };

private:
    MenuManager();
    ~MenuManager();

public:
    void refreshEventReceiver();
    void clearEventReceiver();
    bool getMenuInput();    // inline

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
};


inline bool MenuManager::isInMenu()
{
    return (currentMenuPage != 0);
}

inline bool MenuManager::getMenuInput()
{
    return menuInput;
}

#endif // MENUMANAGER_H
