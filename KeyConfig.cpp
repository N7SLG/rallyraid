
#include "KeyConfig.h"
#include "stdafx.h"

#include <OISKeyboard.h>
#include <OISJoyStick.h>

#include <math.h>
#include <assert.h>

static const char* keyCodes[] =
{
    "0x00","ESC","1","2","3","4","5","6","7","8","9","0",
    "-","=","back","tab",
    "Q","W","E","R","T","Y","U","I","O","P",
    "(",")","Enter","Left Control",
    "A","S","D","F","G","H","J","K","L",
    ";","'","Grave","Left Shift","\\",
    "Z","X","C","V","B","N","M",
    ",",".","/","Right Shift","*","Left Alt","Space","Capital",
    "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10",
    "Numlock","Scroll","numpad7","numpad8","numpad9","numpad-",
    "numpad4","numpad5","numpad6","numpad+","numpad1","numpad2","numpad3",
    "numpad0","numpad.","0x54","0x55","oem_102","F11","F12","0x59",
    "0x5a","0x5b","0x5c","0x5d","0x5e","0x5f","0x60","0x61","0x62",
    "0x63","F13","F14","F15","0x67","0x68","0x69","0x6a","0x6b",
    "0x6c","0x6d","0x6e","0x6f","kana","0x71","0x72","abnt_c1",
    "0x74","0x75","0x76","0x77","0x78","convert", "0x7a","noconvert", 
    "0x7c","yen","abnt_c2","0x7f","0x80","0x81","0x82","0x83","0x84",
    "0x85","0x86","0x87","0x88","0x89","0x8a","0x8b","0x8c",
    "numpad=","0x8e","0x8f","prevtrack","at","colon","underline",
    "kanji","stop","ax","unlabeled", "0x98","nexttrack", "0x9a",
    "0x9b","numpadenter","rcontrol", "0x9e", "0x9f","mute","calculater",
    "playpause", "0xa3","mediastop","0xa5","0xa6","0xa7","0xa8","0xa9",
    "0xaa","0xab","0xac","0xad","volumedown", "0xaf","volumeup", "0xb1",
    "webhome","numpadcomma", "0xb4","divide", "0xb6","prtscr","Right Alt",
    "0xb9","0xba","0xbb","0xbc","0xbd","0xbe", "0xbf","0xc0","0xc1",
    "0xc2","0xc3","0xc4","Pause","0xc6","Home","Up","Page Up","0xca",
    "Left", "0xcc","Right", "0xce","End","Down","Page Down","Insert",
    "Delete","0xd4","0xd5","0xd6","0xd7","0xd8","0xd9","0xda","Left Win",
    "Right Win","Apps","Power","Sleep","0xe0","0xe1","0xe2","Wake", "0xe4",
    "Web Search","Web Favorites","Web Stop","Web Forward","Web Back",
    "My Computer","Mail","Media Select"
};

static const char* typeCodes[] =
{
    "Keyboard",
    "JoystickButton",
    "JoystickAxis",
    "JoystickSliderX",
    "JoystickSliderY",
    "JoystickPov"
};

OIS::JoyStickState KeyConfig::centralJoystickState;

