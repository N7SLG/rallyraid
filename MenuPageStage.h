
#ifndef MENUPAGESTAGE_H
#define MENUPAGESTAGE_H

#include "MenuPageBase.h"
#include "MenuManager.h"
#include "ItinerManager.h"


class Race;
class Stage;
class VehicleType;
class ScreenQuad;

class MenuPageStage : public MenuPageBase
{
public:
    enum MenuItemId
    {
        MI_WINDOW = MAX_MENU_ITEMS * MenuManager::MP_STAGE,
        MI_BUTTONSTART,
        MI_BUTTONBACK,

        MI_SCROLLROADBOOK,

        NUMBER_OF_MENUITEMS
    };


    MenuPageStage();
    virtual ~MenuPageStage();

    static MenuPageStage* menuPageStage;
    
    bool isVisible(); // inline
    void render();

protected:
    virtual bool OnEvent (const irr::SEvent &event);
    virtual void open();
    virtual void close();

private:
    void refresh();
    void refreshItiner();
    void setVisible(bool visible);
    void updateRoadBook();

private:
    irr::gui::IGUIImage*    window;
    irr::gui::IGUIStaticText*   staticTextStageName;
    irr::gui::IGUIStaticText*   staticTextStageDescription;
    ScreenQuad*             bgQuad;
    //irr::gui::IGUIStaticText* staticTextRaceData;
    bool                    willOpenOtherWindow;
    bool                    visible;
    
    Race*                   selectedRace;
    Stage*                  selectedStage;
    VehicleType*            selectedVehicleType;
    ItinerManager::itinerPointList_t::const_iterator itinerIt;

    class RoadBookEntry
    {
    public:
        irr::gui::IGUIStaticText*   numText;
        irr::gui::IGUIStaticText*   globalDistanceText;
        irr::gui::IGUIStaticText*   localDistanceText;
        irr::gui::IGUIStaticText*   noteText;
        ScreenQuad*                 itinerQuad;
        ScreenQuad*                 itiner2Quad;
    };
    ScreenQuad*     roadBookBGQuad;
    RoadBookEntry   roadBookEntries[6];

    ScreenQuad*     stageImageQuad;
    irr::gui::IGUIScrollBar*    roadBookScrollBar;
    
    
    friend class MenuPageMain;
};


inline bool MenuPageStage::isVisible()
{
    return visible;
}

#endif // MENUPAGESTAGE_H
