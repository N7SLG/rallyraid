
#ifndef MENUPAGEOPTIONS_H
#define MENUPAGEOPTIONS_H

#include "MenuPageBase.h"
#include "MenuManager.h"
#include <map>


class StageState;

class MenuPageOptions : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_OPTIONS,
        MI_BUTTONSAVE,
        MI_BUTTONCENTER,
        MI_BUTTONPRIMARY,
        MI_BUTTONSECONDARY,

        MI_TABLEKB,

        MI_COMBOBOXDIFFICULTY,
        MI_COMBOBOXDRIVERTYPE,
        MI_COMBOBOXRESOLUTION,
        MI_COMBOBOXDISPLAYBITS,

        MI_CBDESKTOPRESOLUTION,
        MI_CBFULLSCREEN,
        MI_CBVSYNC,
        MI_CBSCANJOYSTICK,
        MI_CBSHOWNAMES,
        MI_CBNAVIGATIONASSISTANT,
        MI_CBMANUALSHIFTING,
        MI_CBSEQUENTIALSHIFTING,
        MI_CBLINEARSTEERING,
        MI_CBDAMAGE,

        NUMBER_OF_MENUITEMS
    };


    MenuPageOptions();
    virtual ~MenuPageOptions();

    static MenuPageOptions* menuPageOptions;

public:
    void optionKBClosed();

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshGame();
    void refreshGraphics();
    void refreshKB();

private:
    irr::gui::IGUIWindow*   window;
    irr::gui::IGUITable*    tableKB;
    irr::gui::IGUIComboBox* comboBoxDifficulty;
    irr::gui::IGUIComboBox* comboBoxDriverType;
    irr::gui::IGUIComboBox* comboBoxResolution;
    irr::gui::IGUIComboBox* comboBoxDisplayBits;
    irr::gui::IGUICheckBox* cbDesktopResolution;
    irr::gui::IGUICheckBox* cbFullScreen;
    irr::gui::IGUICheckBox* cbVsync;
    irr::gui::IGUICheckBox* cbScanJoystick;
    irr::gui::IGUICheckBox* cbShowNames;
    irr::gui::IGUICheckBox* cbNavigationAssistant;
    irr::gui::IGUICheckBox* cbManualShifting;
    irr::gui::IGUICheckBox* cbSequentialShifting;
    irr::gui::IGUICheckBox* cbLinearSteering;
    irr::gui::IGUICheckBox* cbDamage;

    std::map<int, int>      resolutionMap;
    int                     lastKeyName;
    bool                    primary;
};

#endif // MENUPAGEOPTIONS_H