KeyConfig* KeyConfig::getKeyConfig(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState, float deadZone, bool continous)
{
    KeyConfig* ret = 0;

    if (joystickState.mAxes.size() != centralJoystickState.mAxes.size() ||
        joystickState.mButtons.size() != centralJoystickState.mButtons.size()
        )
    {
        dprintf(MY_DEBUG_INFO, "joystick state has been changed: buttons: %lu -> %lu, axis: %lu -> %lu\n",
            joystickState.mButtons.size(), centralJoystickState.mButtons.size(),
            joystickState.mAxes.size(), centralJoystickState.mAxes.size());
        centralJoystickState = joystickState;
        return ret;
    }

    for (/*OIS::KeyCode*/unsigned int i = OIS::KC_ESCAPE; i <= OIS::KC_MEDIASELECT; i++)
    {
        if (keyboard->isKeyDown((OIS::KeyCode)i))
        {
            ret = new KeyConfigKeyboard();
            ret->continous = continous;
            ret->key = i;
            dprintf(MY_DEBUG_INFO, "keyboard assign: %u\n", i);
            return ret;
        }
    }

    for (unsigned int i = 0; i < joystickState.mButtons.size(); i++)
    {
        if (joystickState.mButtons[i])
        {
            ret = new KeyConfigJoystickButton();
            ret->continous = continous;
            ret->key = i;
            dprintf(MY_DEBUG_INFO, "joystick button assign: %u\n", i);
            return ret;
        }
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        if (joystickState.mPOV[i].direction)
        {
            ret = new KeyConfigJoystickPov();
            ret->key = i;
            ret->continous = continous;
            ((KeyConfigJoystickPov*)ret)->key2 = joystickState.mPOV[i].direction;
            dprintf(MY_DEBUG_INFO, "joystick pov assign: %u %x\n", i, joystickState.mPOV[i].direction);
            return ret;
        }
    }
    

    for (unsigned int i = 0; i < joystickState.mAxes.size(); i++)
    {
        if (abs(centralJoystickState.mAxes[i].abs - joystickState.mAxes[i].abs) > 10000)
        {
            if (joystickState.mAxes[i].abs == OIS::JoyStick::MIN_AXIS ||
                joystickState.mAxes[i].abs == OIS::JoyStick::MAX_AXIS ||
                abs(centralJoystickState.mAxes[i].abs - joystickState.mAxes[i].abs) > 20000)
            {
                dprintf(MY_DEBUG_INFO, "joystick state has been changed, but recalibrate: axes[%u]: %d -> %d\n", i,
                    centralJoystickState.mAxes[i].abs, joystickState.mAxes[i].abs);
                centralJoystickState = joystickState;
                return ret;
            }
            
            KeyConfigJoystickAxis* kc = new KeyConfigJoystickAxis();
            kc->key = i;
            kc->from = centralJoystickState.mAxes[i].abs;
            if (joystickState.mAxes[i].abs < kc->from/*-5000*/)
            {
                kc->to = OIS::JoyStick::MIN_AXIS;
            }
            else //if (joystickState.mAxes[i].abs > kc->from/*5000*/)
            {
                kc->to = OIS::JoyStick::MAX_AXIS;
            }
            dprintf(MY_DEBUG_INFO, "joystick axis assign: %u, from: %d, to: %d\n", i, kc->from, kc->to);
            return kc;
        }
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        if (abs(centralJoystickState.mSliders[i].abX - joystickState.mSliders[i].abX) > 10000)
        {
            if (joystickState.mSliders[i].abX == OIS::JoyStick::MIN_AXIS ||
                joystickState.mSliders[i].abX == OIS::JoyStick::MAX_AXIS ||
                abs(centralJoystickState.mSliders[i].abX - joystickState.mSliders[i].abX) > 20000)
            {
                dprintf(MY_DEBUG_INFO, "joystick state has been changed, but recalibrate: slider[%u] x: %d -> %d\n", i,
                    centralJoystickState.mSliders[i].abY, joystickState.mSliders[i].abY);
                centralJoystickState = joystickState;
                return ret;
            }
            
            KeyConfigJoystickAxis* kc = new KeyConfigJoystickSliderX();
            kc->key = i;
            kc->from = centralJoystickState.mSliders[i].abX;
            if (joystickState.mSliders[i].abX < kc->from/*-5000*/)
            {
                kc->to = OIS::JoyStick::MIN_AXIS;
            }
            else //if (joystickState.mSliders[i].abX > kc->from/*5000*/)
            {
                kc->to = OIS::JoyStick::MAX_AXIS;
            }
            dprintf(MY_DEBUG_INFO, "joystick slide x assign: %u, from: %d, to: %d\n", i, kc->from, kc->to);
            return kc;
        }
    }

    for (unsigned int i = 0; i < 4; i++)
    {
        if (abs(centralJoystickState.mSliders[i].abY - joystickState.mSliders[i].abY) > 10000)
        {
            if (joystickState.mSliders[i].abY == OIS::JoyStick::MIN_AXIS ||
                joystickState.mSliders[i].abY == OIS::JoyStick::MAX_AXIS ||
                abs(centralJoystickState.mSliders[i].abY - joystickState.mSliders[i].abY) > 20000)
            {
                dprintf(MY_DEBUG_INFO, "joystick state has been changed, but recalibrate: slider[%u] y: %d -> %d\n", i,
                    centralJoystickState.mSliders[i].abY, joystickState.mSliders[i].abY);
                centralJoystickState = joystickState;
                return ret;
            }
            
            KeyConfigJoystickAxis* kc = new KeyConfigJoystickSliderY();
            kc->key = i;
            kc->from = centralJoystickState.mSliders[i].abY;
            if (joystickState.mSliders[i].abY < kc->from/*-5000*/)
            {
                kc->to = OIS::JoyStick::MIN_AXIS;
            }
            else //if (joystickState.mSliders[i].abY > kc->from/*5000*/)
            {
                kc->to = OIS::JoyStick::MAX_AXIS;
            }
            dprintf(MY_DEBUG_INFO, "joystick slide y assign: %u (%d - %d)\n", i, centralJoystickState.mSliders[i].abY, joystickState.mSliders[i].abY);
            return kc;
        }
    }

    return ret;
}

