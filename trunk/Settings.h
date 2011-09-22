#ifndef SETTINGS_H
#define SETTINGS_H

#include "stdafx.h"
#include <string>

class Settings
{
public:
    static void initialize();
    static void finalize();
    static Settings* getInstance() {return settings;}

private:
    static Settings* settings;
    static const std::string settingsFilename;

private:
    Settings();
    ~Settings();

public:
    void read();
    void write();

public:
    bool                    preloadObjects;
    bool                    cacheObjects;
    unsigned int            targetFps;
    bool                    nonshaderLight;
    unsigned int            objectWireSize;
    unsigned int            objectWireNum;
    unsigned int            objectDensity;
    bool                    useTerrainDetail;
    bool                    showNames;
    unsigned int            difficulty;
    bool                    navigationAssistant;
    unsigned int            resolutionX;
    unsigned int            resolutionY;
    unsigned int            displayBits;
    std::string             driverType;
    bool                    fullScreen;
    bool                    vsync;
    bool                    scanForJoystick;
    unsigned int            shadowMapSize;
    float                   joystickDeadZone;
    bool                    manualGearShifting;
    bool                    sequentialGearShifting;
    bool                    editorMode;
    float                   steerRate;
    float                   steerRatePressed;
    float                   groundFriction;
    float                   fpsStep;
};

#endif // SETTINGS_H
