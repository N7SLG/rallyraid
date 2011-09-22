
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

        FONT_SPECIAL_BIGFONT,

        FONT_COMIC_10PX,
        FONT_COMIC_12PX,
        FONT_COMIC_14PX,
        FONT_COMIC_16PX,
        FONT_COMIC_18PX,
        FONT_COMIC_20PX,
        FONT_COMIC_22PX,
        FONT_COMIC_24PX,
        FONT_COMIC_26PX,
        FONT_COMIC_28PX,

        FONT_VERDANA_8PX,
        FONT_VERDANA_10PX,
        FONT_VERDANA_12PX,
        FONT_VERDANA_14PX,
        FONT_VERDANA_16PX,
        FONT_VERDANA_18PX,
        FONT_VERDANA_20PX,
        FONT_VERDANA_22PX,
        FONT_VERDANA_24PX,
        FONT_VERDANA_26PX,
        FONT_VERDANA_28PX,

        FONT_VERDANA_18PX_BORDER,
        FONT_VERDANA_22PX_BORDER,

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
