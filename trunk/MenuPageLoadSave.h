
#ifndef MENUPAGELOADSAVE_H
#define MENUPAGELOADSAVE_H

#include "MenuPageBase.h"
#include "MenuManager.h"
#include <string>


class Race;
class VehicleType;

class MenuPageLoadSave : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_LOADSAVE,
        MI_BUTTONBACK,
        MI_BUTTONLOADSAVE,

        MI_TABLELOADABLEGAMES,

        MI_EBSAVENAME,

        NUMBER_OF_MENUITEMS
    };


    MenuPageLoadSave();
    virtual ~MenuPageLoadSave();

    static MenuPageLoadSave* menuPageLoadSave;

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshLoadableGames();
    void refreshEditBoxes();

private:
    irr::gui::IGUIImage*    window;
    irr::gui::IGUIStaticText* staticTextName;
    irr::gui::IGUIButton*   buttonLoadSave;
    irr::gui::IGUITable*    tableLoadableGames;
    irr::gui::IGUIEditBox*  editBoxSaveName;
    
    bool                    willOpenOtherWindow;
    bool                    load;
    MenuManager::MenuPageId prevMP;
    std::string             saveName;
    
    
    friend class MenuPageMain;
    friend class MenuPageInGame;
};

#endif // MENUPAGELOADSAVE_H