KeyConfig* KeyConfig::getKeyConfig(KeyConfig::type_t p_type, unsigned int p_key, unsigned int p_key2, int p_from, int p_to, bool continous)
{
    switch (p_type)
    {
        case KeyConfig::Keyboard:
        {
            KeyConfigKeyboard* ret = new KeyConfigKeyboard();
            ret->continous = continous;
            ret->key = p_key;
            return ret;
            break;
        }
        case KeyConfig::JoystickButton:
        {
            KeyConfigJoystickButton* ret = new KeyConfigJoystickButton();
            ret->continous = continous;
            ret->key = p_key;
            return ret;
            break;
        }
        case KeyConfig::JoystickAxis:
        {
            KeyConfigJoystickAxis* ret = new KeyConfigJoystickAxis();
            ret->continous = continous;
            ret->key = p_key;
            ret->from = p_from;
            ret->to = p_to;
            return ret;
            break;
        }
        case KeyConfig::JoystickSliderX:
        {
            KeyConfigJoystickSliderX* ret = new KeyConfigJoystickSliderX();
            ret->continous = continous;
            ret->key = p_key;
            ret->from = p_from;
            ret->to = p_to;
            return ret;
            break;
        }
        case KeyConfig::JoystickSliderY:
        {
            KeyConfigJoystickSliderY* ret = new KeyConfigJoystickSliderY();
            ret->continous = continous;
            ret->key = p_key;
            ret->from = p_from;
            ret->to = p_to;
            return ret;
            break;
        }
        case KeyConfig::JoystickPov:
        {
            KeyConfigJoystickPov* ret = new KeyConfigJoystickPov();
            ret->continous = continous;
            ret->key = p_key;
            ret->key2 = p_key2;
            return ret;
            break;
        }
    }

    return 0;
}

void KeyConfig::recalibrate(OIS::JoyStick* joystick)
{
    centralJoystickState = joystick->getJoyStickState();
}

// class KeyConfig
KeyConfig::KeyConfig(type_t p_type)
    : continous(true), active(false), lastActive(false), type(p_type), key((unsigned int)-1)
{
}

KeyConfig::~KeyConfig()
{
}

float KeyConfig::getPercentage(int state)
{
    bool bstate = state != 0;
    lastActive = active;
    if (continous)
    {
        active = bstate;
        return active ? 1.0f : 0.0f;
    }
    else
    {
        if (bstate != active)
        {
            active = bstate;
            return active ? 1.0f : 0.0f;
        }
    }
    return 0.0f;
}

// class KeyConfigKeyboard : public KeyConfig
KeyConfigKeyboard::KeyConfigKeyboard()
    : KeyConfig(KeyConfig::Keyboard)
{
}

KeyConfigKeyboard::~KeyConfigKeyboard()
{
}

float KeyConfigKeyboard::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joytickState)
{
    return KeyConfig::getPercentage((int)(keyboard->isKeyDown((OIS::KeyCode)key)));
}

void KeyConfigKeyboard::writeToFile(FILE* f, const std::string& prefix)
{
    fprintf_s(f, "%s_type=%u\n", prefix.c_str(), (unsigned int)type);
    fprintf_s(f, "%s_key=%u\n", prefix.c_str(), key);
}

const wchar_t* KeyConfigKeyboard::getName()
{
    name = typeCodes[type];
    name += L" ";
    name += keyCodes[key];
    return name.c_str();
}

// class KeyConfigJoystickButton : public KeyConfig
KeyConfigJoystickButton::KeyConfigJoystickButton()
    : KeyConfig(KeyConfig::JoystickButton)
{
}

KeyConfigJoystickButton::~KeyConfigJoystickButton()
{
}

float KeyConfigJoystickButton::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState)
{
    if (key < joystickState.mButtons.size())
    {
        return KeyConfig::getPercentage(joystickState.mButtons[key]);
    }
    else
    {
        return 0.0f;
    }
}

void KeyConfigJoystickButton::writeToFile(FILE* f, const std::string& prefix)
{
    fprintf_s(f, "%s_type=%u\n", prefix.c_str(), (unsigned int)type);
    fprintf_s(f, "%s_key=%u\n", prefix.c_str(), key);
}

const wchar_t* KeyConfigJoystickButton::getName()
{
    name = typeCodes[type];
    name += L" ";
    name += key;
    return name.c_str();
}

// class KeyConfigJoystickPov : public KeyConfig
KeyConfigJoystickPov::KeyConfigJoystickPov()
    : KeyConfig(KeyConfig::JoystickPov), key2((unsigned int)-1)
{
}

