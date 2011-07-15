
#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <map>
#include <vector>
#include <string>
#include <utility>

namespace OIS
{
    class InputManager;
    class Mouse;
    class Keyboard;
    class JoyStick;
    class JoyStickState;
}

class KeyConfig;

class EventReceiver
{
public:
    //typedef std::map<std::string, unsigned int> strToKey_t;
    //typedef std::vector<std::string> keyToStr_t;
private:
    enum KeyName
    {
        ACCELERATE = 0,
        BRAKE,
        LEFT,
        RIGHT,
        HANDBRAKE,
        CLUTCH,
        PHYSICS,
        FPS_CAMERA,
        LOOK_LEFT,
        LOOK_RIGHT,
        CHANGE_VIEW,
        OPEN_EDITOR,
        RESET_VEHICLE,
        SWITCH_INPUT,
        EXIT_TO_MENU,
        ROADBOOK_NEXT,
        ROADBOOK_PREV,
        RESET_PARTIAL,
        INC_FPS_SPEED,
        DEC_FPS_SPEED,
        NUMBER_OF_KEYNAMES
    };
    struct KeyProperty
    {
        bool            continous;
        KeyConfig*      primaryKeyConfig;
        KeyConfig*      secondaryKeyConfig;
        std::string     keyLongName;
    };
    typedef std::map<std::string, KeyName> keyNameMap_t;
    //typedef std::map<KeyName, KeyProperty> keyMap_t;

private:
    static const std::string keyMappingFilename;

public:
    EventReceiver();
    ~EventReceiver();

    void recalibrate();
    void checkEvents();
    void checkEventsMenu();
    void updateWindow(unsigned int width, unsigned int height);
    void saveKeyMapping();

private:
    void clearKeyMapping();
    void loadKeyMapping();
    float getPercentage(KeyName BUTTON, const OIS::JoyStickState& joystickState);
    bool getPressed(KeyName BUTTON);
    bool getReleased(KeyName BUTTON);

private:
    //OIS Input devices
    OIS::InputManager*  inputManager;
    OIS::Mouse*         mouse;
    OIS::Keyboard*      keyboard;
    OIS::JoyStick*      joystick;

    //OIS::JoyStickState  centralJoystickState;
    float               deadZone;
    KeyConfig*          test_kc;
    keyNameMap_t        keyNameMap;
    KeyProperty         keyMap[NUMBER_OF_KEYNAMES];


    friend class MenuPageOptions;
    friend class MenuPageOptionsKB;
};

#endif // EVENTRECEIVER_H
