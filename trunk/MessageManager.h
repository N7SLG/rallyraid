
#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <irrlicht.h>
#include <list>

class MessageManager
{
public:
    static void initialize();
    static void finalize();
    static MessageManager* getInstance() {return messageManager;}

private:
    static MessageManager* messageManager;

private:
    MessageManager();
    ~MessageManager();

public:
    void addText(const wchar_t* text, unsigned int timeoutSec, bool renderRefresh = false, bool addToHistory = true);
    void updateText(unsigned int tick);
    void refreshText();
    void hideText();
    
    irr::gui::IGUIStaticText* messageText;
    unsigned int timeout;
    std::list<irr::core::stringw> messageHistory;
};

#endif // MESSAGEMANAGER_H

