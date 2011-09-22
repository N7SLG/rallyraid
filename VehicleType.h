#ifndef VEHICLETYPE_H
#define VEHICLETYPE_H

#include <string>
#include <irrlicht.h>
#include <map>
#include "Player_defs.h"

class VehicleTypeTyre
{
public:
    VehicleTypeTyre(unsigned int id = (unsigned int)-1);
    ~VehicleTypeTyre();

public:
    unsigned int            id;
    std::string             objectName;
    irr::video::ITexture*   texture;
    irr::core::vector3df    scale;
    irr::core::vector3df    localPos;
    float                   mass;
    float                   friction;
    float                   suspensionLength;
    float                   suspensionSpring;
    float                   suspensionDamper;
    bool                    steerable;
    bool                    torqueable;
    bool                    handbrakeable;
};
typedef std::map<unsigned int, VehicleTypeTyre*> vehicleTypeTyreMap_t;
typedef std::map<unsigned int, float> gearMap_t;

class VehicleType
{
public:
    VehicleType(const std::string& vehicleTypeName, const std::string& vehicleTypeFilename, bool& ret);
    ~VehicleType();

    const std::string& getName(); // inline
    const std::string& getLongName(); // inline
    float getMaxSpeed(); // inline
    float getMaxRPM(); // inline

private:
    bool read(const std::string& vehicleTypeFilename);

private:
    std::string             vehicleTypeName;
    vehicleTypeTyreMap_t    vehicleTypeTyreMap;
    gearMap_t               gearMap;

    std::string             objectName;
    std::string             vehicleLongName;
    irr::video::ITexture*   texture;
    std::string             engineSoundFilename;
    float                   maxBrakeForce;
    float                   maxSpeed;
    float                   maxTorque;
    float                   maxTorqueRate;
    unsigned int            changeGearTime;
    float                   maxSteerAngle;
    float                   maxSteerRate;
    irr::core::matrix4      viewPos[VIEW_SIZE];
    irr::core::matrix4      viewDest[VIEW_SIZE];
    float                   mass;   // only used for info in the main menu
    
    static float            maxMaxBrakeForce;
    static float            maxMaxSpeed;
    static float            maxMaxTorque;
    static float            maxMaxTorqueRate;
    static float            maxMaxSteerAngle;
    

    friend class Vehicle;
    friend class MenuPageMain;
};

inline const std::string& VehicleType::getName()
{
    return vehicleTypeName;
}

inline const std::string& VehicleType::getLongName()
{
    return vehicleLongName;
}

inline float VehicleType::getMaxSpeed()
{
    return maxSpeed;
}

inline float VehicleType::getMaxRPM()
{
    return maxTorque;
}

#endif // VEHICLETYPE_H
