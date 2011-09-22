
#include "MessageManager.h"
#include "TheGame.h"
#include "FontManager.h"

MessageManager* MessageManager::messageManager = 0;

void MessageManager::initialize()
{
    if (messageManager == 0)
    {
        messageManager = new MessageManager();
    }
}

void MessageManager::finalize()
{
    if (messageManager)
    {
        delete messageManager;
        messageManager = 0;
    }
}

MessageManager::MessageManager()
    : messageText(0),
      timeout(0),
      messageHistory()
{
    messageText = TheGame::getInstance()->getEnv()->addStaticText(L"messageText",
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width/2-400,40,TheGame::getInstance()->getScreenSize().Width/2+400,110),
        true, // border
        true,  // wordwrap
        0, -1,
        true); // show bg
    messageText->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_12PX));
    messageText->setVisible(false);
}

MessageManager::~MessageManager()
{
    messageHistory.clear();
}

void MessageManager::addText(const wchar_t* text, unsigned int timeoutSec, bool renderRefresh, bool addToHistory)
{
    irr::core::recti messageRect = messageText->getRelativePosition();
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
    timeout = /*device->getTimer()->getTime()*/TheGame::getInstance()->getTick()+timeoutSec*1000;
    if (text)
    {
        messageText->setText(text);
        if (addToHistory)
        {
            irr::core::stringw ins(text);
            irr::core::list<irr::core::stringw> insList;
            unsigned int ret = ins.split(insList, L"\n");
            messageHistory.push_front(irr::core::stringw(L"---------------------------"));
            if (ret == 0)
            {
                messageHistory.push_front(ins);
            }
            else
            {
                irr::core::list<irr::core::stringw>::ConstIterator it = insList.getLast();
                while (true)
                {
                    messageHistory.push_front(*it);
                    if (it == insList.begin()) break;
                    it--;
                }
            }            
            
            /*
            core::stringw ins(text);
            ins.replace(L'\n', L'|');
            messageHistory.push_front(ins);
            */
        }
    }
    messageRect.LowerRightCorner.Y = messageRect.UpperLeftCorner.Y + messageText->getTextHeight();
    messageText->setRelativePosition(messageRect);
    messageText->setVisible(true);
    if (renderRefresh)
    {
       device->run();
       device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
       device->getGUIEnvironment()->drawAll();
       device->getVideoDriver()->endScene();
    }
}

void MessageManager::updateText(unsigned int tick)
{
    //printf("updateText %u %u\n", timeout, device->getTimer()->getTime());
    if (timeout==0 || tick < timeout) return;
    messageText->setVisible(false);
    timeout = 0;
}

void MessageManager::hideText()
{
    messageText->setVisible(false);
    timeout = 0;
}

void MessageManager::refreshText()
{
    irr::IrrlichtDevice* device = TheGame::getInstance()->getDevice();
    device->run();
    device->getVideoDriver()->beginScene(true, true, irr::video::SColor(0,192,192,192));
    device->getGUIEnvironment()->drawAll();
    device->getVideoDriver()->endScene();
}
