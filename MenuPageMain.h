
#ifndef MENUPAGEMAIN_H
#define MENUPAGEMAIN_H

#include "MenuPageBase.h"
#include "MenuManager.h"


class Race;
class VehicleType;

class MenuPageMain : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_MAIN,
        MI_BUTTONSTART,
        MI_BUTTONLOAD,
        MI_BUTTONOPTIONS,

        MI_TABLERACES,
        MI_TABLEVEHICLES,

        MI_CBEDITORMODE,

        MI_STRACEDATA,
        MI_STVEHICLEDATA,

        NUMBER_OF_MENUITEMS
    };


    MenuPageMain();
    virtual ~MenuPageMain();

    static MenuPageMain* menuPageMain;

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshRaceData(Race* race);
    void refreshVehicleData(VehicleType* vehicleType);

private:
    irr::gui::IGUIImage*    window;
    irr::gui::IGUIStaticText* staticTextGameName;
    irr::gui::IGUITable*    tableRaces;
    irr::gui::IGUITable*    tableVehicles;
    irr::gui::IGUICheckBox* checkBoxEditor;
    irr::gui::IGUIStaticText* staticTextRaceData;
    irr::gui::IGUIStaticText* staticTextVehicleData;
    
    Race*                   selectedRace;
    VehicleType*            selectedVehicleType;
    
    bool                    willOpenOtherWindow;
};

#endif // MENUPAGEMAIN_H
