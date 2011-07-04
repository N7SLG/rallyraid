
#ifndef MENUPAGEEDITORDAY_H
#define MENUPAGEEDITORDAY_H

#include "MenuPageBase.h"
#include "MenuManager.h"

class MenuPageEditorDay : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_EDITORDAY,
        MI_BUTTONREFRESH,
        MI_BUTTONSAVE,
        MI_BUTTONCREATESTAGE,
        MI_BUTTONCREATEROAD,
        MI_TABCONTROL,

        MI_TABSTAGES,
        MI_TABGLOBALOBJECTS,
        MI_TABROADS,

        MI_TABLESTAGES,
        MI_TABLEGLOBALOBJECTS,
        MI_TABLEROADS,

        MI_EBLONGNAME,
        MI_EBSHORTDESCRIPTION,
        MI_EBNEWSTAGENAME,
        MI_EBNEWROADFILENAME,
        MI_EBNEWROADNAME,
        MI_EBNEWROADDATAFILENAME,

        NUMBER_OF_MENUITEMS
    };


    MenuPageEditorDay();
    virtual ~MenuPageEditorDay();

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshStages();
    void refreshGlobalObjects();
    void refreshEditBoxes();
    void refreshRoads();
    void refreshRoadEditBoxes(const wchar_t* newRoadName = L"");

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUITable*    tableStages;
    irr::gui::IGUITable*    tableGlobalObjects;
    irr::gui::IGUITable*    tableRoads;
    irr::gui::IGUIEditBox*  editBoxLongName;
    irr::gui::IGUIEditBox*  editBoxShortDescription;
    irr::gui::IGUIEditBox*  editBoxNewStage;
    irr::gui::IGUIEditBox*  editBoxNewRoadFilename;
    irr::gui::IGUIEditBox*  editBoxNewRoadName;
    irr::gui::IGUIEditBox*  editBoxNewRoadDataFilename;
};

#endif // MENUPAGEEDITORDAY_H