KeyConfigJoystickPov::~KeyConfigJoystickPov()
{
}

float KeyConfigJoystickPov::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState)
{
    return KeyConfig::getPercentage((int)(joystickState.mPOV[key].direction == key2));
}

void KeyConfigJoystickPov::writeToFile(FILE* f, const std::string& prefix)
{
    fprintf_s(f, "%s_type=%u\n", prefix.c_str(), (unsigned int)type);
    fprintf_s(f, "%s_key=%u\n", prefix.c_str(), key);
    fprintf_s(f, "%s_key2=%u\n", prefix.c_str(), key2);
}

const wchar_t* KeyConfigJoystickPov::getName()
{
    name = typeCodes[type];
    name += L" ";
    name += key;
    switch (key2)
    {
    case OIS::Pov::North:
        name += L" up";
        break;
    case OIS::Pov::South:
        name += L" down";
        break;
    case OIS::Pov::East:
        name += L" right";
        break;
    case OIS::Pov::West:
        name += L" left";
        break;
    case OIS::Pov::NorthEast:
        name += L" up-right";
        break;
    case OIS::Pov::SouthEast:
        name += L" down-right";
        break;
    case OIS::Pov::NorthWest:
        name += L" up-left";
        break;
    case OIS::Pov::SouthWest:
        name += L" down-left";
        break;
    default:
        name += L" unknown";
        break;
    }
    return name.c_str();
}


// class KeyConfigJoystickAxis : public KeyConfig
KeyConfigJoystickAxis::KeyConfigJoystickAxis()
    : KeyConfig(KeyConfig::JoystickAxis), from(OIS::JoyStick::MIN_AXIS), to(OIS::JoyStick::MAX_AXIS)
{
}

KeyConfigJoystickAxis::KeyConfigJoystickAxis(KeyConfig::type_t p_type)
    : KeyConfig(p_type), from(OIS::JoyStick::MIN_AXIS), to(OIS::JoyStick::MAX_AXIS)
{
}

KeyConfigJoystickAxis::~KeyConfigJoystickAxis()
{
}

float KeyConfigJoystickAxis::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState)
{
    if (key < joystickState.mAxes.size())
    {
        return KeyConfigJoystickAxis::getPercentage(joystickState.mAxes[key].abs);
    }
    else
    {
        return 0.0f;
    }
}

float KeyConfigJoystickAxis::getPercentage(int state)
{
    int min = to < from ? to : from;
    int max = to > from ? to : from;

    /*if (min < 0)
    {
        max += -min;
        state += -min;
        min = 0;
    }*/

    if (state < min)
        state = min;
    else
        if (state > max)
            state = max;
    
    if (to > from)
    {
        return (float)(state - min) / (float)(max - min);
    }
    else
    {
        return (float)(max - state) / (float)(max - min);
    }
}

void KeyConfigJoystickAxis::writeToFile(FILE* f, const std::string& prefix)
{
    fprintf_s(f, "%s_type=%u\n", prefix.c_str(), (unsigned int)type);
    fprintf_s(f, "%s_key=%u\n", prefix.c_str(), key);
    fprintf_s(f, "%s_from=%d\n", prefix.c_str(), from);
    fprintf_s(f, "%s_to=%d\n", prefix.c_str(), to);
}

const wchar_t* KeyConfigJoystickAxis::getName()
{
    name = typeCodes[type];
    name += L" ";
    name += key;
    if (to > from)
    {
        name += L" +";
    }
    else
    {
        name += L" -";
    }
    return name.c_str();
}

// class KeyConfigJoystickSliderX : public KeyConfigJoystickAxis
KeyConfigJoystickSliderX::KeyConfigJoystickSliderX()
    : KeyConfigJoystickAxis(KeyConfig::JoystickSliderX)
{
}

KeyConfigJoystickSliderX::~KeyConfigJoystickSliderX()
{
}

float KeyConfigJoystickSliderX::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState)
{
    return KeyConfigJoystickAxis::getPercentage(joystickState.mSliders[key].abX);
}

// class KeyConfigJoystickSliderY : public KeyConfigJoystickAxis
KeyConfigJoystickSliderY::KeyConfigJoystickSliderY()
    : KeyConfigJoystickAxis(KeyConfig::JoystickSliderY)
{
}

KeyConfigJoystickSliderY::~KeyConfigJoystickSliderY()
{
}

float KeyConfigJoystickSliderY::getPercentage(OIS::Keyboard* keyboard, const OIS::JoyStickState& joystickState)
{
    return KeyConfigJoystickAxis::getPercentage(joystickState.mSliders[key].abY);
}

