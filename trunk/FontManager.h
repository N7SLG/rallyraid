
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <irrlicht.h>

class FontManager
{
public:
    enum FontType {
        FONT_BUILTIN = 0,
        FONT_BASE1,
        FONT_BASE2,
        FONT_BASE3,

        FONT_SMALL,
        FONT_NORMAL,
        FONT_LARGE,
        FONT_EXTRALARGE,

        FONT_SMALLBOLD,
        FONT_NORMALBOLD,
        FONT_LARGEBOLD,
        FONT_EXTRALARGEBOLD,

        FONT_SPECIALSMALL,
        FONT_SPECIALNORMAL,
        FONT_SPECIALLARGE,
        FONT_SPECIALEXTRALARGE,

        FONT_SPECIALSMALLBOLD,
        FONT_SPECIALNORMALBOLD,
        FONT_SPECIALLARGEBOLD,
        FONT_SPECIALEXTRALARGEBOLD,

        FONT_SPECIAL14,
        FONT_SPECIAL16,
        FONT_SPECIAL18,

        FONT_COUNT,
    };

public:
    static void initialize();
    static void finalize();
    
    static FontManager* getInstance() {return fontManager;}

private:
    static FontManager* fontManager;

private:
    FontManager();
    ~FontManager();

public:
    irr::gui::IGUIFont* getFont(FontType fontType); // inline

private:
    irr::gui::IGUIFont** fonts;

};

inline irr::gui::IGUIFont* FontManager::getFont(FontManager::FontType fontType)
{
    return fonts[fontType];
}

#endif // FONTMANAGER_H
