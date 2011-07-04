
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
/*
    fonts[FONT_SMALL] = env->getFont("data/fonts/arial_8px.xml");
    fonts[FONT_NORMAL] = env->getFont("data/fonts/arial_10px.xml");
    fonts[FONT_LARGE] = env->getFont("data/fonts/arial_12px.xml");
    fonts[FONT_EXTRALARGE] = env->getFont("data/fonts/arial_14px.xml");
*/
    fonts[FONT_SMALL] = env->getFont("data/fonts/verdana_8px.xml");
    fonts[FONT_NORMAL] = env->getFont("data/fonts/verdana_10px.xml");
    fonts[FONT_LARGE] = env->getFont("data/fonts/verdana_12px.xml");
    fonts[FONT_EXTRALARGE] = env->getFont("data/fonts/verdana_14px.xml");

    fonts[FONT_SMALLBOLD] = env->getFont("data/fonts/verdana_8px_b.xml");
    fonts[FONT_NORMALBOLD] = env->getFont("data/fonts/verdana_10px_b.xml");
    fonts[FONT_LARGEBOLD] = env->getFont("data/fonts/verdana_12px_b.xml");
    fonts[FONT_EXTRALARGEBOLD] = env->getFont("data/fonts/verdana_14px_b.xml");

    fonts[FONT_SPECIALSMALL] = env->getFont("data/fonts/comic_8px.xml");
    fonts[FONT_SPECIALNORMAL] = env->getFont("data/fonts/comic_10px.xml");
    fonts[FONT_SPECIALLARGE] = env->getFont("data/fonts/comic_12px.xml");
    fonts[FONT_SPECIALEXTRALARGE] = env->getFont("data/fonts/comic_14px.xml");

    fonts[FONT_SPECIALSMALLBOLD] = env->getFont("data/fonts/comic_8px_b.xml");
    fonts[FONT_SPECIALNORMALBOLD] = env->getFont("data/fonts/comic_10px_b.xml");
    fonts[FONT_SPECIALLARGEBOLD] = env->getFont("data/fonts/comic_12px_b.xml");
    fonts[FONT_SPECIALEXTRALARGEBOLD] = env->getFont("data/fonts/comic_14px_b.xml");

    fonts[FONT_SPECIAL14] = env->getFont("data/fonts/verdana_14px_b_a.xml");
    fonts[FONT_SPECIAL16] = env->getFont("data/fonts/verdana_16px_b_a.xml");
    fonts[FONT_SPECIAL18] = env->getFont("data/fonts/verdana_18px_b_a.xml");

    assert(fonts[FONT_NORMAL]);
}

FontManager::~FontManager()
{
    if (fonts)
    {
        delete [] fonts;
        fonts = 0;
    }
}
