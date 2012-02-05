
#ifndef MENUPAGEEDITORRACE_H
#define MENUPAGEEDITORRACE_H

#include "MenuPageBase.h"
#include "MenuManager.h"

class MenuPageEditorRace : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_EDITORRACE,
        MI_BUTTONREFRESH,
        MI_BUTTONSAVE,
        MI_BUTTONCREATEDAY,
        MI_BUTTONCREATEROAD,
        MI_TABCONTROL,

        MI_TABDAYS,
        MI_TABCOMPETITORS,
        MI_TABGLOBALOBJECTS,
        MI_TABROADS,

        MI_TABLEDAYS,
        MI_TABLECOMPETITORS,
        MI_TABLEGLOBALOBJECTS,
        MI_TABLEROADS,

        MI_EBLONGNAME,
        MI_EBSHORTDESCRIPTION,
        MI_EBNEWDAYNAME,
        MI_EBNEWROADFILENAME,
        MI_EBNEWROADNAME,
        MI_EBNEWROADDATAFILENAME,
        MI_EBNEWROADHMRADIUS,
        MI_EBNEWROADHM,
        MI_EBIMAGE,

        MI_CBROADHMFIX,

        NUMBER_OF_MENUITEMS
    };


    MenuPageEditorRace();
    virtual ~MenuPageEditorRace();

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshDays();
    void refreshCompetitors();
    void refreshGlobalObjects();
    void refreshEditBoxes();
    void refreshRoads();
    void refreshRoadEditBoxes(const wchar_t* newRoadName = L"");

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUITable*    tableDays;
    irr::gui::IGUITable*    tableCompetitors;
    irr::gui::IGUITable*    tableGlobalObjects;
    irr::gui::IGUITable*    tableRoads;
    irr::gui::IGUIEditBox*  editBoxLongName;
    irr::gui::IGUIEditBox*  editBoxShortDescription;
    irr::gui::IGUIEditBox*  editBoxNewDay;
    irr::gui::IGUIEditBox*  editBoxNewRoadFilename;
    irr::gui::IGUIEditBox*  editBoxNewRoadName;
    irr::gui::IGUIEditBox*  editBoxNewRoadDataFilename;
    irr::gui::IGUIEditBox*  editBoxNewRoadHMRadius;
    irr::gui::IGUIEditBox*  editBoxNewRoadHM;
    irr::gui::IGUIEditBox*  editBoxImage;
    irr::gui::IGUICheckBox* cbRoadHMFix;
};

#endif // MENUPAGEEDITORRACE_H
