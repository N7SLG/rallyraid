

#include "Player.h"
#include "Vehicle.h"
#include "Competitor.h"
#include "RaceEngine.h"
#include "ItinerPoint.h"
#include "Settings.h"
#include "GamePlay_defs.h"
#include "WStringConverter.h"
#include "MessageManager.h"
#include <assert.h>


Player* Player::player = 0;
ItinerManager::itinerPointList_t Player::helperItinerPointList;


void Player::initialize()
{
    if (player == 0)
    {
        player = new Player();
    }
}

void Player::finalize()
{
    if (player)
    {
        delete player;
        player = 0;
    }
}

    
Player::Player()
    : vehicle(0),
      competitor(new Competitor(499, "Player", "-", "Player's Team", "vw3", 100, false)),
      starter(0),
      viewNum(VIEW_0),
      viewMask(VIEW_CENTER),
      recenterView(true),
      firstPressed(false),
      distance(0.f),
      savedDistance(0.f),
      lastVehicleDistance(0.f),
      savedVehicleDistance(0.f),
      stageTime(0),
      savedStageTime(0),
      stagePenaltyTime(0),
      savedStagePenaltyTime(0),
      suspensionSpringModifier(0.0f),
      suspensionDamperModifier(0.0f),
      brakeBalance(0.2f),
      loaded(false),
      savedPos(),
      savedRot(),
      savedSpeed(0.0f),
      savedCondition(1.0f),
      savedConditionSteer(1.0f),
      prevItinerIt(helperItinerPointList.end()),
      currItinerIt(helperItinerPointList.end()),
      savedPrevItinerIt(helperItinerPointList.end()),
      savedCurrItinerIt(helperItinerPointList.end()),
      passedWayPoints(),
      savedPassedWayPoints()
{
}

Player::~Player()
{
    finalizeVehicle();
    delete competitor;
}


void Player::initializeVehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation, Stage* stage)
{
    assert(vehicle == 0);
    competitor->setVehicleTypeName(vehicleTypeName);
    vehicle = new Vehicle(vehicleTypeName, apos, rotation, Settings::getInstance()->manualGearShifting,
        Settings::getInstance()->sequentialGearShifting, suspensionSpringModifier, suspensionDamperModifier, brakeBalance,
        savedCondition, savedConditionSteer);
    recenterView = true;
    firstPressed = false;
    distance = savedDistance;
    lastVehicleDistance = savedVehicleDistance;
    vehicle->setDistance(savedVehicleDistance);
    vehicle->setVehicleCollisionCB(this);
    if (starter)
    {
        starter->vehicle = vehicle;
    }
    stageTime = savedStageTime;
    stagePenaltyTime = savedStagePenaltyTime;
    if (loaded)
    {
        prevItinerIt = savedPrevItinerIt;
        currItinerIt = savedCurrItinerIt;
        irr::core::vector3df dir(1.0f, 0.0f, 0.0f);
        dir = rotation.rotationToDirection(dir);
        vehicle->addStartImpulse(savedSpeed, dir);
    }
    else
    {
        resetItiner(stage);
    }
    passedWayPoints = savedPassedWayPoints;
    savedDistance = 0.f;
    savedVehicleDistance = 0.f;
    savedPrevItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    savedCurrItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    savedPassedWayPoints.clear();
    savedStageTime = 0;
    savedStagePenaltyTime = 0;
    savedCondition = 1.0f;
    savedConditionSteer = 1.0f;
    loaded = false;
}

void Player::finalizeVehicle()
{
    if (vehicle)
    {
        delete vehicle;
        vehicle = 0;
    }
}

