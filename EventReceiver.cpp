
#include "EventReceiver.h"
#include "TheGame.h"
#include "KeyConfig.h"
#include "ConfigFile.h"
#include "StringConverter.h"
#include "Player.h"
#include "Vehicle.h"
#include "MenuManager.h"
#include "TheEarth.h"
#include "MenuPageOptionsKB.h"
#include "Hud.h"
#include "Settings.h"

#include "stdafx.h"

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OISJoyStick.h>

const std::string EventReceiver::keyMappingFilename = "data/Dakar2012_keymapping.cfg";

EventReceiver::EventReceiver()
    : inputManager(0),
      mouse(0),
      keyboard(0),
      joystick(0),
      deadZone(0.01f),
      test_kc(0),
      keyNameMap()
{
    inputManager = OIS::InputManager::createInputSystem(TheGame::getInstance()->getWindowId());

    keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
    //mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, false));
    joystick = static_cast<OIS::JoyStick*>(inputManager->createInputObject(OIS::OISJoyStick, false));

    recalibrate();

    KeyProperty kp;
    kp.primaryKeyConfig = kp.secondaryKeyConfig = 0;
    kp.continous = true;
    keyNameMap["accelerate"] = ACCELERATE;
    kp.keyLongName = "Accelerate";
    keyMap[ACCELERATE] = kp;
    keyNameMap["brake"] = BRAKE;
    kp.keyLongName = "Brake";
    keyMap[BRAKE] = kp;
    keyNameMap["left"] = LEFT;
    kp.keyLongName = "Steer left";
    keyMap[LEFT] = kp;
    keyNameMap["right"] = RIGHT;
    kp.keyLongName = "Steer right";
    keyMap[RIGHT] = kp;
    keyNameMap["handbrake"] = HANDBRAKE;
    kp.keyLongName = "Handbrake";
    keyMap[HANDBRAKE] = kp;
    keyNameMap["clutch"] = CLUTCH;
    kp.keyLongName = "Clutch";
    keyMap[CLUTCH] = kp;
    keyNameMap["look_left"] = LOOK_LEFT;
    kp.keyLongName = "Look left";
    keyMap[LOOK_LEFT] = kp;
    keyNameMap["look_right"] = LOOK_RIGHT;
    kp.keyLongName = "Look right";
    keyMap[LOOK_RIGHT] = kp;

    kp.continous = false;
    keyNameMap["physics"] = PHYSICS;
    kp.keyLongName = "Switch Physics on/off";
    keyMap[PHYSICS] = kp;
    keyNameMap["fps_camera"] = FPS_CAMERA;
    kp.keyLongName = "Change FPS camera";
    keyMap[FPS_CAMERA] = kp;
    keyNameMap["change_view"] = CHANGE_VIEW;
    kp.keyLongName = "Change view";
    keyMap[CHANGE_VIEW] = kp;
    keyNameMap["open_editor"] = OPEN_EDITOR;
    kp.keyLongName = "Open editor window";
    keyMap[OPEN_EDITOR] = kp;
    keyNameMap["reset_vehicle"] = RESET_VEHICLE;
    kp.keyLongName = "Reset vehicle";
    keyMap[RESET_VEHICLE] = kp;
    keyNameMap["switch_input"] = SWITCH_INPUT;
    kp.keyLongName = "Switch input while editor (game/editor)";
    keyMap[SWITCH_INPUT] = kp;
    keyNameMap["exit_to_menu"] = EXIT_TO_MENU;
    kp.keyLongName = "Game pause menu";
    keyMap[EXIT_TO_MENU] = kp;
    keyNameMap["roadbook_next"] = ROADBOOK_NEXT;
    kp.keyLongName = "Go to the next roadbook element";
    keyMap[ROADBOOK_NEXT] = kp;
    keyNameMap["roadbook_prev"] = ROADBOOK_PREV;
    kp.keyLongName = "Go to the previous roadbook element";
    keyMap[ROADBOOK_PREV] = kp;
    keyNameMap["reset_partial"] = RESET_PARTIAL;
    kp.keyLongName = "Reset partial";
    keyMap[RESET_PARTIAL] = kp;

    loadKeyMapping();
    //saveKeyMapping();
}

EventReceiver::~EventReceiver()
{
    clearKeyMapping();

    if (inputManager)
    {
        //inputManager->destroyInputObject(mouse);
        //mouse = 0;
        inputManager->destroyInputObject(keyboard);
        keyboard = 0;
        inputManager->destroyInputObject(joystick);
        joystick = 0;
 
        OIS::InputManager::destroyInputSystem(inputManager);
        inputManager = 0;
    }
}

