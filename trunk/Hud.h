#ifndef HUD_H
#define HUD_H

#include <irrlicht.h>

class ScreenQuad;

class Hud
{
public:
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
public:
    static void initialize();
    static void finalize();
    
    static Hud* getInstance() {return hud;}

private:
    static Hud* hud;

public:
    Hud();
    ~Hud();
    
    void setVisible(bool newVisible);
    bool getVisible(); // inline
    
    void preRender(float p_angle);
    void render();

    void updateRoadBook();
    
    irr::gui::IGUIStaticText* getEditorText(); // inline

private:
    bool            visible;
    ScreenQuad*     miniMapQuad;
    ScreenQuad*     compassQuad;
    ScreenQuad*     compassWPQuad;
    ScreenQuad*     tripMasterQuad;
    ScreenQuad*     roadBookBGQuad;

    irr::gui::IGUIStaticText*   compassText;
    irr::gui::IGUIStaticText*   tmPartText;
    irr::gui::IGUIStaticText*   tmTotalText;
    irr::gui::IGUIStaticText*   speedText;
    irr::gui::IGUIStaticText*   stageTimeText;
    irr::gui::IGUIStaticText*   editorText;

    RoadBookEntry roadBookEntries[4];
};

inline bool Hud::getVisible()
{
    return visible;
}

inline irr::gui::IGUIStaticText* Hud::getEditorText()
{
    return editorText;
}

#endif // HUD_H
