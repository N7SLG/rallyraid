
#ifndef MENUPAGEOPTIONSKB_H
#define MENUPAGEOPTIONSKB_H

#include "MenuPageBase.h"
#include "MenuManager.h"


class KeyConfig;

class MenuPageOptionsKB : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_OPTIONSKB,
        MI_BUTTONCANCEL,
        MI_BUTTONCLEAR,

        NUMBER_OF_MENUITEMS
    };


    MenuPageOptionsKB();
    virtual ~MenuPageOptionsKB();

    static MenuPageOptionsKB* menuPageOptionsKB;

public:
    void keyConfigReceived(KeyConfig* kc);

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    irr::gui::IGUIWindow*   window;
    KeyConfig*              kc;


    friend class MenuPageOptions;
};

#endif // MENUPAGEOPTIONSKB_H