void EventReceiver::recalibrate()
{
    joystick->capture();
    //centralJoystickState = joystick->getJoyStickState();
    KeyConfig::recalibrate(joystick);
}

void EventReceiver::updateWindow(unsigned int width, unsigned int height)
{
    if (mouse)
    {
        const OIS::MouseState &ms = mouse->getMouseState();
        ms.width = width;
        ms.height = height;
    }
}

void EventReceiver::saveKeyMapping()
{
    FILE* f;
    errno_t error = fopen_s(&f, keyMappingFilename.c_str(), "w");
    if (error)
    {
        printf("unable to open file for write %s\n", keyMappingFilename.c_str());
        return;
    }

    bool first = true;

    for (keyNameMap_t::const_iterator it = keyNameMap.begin();
         it != keyNameMap.end();
         it++)
    {
        if (keyMap[it->second].primaryKeyConfig || keyMap[it->second].secondaryKeyConfig)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                fprintf_s(f, "\n");
            }
            fprintf_s(f, "[%s]\n", it->first.c_str());
        }
        if (keyMap[it->second].primaryKeyConfig)
        {
            keyMap[it->second].primaryKeyConfig->writeToFile(f, "primary");
        }
        if (keyMap[it->second].secondaryKeyConfig)
        {
            keyMap[it->second].secondaryKeyConfig->writeToFile(f, "secondary");
        }
    }

    fclose(f);
}

void EventReceiver::clearKeyMapping()
{
    for (unsigned int kn = ACCELERATE; kn < NUMBER_OF_KEYNAMES; kn++)
    {
        if (keyMap[kn].primaryKeyConfig)
        {
            delete keyMap[kn].primaryKeyConfig;
        }
        if (keyMap[kn].secondaryKeyConfig)
        {
            delete keyMap[kn].secondaryKeyConfig;
        }
        keyMap[kn].primaryKeyConfig = keyMap[kn].secondaryKeyConfig = 0;
    }
}

void EventReceiver::loadKeyMapping()
{
    // Load resource paths from config file
    clearKeyMapping();

    ConfigFile cf;
    cf.load(keyMappingFilename);

    dprintf(MY_DEBUG_NOTE, "Read keymapping file:\n");
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string secName, keyName, valueName;
    while (seci.hasMoreElements())
    {
        bool primary = false;
        bool secondary = false;
        unsigned int p_type = 0;
        unsigned int p_key = (unsigned int)-1;
        unsigned int p_key2 = (unsigned int)-1;
        int p_from = 0;
        int p_to = 0;
        unsigned int s_type = 0;
        unsigned int s_key = (unsigned int)-1;
        unsigned int s_key2 = (unsigned int)-1;
        int s_from = 0;
        int s_to = 0;

        secName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tKey: %s\n", secName.c_str());
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        for (ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valueName = i->second;

            if (keyName == "primary_type")
            {
                primary = true;
                p_type = StringConverter::parseUnsignedInt(valueName, 0);
            } else
            if (keyName == "secondary_type")
            {
                secondary = true;
                s_type = StringConverter::parseUnsignedInt(valueName, 0);
            } else
            if (keyName == "primary_key")
            {
                primary = true;
                p_key = StringConverter::parseUnsignedInt(valueName, (unsigned int)-1);
            } else
            if (keyName == "secondary_key")
            {
                secondary = true;
                s_key = StringConverter::parseUnsignedInt(valueName, (unsigned int)-1);
            } else
            if (keyName == "primary_key2")
            {
                primary = true;
                p_key2 = StringConverter::parseUnsignedInt(valueName, (unsigned int)-1);
            } else
            if (keyName == "secondary_key2")
            {
                secondary = true;
                s_key2 = StringConverter::parseUnsignedInt(valueName, (unsigned int)-1);
            } else
            if (keyName == "primary_from")
            {
                primary = true;
                p_from = StringConverter::parseInt(valueName, 0);
            } else
            if (keyName == "secondary_from")
            {
                secondary = true;
                s_from = StringConverter::parseInt(valueName, 0);
            } else
            if (keyName == "primary_to")
            {
                primary = true;
                p_to = StringConverter::parseInt(valueName, 0);
            } else
            if (keyName == "secondary_to")
            {
                secondary = true;
                s_to = StringConverter::parseInt(valueName, 0);
            }
        }
        keyNameMap_t::iterator kit = keyNameMap.find(secName);
        if (kit != keyNameMap.end())
        {
            KeyName kn = kit->second;
            assert(kn < NUMBER_OF_KEYNAMES);
            if (primary)
            {
                keyMap[kn].primaryKeyConfig = KeyConfig::getKeyConfig((KeyConfig::type_t)p_type, p_key, p_key2, p_from, p_to, keyMap[kn].continous);
            }
            if (secondary)
            {
                keyMap[kn].secondaryKeyConfig = KeyConfig::getKeyConfig((KeyConfig::type_t)s_type, s_key, s_key2, s_from, s_to, keyMap[kn].continous);
            }
        }
    }
}

