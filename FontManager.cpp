
#include "FontManager.h"
#include "TheGame.h"
#include <assert.h>

FontManager* FontManager::fontManager = 0;

void FontManager::initialize()
{
    if (fontManager == 0)
    {
        fontManager = new FontManager();
    }
}

void FontManager::finalize()
{
    if (fontManager)
    {
        delete fontManager;
        fontManager = 0;
    }
}


FontManager::FontManager()
    : fonts(0)
{
    fonts = new irr::gui::IGUIFont*[FONT_COUNT];

    irr::gui::IGUIEnvironment* env = TheGame::getInstance()->getEnv();
    
    fonts[FONT_BUILTIN] = env->getBuiltInFont();
    fonts[FONT_BASE1] = env->getFont("data/fonts/fontlucida.png");
    fonts[FONT_BASE2] = env->getFont("data/fonts/fontcourier.bmp");
    fonts[FONT_BASE3] = env->getFont("data/fonts/fonthaettenschweiler.bmp");
    fonts[FONT_SPECIAL_BIGFONT] = env->getFont("data/fonts/bigfont.png");
/*
    fonts[FONT_COMIC_10PX] = env->getFont("data/fonts/comic_10px_b.xml");
    fonts[FONT_COMIC_12PX] = env->getFont("data/fonts/comic_12px_b.xml");
    fonts[FONT_COMIC_14PX] = env->getFont("data/fonts/comic_14px_b.xml");
    fonts[FONT_COMIC_16PX] = env->getFont("data/fonts/comic_16px_b.xml");
    fonts[FONT_COMIC_18PX] = env->getFont("data/fonts/comic_18px_b.xml");
    fonts[FONT_COMIC_20PX] = env->getFont("data/fonts/comic_20px_b.xml");
    fonts[FONT_COMIC_22PX] = env->getFont("data/fonts/comic_22px_b.xml");
    fonts[FONT_COMIC_24PX] = env->getFont("data/fonts/comic_24px_b.xml");
    fonts[FONT_COMIC_26PX] = env->getFont("data/fonts/comic_26px_b.xml");
    fonts[FONT_COMIC_28PX] = env->getFont("data/fonts/comic_28px_b.xml");
*/
    fonts[FONT_VERDANA_8PX] = env->getFont("data/fonts/verdana_8px_b.xml");
    fonts[FONT_VERDANA_10PX] = env->getFont("data/fonts/verdana_10px_b.xml");
    fonts[FONT_VERDANA_12PX] = env->getFont("data/fonts/verdana_12px_b.xml");
    fonts[FONT_VERDANA_14PX] = env->getFont("data/fonts/verdana_14px_b.xml");
    fonts[FONT_VERDANA_16PX] = env->getFont("data/fonts/verdana_16px_b.xml");
    fonts[FONT_VERDANA_18PX] = env->getFont("data/fonts/verdana_18px_b.xml");
    fonts[FONT_VERDANA_20PX] = env->getFont("data/fonts/verdana_20px_b.xml");
    fonts[FONT_VERDANA_22PX] = env->getFont("data/fonts/verdana_22px_b.xml");
    fonts[FONT_VERDANA_24PX] = env->getFont("data/fonts/verdana_24px_b.xml");
    fonts[FONT_VERDANA_26PX] = env->getFont("data/fonts/verdana_26px_b.xml");
    fonts[FONT_VERDANA_28PX] = env->getFont("data/fonts/verdana_28px_b.xml");

    fonts[FONT_VERDANA_18PX_BORDER] = env->getFont("data/fonts/verdana_18px_b_border.xml");
    fonts[FONT_VERDANA_22PX_BORDER] = env->getFont("data/fonts/verdana_22px_b_border.xml");
}

FontManager::~FontManager()
{
    if (fonts)
    {
        delete [] fonts;
        fonts = 0;
    }
}
