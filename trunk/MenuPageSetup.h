
#ifndef MENUPAGESETUP_H
#define MENUPAGESETUP_H

#include "MenuPageBase.h"
#include "MenuManager.h"
#include <map>


class StageState;

class MenuPageSetup : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_SETUP,

        MI_TABLEKB,

        MI_SCROLLSUSPENSIONSPRING,
        MI_SCROLLSUSPENSIONDAMPER,
        MI_SCROLLBRAKEBALANCE,
        
        MI_EBPLAYERNAME,
        MI_EBPLAYERCONAME,
        MI_EBPLAYERTEAMNAME,

        NUMBER_OF_MENUITEMS
    };


    MenuPageSetup();
    virtual ~MenuPageSetup();

    static MenuPageSetup* menuPageSetup;

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshVehicle();
    void refreshPlayer();

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUIScrollBar* scrollSuspensionSpring;
    irr::gui::IGUIScrollBar* scrollSuspensionDamper;
    irr::gui::IGUIScrollBar* scrollBrakeBalance;
    irr::gui::IGUIEditBox*  editBoxPlayerName;
    irr::gui::IGUIEditBox*  editBoxPlayerCoName;
    irr::gui::IGUIEditBox*  editBoxPlayerTeamName;
};

#endif // MENUPAGESETUP_H