#define IS_PRESSED(BUTTON) ((keyMap[BUTTON].primaryKeyConfig && keyMap[BUTTON].primaryKeyConfig->getPercentage(keyboard, joystickState)) || \
    (keyMap[BUTTON].secondaryKeyConfig && keyMap[BUTTON].secondaryKeyConfig->getPercentage(keyboard, joystickState)))

float EventReceiver::getPercentage(EventReceiver::KeyName BUTTON, const OIS::JoyStickState& joystickState)
{
    float PERC = 0.0f;
    if (keyMap[BUTTON].primaryKeyConfig)
        PERC = keyMap[BUTTON].primaryKeyConfig->getPercentage(keyboard, joystickState);
    
    if (keyMap[BUTTON].secondaryKeyConfig)
    {
        float p2 = 0.0f;
        p2 = keyMap[BUTTON].secondaryKeyConfig->getPercentage(keyboard, joystickState);
        if (p2 > PERC) PERC = p2;
    }
    return PERC;
}

bool EventReceiver::getPressed(EventReceiver::KeyName BUTTON)
{
    bool p1 = false;
    bool p2 = false;
    if (keyMap[BUTTON].primaryKeyConfig)
        p1 = keyMap[BUTTON].primaryKeyConfig->getPressed();
    if (keyMap[BUTTON].secondaryKeyConfig)
        p2 = keyMap[BUTTON].secondaryKeyConfig->getPressed();
    return p1 || p2;
}

bool EventReceiver::getReleased(EventReceiver::KeyName BUTTON)
{
    bool p1 = false;
    bool p2 = false;
    if (keyMap[BUTTON].primaryKeyConfig)
        p1 = keyMap[BUTTON].primaryKeyConfig->getReleased();
    if (keyMap[BUTTON].secondaryKeyConfig)
        p2 = keyMap[BUTTON].secondaryKeyConfig->getReleased();
    return p1 || p2;
}

