

#include "Player.h"
#include "Vehicle.h"
#include "Competitor.h"
#include "RaceEngine.h"
#include "ItinerPoint.h"
#include "Settings.h"
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
      competitor(new Competitor(499, "Player", "-", "Player's Team", "vw3", 0, false)),
      starter(0),
      viewNum(VIEW_0),
      viewMask(VIEW_CENTER),
      recenterView(true),
      firstPressed(false),
      distance(0.f),
      savedDistance(0.f),
      lastVehicleDistance(0.f),
      savedVehicleDistance(0.f),
      prevItinerIt(helperItinerPointList.end()),
      currItinerIt(helperItinerPointList.end()),
      savedPrevItinerIt(helperItinerPointList.end()),
      savedCurrItinerIt(helperItinerPointList.end()),
      passedWayPoints(),
      savedPassedWayPoints(),
      stageTime(0),
      savedStageTime(0),
      suspensionSpringModifier(0.0f),
      suspensionDamperModifier(0.0f)
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
        Settings::getInstance()->sequentialGearShifting, suspensionSpringModifier, suspensionDamperModifier);
    recenterView = true;
    firstPressed = false;
    distance = savedDistance;
    lastVehicleDistance = savedVehicleDistance;
    vehicle->setDistance(savedVehicleDistance);
    if (savedPrevItinerIt!=helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/)
    {
        prevItinerIt = savedPrevItinerIt;
    }
    else
    {
        if (stage)
        {
            prevItinerIt = stage->getItinerPointList().end();
            /*printf("start test: p = e, begin: %u, end: %u\n", prevItinerIt == stage->getItinerPointList().begin(), prevItinerIt == stage->getItinerPointList().end());
            prevItinerIt = stage->getItinerPointList().begin();
            printf("start test: p = b, begin: %u, end: %u\n", prevItinerIt == stage->getItinerPointList().begin(), prevItinerIt == stage->getItinerPointList().end());
            prevItinerIt--;
            printf("start test: p--, begin: %u, end: %u\n", prevItinerIt == stage->getItinerPointList().begin(), prevItinerIt == stage->getItinerPointList().end());
            assert(0 && "end test");*/
        }
        else
        {
            prevItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
        }
    }
    
    if (savedCurrItinerIt!=helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/)
    {
        currItinerIt = savedCurrItinerIt;
    }
    else
    {
        if (stage)
        {
            currItinerIt = stage->getItinerPointList().begin();
        }
        else
        {
            currItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
        }
    }
    passedWayPoints = savedPassedWayPoints;
    savedDistance = 0.f;
    savedVehicleDistance = 0.f;
    savedPrevItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    savedCurrItinerIt = helperItinerPointList.end()/*ItinerManager::itinerPointList_t::const_iterator()*/;
    savedPassedWayPoints.clear();
    if (savedStageTime > 0)
    {
        stageTime = savedStageTime;
    }
    savedStageTime = 0;
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

    ret = fprintf(f, "%s\n", competitor->getName().c_str());
    ret = fprintf(f, "%s\n", competitor->getTeamName().c_str());
    ret = fprintf(f, "%s\n", competitor->getVehicleTypeName().c_str());
    ret = fprintf(f, "%f\n", lastVehicleDistance);
    ret = fprintf(f, "%f\n", distance);
    
    ret = fprintf(f, "%u\n", (stage && prevItinerIt!=stage->getItinerPointList().end())?(*prevItinerIt)->getNum():0);
    ret = fprintf(f, "%u\n", (stage && currItinerIt!=stage->getItinerPointList().end())?(*currItinerIt)->getNum():0);
    ret = fprintf(f, "%u\n", stageTime);
    ret = fprintf(f, "%f\n", suspensionSpringModifier);
    ret = fprintf(f, "%f\n", suspensionDamperModifier);
    
    ret = fprintf(f, "%lu\n", passedWayPoints.size());
    for (WayPointManager::wayPointNumSet_t::const_iterator it = passedWayPoints.begin();
         it != passedWayPoints.end() && ret;
         it++)
    {
        ret = fprintf(f, "%u\n", *it);
    }
    

    fclose(f);

    return (ret > 0);
}

bool Player::load(const std::string& filename, Stage* stage)
{
    FILE* f;
    int ret = 0;
    char name[256];
    char teamName[256];
    char vehicleTypeName[256];
    unsigned int prevItinerNum = 0;
    unsigned int currItinerNum = 0;
    unsigned int num = 0;
    unsigned long numOfPWP = 0;
    

    errno_t error = fopen_s(&f, filename.c_str(), "r");
    if (error)
    {
        printf("player file unable to open: %s\n", filename.c_str());
        return false;
    }
    
    ret = fscanf_s(f, "%s\n", name);
    if (ret < 1)
    {
        printf("player file unable to read player name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%s\n", teamName);
    if (ret < 1)
    {
        printf("player file unable to read team name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%s\n", vehicleTypeName);
    if (ret < 1)
    {
        printf("player file unable to read vehicle type name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%f\n%f\n", &savedVehicleDistance, &savedDistance);
    if (ret < 2)
    {
        printf("player file unable to read distances: %s\n", filename.c_str());
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

    ret = fscanf_s(f, "%u\n", &savedStageTime);
    if (ret < 1)
    {
        printf("player file unable to read stage time: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    ret = fscanf_s(f, "%f\n%f\n", &suspensionSpringModifier, &suspensionDamperModifier);
    if (ret < 2)
    {
        printf("player file unable to read suspension modifiers: %s\n", filename.c_str());
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

    fclose(f);
    competitor->setName(name);
    competitor->setTeamName(teamName);
    competitor->setVehicleTypeName(vehicleTypeName);
    return true;
}

