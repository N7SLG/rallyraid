
#include "MenuPageBase.h"


MenuPageBase::MenuPageBase()
    : parentMenuPage(0),
      opened(false)
{
}

MenuPageBase::~MenuPageBase()
{
}

bool MenuPageBase::openMenu(MenuPageBase* parent)
{
    if (!opened)
    {
        parentMenuPage = parent;
        opened = true;
        open();
        return true;
    }
    return false;
}

MenuPageBase* MenuPageBase::closeMenu()
{
    if (opened)
    {
        MenuPageBase* ret = parentMenuPage;
        close();
        parentMenuPage = 0;
        opened = false;
        return ret;
    }
    return 0;
}
