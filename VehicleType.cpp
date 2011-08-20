
#include "VehicleType.h"
#include "VehicleTypeManager.h"
#include "ConfigFile.h"
#include "stdafx.h"
#include "TheGame.h"
#include "StringConverter.h"


VehicleTypeTyre::VehicleTypeTyre(unsigned int id)
    : id(id),
      objectName(),
      texture(0),
      scale(),
      localPos(),
      mass(30.f),
      friction(0.8f),
      suspensionLength(0.42f),
      suspensionSpring(30.f),
      suspensionDamper(9.0f),
      steerable(false),
      torqueable(true),
      handbrakeable(false)
{
}

VehicleTypeTyre::~VehicleTypeTyre()
{
}

float VehicleType::maxMaxBrakeForce = 0.f;
float VehicleType::maxMaxSpeed = 0.f;
float VehicleType::maxMaxTorque = 0.f;
float VehicleType::maxMaxSteerAngle = 0.f;

VehicleType::VehicleType(const std::string& vehicleTypeName, const std::string& vehicleTypeFilename, bool& ret)
    : vehicleTypeName(vehicleTypeName),
      vehicleTypeTyreMap(),
      gearMap(),
      objectName(),
      vehicleLongName(),
      texture(0),
      engineSoundFilename(),
      maxBrakeForce(100.f),
      maxSpeed(100.f),
      maxTorque(5000.0f),
      maxTorqueRate(200.0f),
      changeGearTime(20),
      maxSteerAngle(35.0f),
      maxSteerRate(0.1f)
{
    ret = read(vehicleTypeFilename);
}

VehicleType::~VehicleType()
{
}