bool Player::save(const std::string& filename)
{
    FILE* f;
    int ret = 0;
    Stage* stage = RaceManager::getInstance()->getCurrentStage();

    errno_t error = fopen_s(&f, filename.c_str(), "w");
    if (error)
    {
        printf("unable to open player file for write %s\n", filename.c_str());
        return false;
    }

    std::string nameToSave = competitor->getName();
    for (unsigned int i = 0; i < nameToSave.length(); i++) if (nameToSave[i] == ' ') nameToSave[i] = '¤';
    ret = fprintf(f, "%s\n", nameToSave.c_str());

    nameToSave = competitor->getCoName();
    for (unsigned int i = 0; i < nameToSave.length(); i++) if (nameToSave[i] == ' ') nameToSave[i] = '¤';
    ret = fprintf(f, "%s\n", nameToSave.c_str());

    nameToSave = competitor->getTeamName();
    for (unsigned int i = 0; i < nameToSave.length(); i++) if (nameToSave[i] == ' ') nameToSave[i] = '¤';
    ret = fprintf(f, "%s\n", nameToSave.c_str());

    ret = fprintf(f, "%s\n", competitor->getVehicleTypeName().c_str());
    ret = fprintf(f, "%f\n", lastVehicleDistance);
    ret = fprintf(f, "%f\n", distance);
    
    ret = fprintf(f, "%u\n", (stage && prevItinerIt!=stage->getItinerPointList().end())?(*prevItinerIt)->getNum():0);
    ret = fprintf(f, "%u\n", (stage && currItinerIt!=stage->getItinerPointList().end())?(*currItinerIt)->getNum():0);
    ret = fprintf(f, "%u\n", stageTime);
    ret = fprintf(f, "%u\n", stagePenaltyTime);
    ret = fprintf(f, "%f\n", suspensionSpringModifier);
    ret = fprintf(f, "%f\n", suspensionDamperModifier);
    ret = fprintf(f, "%f\n", brakeBalance);
    ret = fprintf(f, "%f %f %f\n", savedPos.X, savedPos.Y, savedPos.Z);
    ret = fprintf(f, "%f %f %f\n", savedRot.X, savedRot.Y, savedRot.Z);
    ret = fprintf(f, "%f\n", savedSpeed);
    
    ret = fprintf(f, "%lu\n", passedWayPoints.size());
    for (WayPointManager::wayPointNumSet_t::const_iterator it = passedWayPoints.begin();
         it != passedWayPoints.end() && ret;
         it++)
    {
        ret = fprintf(f, "%u\n", *it);
    }


    ret = fprintf(f, "%f\n", vehicle->getCondition());
    ret = fprintf(f, "%f\n", vehicle->getConditionSteer());
    ret = fprintf(f, "%u\n", viewNum);

    fclose(f);

    return (ret > 0);
}

