
#ifndef MENUPAGEEDITORROAD_H
#define MENUPAGEEDITORROAD_H

#include "MenuPageBase.h"
#include "MenuManager.h"

class MenuPageEditorRoad : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_EDITORROAD,
        MI_BUTTONREFRESH,
        MI_BUTTONSAVE,
        MI_BUTTONCOLOR,
        MI_TABCONTROL,

        MI_TABPOINTS,

        MI_TABLEPOINTS,

        MI_EBRADIUS,
        MI_EBRED,
        MI_EBGREEN,
        MI_EBBLUE,

        NUMBER_OF_MENUITEMS
    };


    MenuPageEditorRoad();
    virtual ~MenuPageEditorRoad();

    static MenuPageEditorRoad* menuPageEditorRoad;

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshPoints();

public:
    void refreshColor();

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUITable*    tablePoints;
    irr::gui::IGUIEditBox*  editBoxRadius;
    irr::gui::IGUIEditBox*  editBoxRed;
    irr::gui::IGUIEditBox*  editBoxGreen;
    irr::gui::IGUIEditBox*  editBoxBlue;
};

#endif // MENUPAGEEDITORROAD_H