bool VehicleType::read(const std::string& vehicleTypeFilename)
{

    ConfigFile cf;
    cf.load(vehicleTypeFilename);

    dprintf(MY_DEBUG_NOTE, "Read vehicle type file: %s\n", vehicleTypeName.c_str());
    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    std::string sectionName, keyName, valueName;
    while (seci.hasMoreElements())
    {
        VehicleTypeTyre* currentTyre = 0;
        bool gear = false;
        unsigned int gearId = 0;
        float gearForce = 1.0f;

        sectionName = seci.peekNextKey();
        dprintf(MY_DEBUG_NOTE, "\tVehicle section: %s\n", sectionName.c_str());
        /*if (sectionName == "tyre")
        {
            currentTyre = new VehicleTypeTyre();
        } else*/
        if (sectionName == "front_right_tyre")
        {
            currentTyre = new VehicleTypeTyre(0);
        } else if (sectionName == "front_left_tyre")
        {
            currentTyre = new VehicleTypeTyre(1);
        } else if (sectionName == "rear_right_tyre")
        {
            currentTyre = new VehicleTypeTyre(2);
        } else if (sectionName == "rear_left_tyre")
        {
            currentTyre = new VehicleTypeTyre(3);
        } else if (sectionName.find("tyre") == 0 && sectionName.size() > 5)
        {
            unsigned int num = StringConverter::parseUnsignedInt(sectionName.substr(5), 0xffffffff);
            if (num != 0xffffffff)
            {
                currentTyre = new VehicleTypeTyre(num);
            }
/*        } else if (sectionName == "tyre_1")
        {
            currentTyre = new VehicleTypeTyre(1);
        } else if (sectionName == "tyre_2")
        {
            currentTyre = new VehicleTypeTyre(2);
        } else if (sectionName == "tyre_3")
        {
            currentTyre = new VehicleTypeTyre(3);
        } else if (sectionName == "tyre_4")
        {
            currentTyre = new VehicleTypeTyre(4);
        } else if (sectionName == "tyre_5")
        {
            currentTyre = new VehicleTypeTyre(5);
        } else if (sectionName == "tyre_6")
        {
            currentTyre = new VehicleTypeTyre(6);
        } else if (sectionName == "tyre_7")
        {
            currentTyre = new VehicleTypeTyre(7);
        } else if (sectionName == "tyre_8")
        {
            currentTyre = new VehicleTypeTyre(8);
        } else if (sectionName == "tyre_9")
        {
            currentTyre = new VehicleTypeTyre(9);
*/
        } else if (sectionName.find("gear") == 0 && sectionName.size() > 5)
        {
            gearId = StringConverter::parseUnsignedInt(sectionName.substr(5), 0);
            if (gearId != 0) gear = true;
/*        } else if (sectionName == "gear_2")
        {
            gear = true;
            gearId = 2;
        } else if (sectionName == "gear_3")
        {
            gear = true;
            gearId = 3;
        } else if (sectionName == "gear_4")
        {
            gear = true;
            gearId = 4;
        } else if (sectionName == "gear_5")
        {
            gear = true;
            gearId = 5;
        } else if (sectionName == "gear_6")
        {
            gear = true;
            gearId = 6;
*/
        }
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        for (ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); ++i)
        {
            keyName = i->first;
            valueName = i->second;
            
            dprintf(MY_DEBUG_NOTE, "\t\t%s = %s\n", keyName.c_str(), valueName.c_str());


            if (currentTyre)
            {
                if (keyName == "id")
                {
                    currentTyre->id = StringConverter::parseUnsignedInt(valueName, 0);
                } else if (keyName == "object_name")
                {
                    currentTyre->objectName = valueName;
                } else if (keyName == "texture")
                {
                    currentTyre->texture = TheGame::getInstance()->getDriver()->getTexture(valueName.c_str());
                } else if (keyName == "scale")
                {
                    StringConverter::parseFloat3(valueName, currentTyre->scale.X, currentTyre->scale.Y, currentTyre->scale.Z);
                } else if (keyName == "local_pos")
                {
                    StringConverter::parseFloat3(valueName, currentTyre->localPos.X, currentTyre->localPos.Y, currentTyre->localPos.Z);
                } else if (keyName == "mass")
                {
                    currentTyre->mass = StringConverter::parseFloat(valueName, 30.f);
                } else if (keyName == "friction")
                {
                    currentTyre->friction = StringConverter::parseFloat(valueName, 0.8f);
                } else if (keyName == "suspension_length")
                {
                    currentTyre->suspensionLength = StringConverter::parseFloat(valueName, 0.42f);
                } else if (keyName == "suspension_spring")
                {
                    currentTyre->suspensionSpring = StringConverter::parseFloat(valueName, 30.0f);
                } else if (keyName == "suspension_damper")
                {
                    currentTyre->suspensionDamper = StringConverter::parseFloat(valueName, 9.0f);
                } else if (keyName == "steerable")
                {
                    currentTyre->steerable = StringConverter::parseBool(valueName, false);
                } else if (keyName == "torqueable")
                {
                    currentTyre->torqueable = StringConverter::parseBool(valueName, true);
                } else if (keyName == "handbrakeable")
                {
                    currentTyre->handbrakeable = StringConverter::parseBool(valueName, false);
                }
            } else if (gear)
            {
                if (keyName == "id")
                {
                    gearId = StringConverter::parseUnsignedInt(valueName, 0);
                } else if (keyName == "force")
                {
                    gearForce = StringConverter::parseFloat(valueName, 1.0f);
                }
            }
            else
            {
                if (keyName == "object_name")
                {
                    objectName = valueName;
                } else if (keyName == "long_name")
                {
                    vehicleLongName = valueName;
                } else if (keyName == "texture")
                {
                    texture = TheGame::getInstance()->getDriver()->getTexture(valueName.c_str());
                } else if (keyName == "engine_sound")
                {
                    engineSoundFilename = valueName;
                } else if (keyName == "max_brake_force")
                {
                    maxBrakeForce = StringConverter::parseFloat(valueName, 100.f);
                } else if (keyName == "max_speed")
                {
                    maxSpeed = StringConverter::parseFloat(valueName, 100.f);
                } else if (keyName == "max_torque")
                {
                    maxTorque = StringConverter::parseFloat(valueName, 5000.0f);
                } else if (keyName == "max_torque_rate")
                {
                    maxTorqueRate = StringConverter::parseFloat(valueName, 200.0f);
                } else if (keyName == "change_gear_time")
                {
                    changeGearTime = StringConverter::parseUnsignedInt(valueName, 20);
                } else if (keyName == "max_steer_angle")
                {
                    maxSteerAngle = StringConverter::parseFloat(valueName, 35.0f);
                } else if (keyName == "max_steer_rate")
                {
                    maxSteerRate = StringConverter::parseFloat(valueName, 0.1f);
                } else if (keyName == "view1_forward_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[0][12], viewPos[0][13], viewPos[0][14]);
                } else if (keyName == "view1_left_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[1][12], viewPos[1][13], viewPos[1][14]);
                } else if (keyName == "view1_right_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[2][12], viewPos[2][13], viewPos[2][14]);
                } else if (keyName == "view1_behind_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[3][12], viewPos[3][13], viewPos[3][14]);
                } else if (keyName == "view2_forward_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[4][12], viewPos[4][13], viewPos[4][14]);
                } else if (keyName == "view2_left_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[5][12], viewPos[5][13], viewPos[5][14]);
                } else if (keyName == "view2_right_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[6][12], viewPos[6][13], viewPos[6][14]);
                } else if (keyName == "view2_behind_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[7][12], viewPos[7][13], viewPos[7][14]);
                } else if (keyName == "view3_forward_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[8][12], viewPos[8][13], viewPos[8][14]);
                } else if (keyName == "view3_left_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[9][12], viewPos[9][13], viewPos[9][14]);
                } else if (keyName == "view3_right_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[10][12], viewPos[10][13], viewPos[10][14]);
                } else if (keyName == "view3_behind_pos")
                {
                    StringConverter::parseFloat3(valueName, viewPos[11][12], viewPos[11][13], viewPos[11][14]);
                } else if (keyName == "view1_forward_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[0][12], viewDest[0][13], viewDest[0][14]);
                } else if (keyName == "view1_left_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[1][12], viewDest[1][13], viewDest[1][14]);
                } else if (keyName == "view1_right_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[2][12], viewDest[2][13], viewDest[2][14]);
                } else if (keyName == "view1_behind_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[3][12], viewDest[3][13], viewDest[3][14]);
                } else if (keyName == "view2_forward_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[4][12], viewDest[4][13], viewDest[4][14]);
                } else if (keyName == "view2_left_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[5][12], viewDest[5][13], viewDest[5][14]);
                } else if (keyName == "view2_right_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[6][12], viewDest[6][13], viewDest[6][14]);
                } else if (keyName == "view2_behind_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[7][12], viewDest[7][13], viewDest[7][14]);
                } else if (keyName == "view3_forward_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[8][12], viewDest[8][13], viewDest[8][14]);
                } else if (keyName == "view3_left_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[9][12], viewDest[9][13], viewDest[9][14]);
                } else if (keyName == "view3_right_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[10][12], viewDest[10][13], viewDest[10][14]);
                } else if (keyName == "view3_behind_dest")
                {
                    StringConverter::parseFloat3(valueName, viewDest[11][12], viewDest[11][13], viewDest[11][14]);
                }
            }
        }
        
        if (currentTyre)
        {
            if (currentTyre->id != (unsigned int)-1 && currentTyre->id < 10)
            {
                if (vehicleTypeTyreMap.find(currentTyre->id) == vehicleTypeTyreMap.end())
                {
                    vehicleTypeTyreMap[currentTyre->id] = currentTyre;
                }
                else
                {
                    assert(0 && "two tyres have the same id");
                }
            }
            else
            {
                delete currentTyre;
                currentTyre = 0;
            }
        } else if (gear && gearId > 0)
        {
            if (gearMap.find(gearId) == gearMap.end())
            {
                gearMap[gearId] = gearForce;
            }
            else
            {
                assert(0 && "two gears have the same id");
            }
        }
    }
    if (maxSpeed > maxMaxSpeed) maxMaxSpeed = maxSpeed;
    if (maxBrakeForce > maxMaxBrakeForce) maxMaxBrakeForce = maxBrakeForce;
    if (maxTorque > maxMaxTorque) maxMaxTorque = maxTorque;
    if (maxSteerAngle > maxMaxSteerAngle) maxMaxSteerAngle = maxSteerAngle;
    return true;
}