bool Player::load(const std::string& filename, Stage* stage)
{
    FILE* f;
    int ret = 0;
    char name[256];
    char coName[256];
    char teamName[256];
    char vehicleTypeName[256];
    unsigned int prevItinerNum = 0;
    unsigned int currItinerNum = 0;
    unsigned int num = 0;
    unsigned long numOfPWP = 0;
    
    loaded = false;

    errno_t error = fopen_s(&f, filename.c_str(), "r");
    if (error)
    {
        printf("player file unable to open: %s\n", filename.c_str());
        return false;
    }
    
    ret = fscanf_s(f, "%s\n", name, 255);
    if (ret < 1)
    {
        printf("player file unable to read player name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%s\n", coName, 255);
    if (ret < 1)
    {
        printf("player file unable to read co-pilot name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%s\n", teamName, 255);
    if (ret < 1)
    {
        printf("player file unable to read team name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%s\n", vehicleTypeName, 255);
    if (ret < 1)
    {
        printf("player file unable to read vehicle type name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    printf("vehicleTypeName: %s\n", vehicleTypeName);
    ret = fscanf_s(f, "%f\n%f\n", &savedVehicleDistance, &savedDistance);
    if (ret < 2)
    {
        printf("player file unable to read distances: %s, ret: %d\n", filename.c_str(), ret);
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%u\n%u\n", &prevItinerNum, &currItinerNum);
    if (ret < 2)
    {
        printf("player file unable to read itiner points' num: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    savedPrevItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    savedCurrItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    if (stage)
    {
        ItinerManager::itinerPointList_t::const_iterator it = stage->getItinerPointList().begin();
        savedPrevItinerIt = stage->getItinerPointList().end();
        savedCurrItinerIt = stage->getItinerPointList().end();
        for (; it != stage->getItinerPointList().end(); it++)
        {
            if (prevItinerNum == (*it)->getNum())
            {
                savedPrevItinerIt = it;
            }
            if (currItinerNum == (*it)->getNum())
            {
                savedCurrItinerIt = it;
            }
        }
    }

    ret = fscanf_s(f, "%u\n%u\n", &savedStageTime, &savedStagePenaltyTime);
    if (ret < 2)
    {
        printf("player file unable to read stage time and stage penalty time: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%f\n%f\n%f\n", &suspensionSpringModifier, &suspensionDamperModifier, &brakeBalance);
    if (ret < 3)
    {
        printf("player file unable to read suspension modifiers and brake balance: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    ret = fscanf_s(f, "%f %f %f\n", &savedPos.X, &savedPos.Y, &savedPos.Z);
    if (ret < 3)
    {
        printf("player file unable to read position: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    ret = fscanf_s(f, "%f %f %f\n", &savedRot.X, &savedRot.Y, &savedRot.Z);
    if (ret < 3)
    {
        printf("player file unable to read rotation: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    ret = fscanf_s(f, "%f\n", &savedSpeed);
    if (ret < 1)
    {
        printf("player file unable to read speed: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    savedPassedWayPoints.clear();
    ret = fscanf_s(f, "%lu\n", &numOfPWP);
    if (ret < 1)
    {
        printf("player file unable to read number of WPs: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    
    for (;numOfPWP > 0 && ret; numOfPWP--)
    {
        ret = fscanf_s(f, "%u\n", &num);
        if (ret < 1)
        {
            printf("player file unable to read WP num: %s\n", filename.c_str());
            fclose(f);
            return false;
        }
        savedPassedWayPoints.insert(num);
    }

    ret = fscanf_s(f, "%f\n", &savedCondition);
    if (ret < 1)
    {
        printf("player file unable to read condition: %s, could be normal if using old savegame\n", filename.c_str());
        savedCondition = 1.0f;
    }

    ret = fscanf_s(f, "%f\n", &savedConditionSteer);
    if (ret < 1)
    {
        printf("player file unable to read condition steer: %s, could be normal if using old savegame\n", filename.c_str());
        savedConditionSteer = 1.0f;
    }

    ret = fscanf_s(f, "%u\n", &viewNum);
    if (ret < 1)
    {
        printf("player file unable to read view num: %s, could be normal if using old savegame\n", filename.c_str());
        viewNum = VIEW_0;
    }

    fclose(f);
    for (unsigned int i = 0; i < strlen(name); i++) if (name[i] == '¤') name[i] = ' ';
    competitor->setName(name);
    for (unsigned int i = 0; i < strlen(coName); i++) if (coName[i] == '¤') coName[i] = ' ';
    competitor->setCoName(coName);
    for (unsigned int i = 0; i < strlen(teamName); i++) if (teamName[i] == '¤') teamName[i] = ' ';
    competitor->setTeamName(teamName);
    competitor->setVehicleTypeName(vehicleTypeName);
    loaded = true;
    return true;
}

void Player::resetVehicle(const irr::core::vector3df& newPos)
{
    if (vehicle)
    {
        vehicle->reset(newPos);
        recenterView = true;
        
        if (starter)
        {
            unsigned int penalty = (RESET_PENALTY - (15*Settings::getInstance()->difficulty));
            starter->penaltyTime += penalty;
            irr::core::stringw str = L"Add ";
            WStringConverter::addTimeToStr(str, penalty);
            str += L" penality, because of restore the car.";
            MessageManager::getInstance()->addText(str.c_str(), 2);
        }
    }
}

void Player::repairVehicle()
{
    if (vehicle)
    {
        if (starter)
        {
            unsigned int penalty = (REPAIR_PENALTY-Settings::getInstance()->difficulty)*vehicle->getDamagePercentage();
            if (penalty)
            {
                starter->penaltyTime += penalty;
                irr::core::stringw str = L"Add ";
                WStringConverter::addTimeToStr(str, penalty);
                str += L" penality, because of repair the car.";
                MessageManager::getInstance()->addText(str.c_str(), 2);
            }
        }
        vehicle->repair();
    }
}

void Player::handleHardCollision(float w)
{
    dprintf(MY_DEBUG_NOTE, "Player collision, w: %f\n", w);
    assert(vehicle);
    if (starter)
    {
        if (w < 0.001f) w = 1.0f; // totally front hit, the AI does not do it normally

        if (w > 0.09f)
        {
            unsigned int penalty = (unsigned int)(w*(float)(COLLISION_PENALTY - (15*Settings::getInstance()->difficulty)));
            starter->penaltyTime += penalty;
            irr::core::stringw str = L"Add ";
            WStringConverter::addTimeToStr(str, penalty);
            str += L" penality, because of hitting the opponent's car.";
            MessageManager::getInstance()->addText(str.c_str(), 2);
        }
    }
}

void Player::handleSoftCollision(float w)
{
    assert(vehicle);
}
