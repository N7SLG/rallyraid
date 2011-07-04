
#ifndef MENUPAGEEDITORSTAGE_H
#define MENUPAGEEDITORSTAGE_H

#include "MenuPageBase.h"
#include "MenuManager.h"

class MenuPageEditorStage : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_EDITORSTAGE,
        MI_BUTTONREFRESH,
        MI_BUTTONSAVE,
        MI_BUTTONCREATEROAD,
        MI_BUTTONHMSET,
        MI_TABCONTROL,

        MI_TABGLOBALOBJECTS,
        MI_TABROADS,
        MI_TABITINER,
        MI_TABAI,
        MI_TABWP,
        MI_TABHM,

        MI_TABLEGLOBALOBJECTS,
        MI_TABLEROADS,
        MI_TABLEITINER,
        MI_TABLEAI,
        MI_TABLEWP,
        MI_TABLEHM,

        MI_EBLONGNAME,
        MI_EBSHORTDESCRIPTION,
        MI_EBNEWROADFILENAME,
        MI_EBNEWROADNAME,
        MI_EBNEWROADDATAFILENAME,
        MI_EBSTAGETIME,
        MI_EBIMAGE,
        MI_EBHMHEIGHT,
        MI_EBHMRADIUS,

        NUMBER_OF_MENUITEMS
    };


    MenuPageEditorStage();
    virtual ~MenuPageEditorStage();

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshGlobalObjects();
    void refreshEditBoxes();
    void refreshRoads();
    void refreshRoadEditBoxes(const wchar_t* newRoadName = L"");
    void refreshItiner();
    void refreshAI();
    void refreshWP();
    void refreshHM();

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUITable*    tableGlobalObjects;
    irr::gui::IGUITable*    tableRoads;
    irr::gui::IGUITable*    tableItiner;
    irr::gui::IGUITable*    tableAI;
    irr::gui::IGUITable*    tableWP;
    irr::gui::IGUITable*    tableHM;
    irr::gui::IGUIEditBox*  editBoxLongName;
    irr::gui::IGUIEditBox*  editBoxShortDescription;
    irr::gui::IGUIEditBox*  editBoxNewRoadFilename;
    irr::gui::IGUIEditBox*  editBoxNewRoadName;
    irr::gui::IGUIEditBox*  editBoxNewRoadDataFilename;
    irr::gui::IGUIEditBox*  editBoxStageTime;
    irr::gui::IGUIEditBox*  editBoxImage;
    irr::gui::IGUIEditBox*  editBoxHMHeight;
    irr::gui::IGUIEditBox*  editBoxHMRadius;
};

#endif // MENUPAGEEDITORSTAGE_H