void EventReceiver::checkEvents()
{
    //Need to capture/update each device
    keyboard->capture();
    joystick->capture();

    const OIS::JoyStickState joystickState = joystick->getJoyStickState();
    
#if 0
    if (test_kc == 0)
    {
        test_kc = KeyConfig::getKeyConfig(keyboard, joystickState, deadZone, false);
        if (test_kc)
        {
            dprintf(MY_DEBUG_INFO, "test_kc assigned: %d\n", test_kc->key);
            //assert(0);
        }
    }
    else
    {
        float pressed = test_kc->getPercentage(keyboard, joystickState);
        if (pressed > deadZone)
        {
            dprintf(MY_DEBUG_INFO, "test_kc pressed: %f\n", pressed);
        }
    }
    if (keyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        delete test_kc;
        test_kc = 0;
    }
    

    //printf("%s\n", keyboard->isKeyDown(OIS::KC_K)?"true":"false");
    /*
    printf("Axes: %lu\n", joystickState.mAxes.size());
    for (unsigned int i = 0; i < joystickState.mAxes.size(); i++)
    {
        printf("\t%u: %d (%d)\n", i, joystickState.mAxes[i].abs, joystickState.mAxes[i].rel);
    }
    printf("Sliders:\n");
    for (unsigned int i = 0; i < 4; i++)
    {
        printf("\t%u: %d, %d\n", i, joystickState.mSliders[i].abX, joystickState.mSliders[i].abY);
    }
    */
#else // 0 or 1
    
    if (MenuManager::getInstance()->getMenuInput())
    {
        if (IS_PRESSED(SWITCH_INPUT))
        {
            MenuManager::getInstance()->clearEventReceiver();
        }
    }
    else
    {
    
        // the real event check

        // mouse
        /*
        mouse->capture();
        const OIS::MouseState mouseState = mouse->getMouseState();
        if (mouseState.buttonDown(OIS::MB_Left))
        {
            printf("mouse button left down\n");
        }

        if (mouseState.buttonDown(OIS::MB_Right))
        {
            printf("mouse button right down\n");
        }

        if (mouseState.buttonDown(OIS::MB_Middle))
        {
            printf("mouse button middle down\n");
        }
        */

        // other

        if (IS_PRESSED(ACCELERATE))
        {
            //dprintf(MY_DEBUG_NOTE, "accelerate pressed\n");
            Player::getInstance()->setFirstPressed();
            Player::getInstance()->getVehicle()->setTorque(-1);
        }
        else
        if (IS_PRESSED(BRAKE))
        {
            //dprintf(MY_DEBUG_NOTE, "brake pressed\n");
            Player::getInstance()->setFirstPressed();
            Player::getInstance()->getVehicle()->setTorque(1);
        }
        else
        {
            Player::getInstance()->getVehicle()->setTorque(0);
        }

        if (IS_PRESSED(LEFT))
        {
            //dprintf(MY_DEBUG_NOTE, "left pressed\n");
            Player::getInstance()->getVehicle()->setSteer(-1);
        }
        else
        if (IS_PRESSED(RIGHT))
        {
            //dprintf(MY_DEBUG_NOTE, "right pressed\n");
            Player::getInstance()->getVehicle()->setSteer(1);
        }
        else
        {
            Player::getInstance()->getVehicle()->setSteer(0);
        }

        if (IS_PRESSED(HANDBRAKE))
        {
            //dprintf(MY_DEBUG_NOTE, "brake pressed\n");
            Player::getInstance()->getVehicle()->setHandbrake(1);
        }
        else
        {
            Player::getInstance()->getVehicle()->setHandbrake(0);
        }

        if (IS_PRESSED(PHYSICS))
        {
            TheGame::getInstance()->setPhysicsOngoing(!TheGame::getInstance()->getPhysicsOngoing());
        }
        /*else
        {
            TheGame::getInstance()->setPhysicsOngoing(false);
        }*/

        if (IS_PRESSED(FPS_CAMERA))
        {
            dprintf(MY_DEBUG_NOTE, "switch camera\n");
            TheGame::getInstance()->switchCamera();
        }

        if (IS_PRESSED(LOOK_LEFT) && getPressed(LOOK_LEFT))
        {
            Player::getInstance()->lookLeft(true);
        }
        else if (getReleased(LOOK_LEFT))
        {
            Player::getInstance()->lookLeft(false);
        }

        if (IS_PRESSED(LOOK_RIGHT) && getPressed(LOOK_RIGHT))
        {
            Player::getInstance()->lookRight(true);
        }
        else if (getReleased(LOOK_RIGHT))
        {
            Player::getInstance()->lookRight(false);
        }

        if (IS_PRESSED(CHANGE_VIEW))
        {
            Player::getInstance()->switchToNextView();
        }

        if (IS_PRESSED(OPEN_EDITOR))
        {
            if (TheGame::getInstance()->getEditorMode())
            {
                MenuManager::getInstance()->open(MenuManager::MP_EDITOR);
            }
        }

        if (IS_PRESSED(RESET_VEHICLE))
        {
            Player::getInstance()->resetVehicle(irr::core::vector3df(
                    TheGame::getInstance()->getCamera()->getPosition().X,
                    TheEarth::getInstance()->getHeight(TheGame::getInstance()->getCamera()->getPosition())+2.5f,
                    TheGame::getInstance()->getCamera()->getPosition().Z));
        }

        if (IS_PRESSED(SWITCH_INPUT))
        {
            MenuManager::getInstance()->refreshEventReceiver();
        }

        if (Settings::getInstance()->navigationAssistant == false)
        {
            if (IS_PRESSED(ROADBOOK_NEXT))
            {
                Player::getInstance()->stepItiner();
                Hud::getInstance()->updateRoadBook();
            }
            else if (IS_PRESSED(ROADBOOK_PREV))
            {
                Player::getInstance()->stepBackItiner();
                Hud::getInstance()->updateRoadBook();
            }
            if (IS_PRESSED(RESET_PARTIAL))
            {
                Player::getInstance()->resetDistance();
            }
        }

        if (IS_PRESSED(EXIT_TO_MENU))
        {
            MenuManager::getInstance()->open(MenuManager::MP_INGAME);
        }
    }
#endif // 0 or 1
}

void EventReceiver::checkEventsMenu()
{
    //Need to capture/update each device
    keyboard->capture();
    joystick->capture();

    const OIS::JoyStickState joystickState = joystick->getJoyStickState();
    
    if (MenuPageOptionsKB::menuPageOptionsKB->isOpened())
    {
        if (test_kc == 0)
        {
            test_kc = KeyConfig::getKeyConfig(keyboard, joystickState, deadZone, false);
            if (test_kc)
            {
                dprintf(MY_DEBUG_INFO, "test_kc assigned: %d\n", test_kc->key);
                MenuPageOptionsKB::menuPageOptionsKB->keyConfigReceived(test_kc);
                //assert(0);
            }
        }
    }
}
