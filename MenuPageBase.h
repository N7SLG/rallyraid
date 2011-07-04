
#ifndef MENUPAGEBASE_H
#define MENUPAGEBASE_H

#include <irrlicht.h>

class MenuPageBase : public irr::IEventReceiver
{
protected:
    MenuPageBase();
    virtual ~MenuPageBase();

protected:
    virtual bool OnEvent(const irr::SEvent &event) = 0;
    virtual void open() = 0;
    virtual void close() = 0;

public:
    bool isOpened(); // inline
    bool getOpened(); // inline

private:
    bool openMenu(MenuPageBase* parent = 0);
    MenuPageBase* closeMenu();

protected:
    MenuPageBase*   parentMenuPage;
    bool            opened;
    
    friend class MenuManager;
};

inline bool MenuPageBase::isOpened()
{
    return opened;
}

inline bool MenuPageBase::getOpened()
{
    return opened;
}

#endif // MENUPAGEBASE_H
