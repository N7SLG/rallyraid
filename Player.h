
#ifndef PLAYER_H
#define PLAYER_H

#include <irrlicht.h>
#include <string>
#include "Player_defs.h"
#include "Vehicle.h"
#include "ItinerManager.h"
#include "RaceManager.h"
#include "Stage.h"
#include "WayPointManager.h"
#include "OffsetManager.h"
//#include <assert.h>

//class VehicleType;
class Competitor;
class Starter;
//class Stage;

class Player : public VehicleCollisionCB
{
public:
    static void initialize();
    static void finalize();
    
    static Player* getInstance() {return player;}

private:
    static Player* player;

private:
    Player();
    ~Player();

public:
    void initializeVehicle(const std::string& vehicleTypeName, const irr::core::vector3df& apos, const irr::core::vector3df& rotation, Stage* stage);
    void finalizeVehicle();

    bool save(const std::string& filename);
    bool load(const std::string& filename, Stage* stage);

    Vehicle* getVehicle(); // inline
    Competitor* getCompetitor(); // inline
    Starter* getStarter(); // inline
    void setStarter(Starter* starter); // inline
    bool getFirstPressed() const; // inline
    bool isFirstPressed() const; // inline
    void setFirstPressed(); // inline

    const irr::core::matrix4& getViewPos() const; // inline
    const irr::core::matrix4& getViewDest() const; // inline
    const irr::core::matrix4& getVehicleMatrix() const; // inline
    float getVehicleSpeed() const; // inline
    int getVehicleGear() const; // inline

    unsigned int getViewNum() const; // inline
    unsigned int getViewMask() const; // inline
    bool getRecenterView() const; // inline
    void clearRecenterView(); // inline
    void lookLeft(bool set); // inline
    void lookRight(bool set); // inline
    void lookCenter(bool set); // inline
    void switchToNextView(); // inline
    void resetVehicle(const irr::core::vector3df& newPos);

    float getDistance() const; // inline
    void resetDistance(); // inline
    void update(); // inline
    
    void stepItiner(); // inline
    void stepBackItiner(); // inline
    const ItinerManager::itinerPointList_t::const_iterator& getPrevItiner(); // inline
    const ItinerManager::itinerPointList_t::const_iterator& getCurrItiner(); // inline
    bool isPrevItinerValid(); //inline
    bool isCurrItinerValid(); //inline
    bool isItinerValid(const ItinerManager::itinerPointList_t::const_iterator& itinerIt); //inline

    unsigned int getStageTime(); // inline
    unsigned int getStagePenaltyTime(); // inline
    void setStageTime(unsigned int stageTime, unsigned int stagePenaltyTime); // inline

    float getSuspensionSpringModifier() const; // inline
    float getSuspensionDamperModifier() const; // inline
    float getBrakeBalance() const; // inline
    void setSuspensionSpringModifier(float suspensionSpringModifier); // inline
    void setSuspensionDamperModifier(float suspensionDamperModifier); // inline
    void setBrakeBalance(float brakeBalance); // inline

    bool addPassedWayPointNum(unsigned int wpNum); // inline
    bool isPassedWayPointNum(unsigned int wpNum) const; // inline
    size_t getPassedWayPointsCount() const; // inline

    const irr::core::vector3df& getSavedPos() const; // inline
    const irr::core::vector3df& getSavedRot() const; // inline

private:
    virtual void handleHardCollision(float w);
    virtual void handleSoftCollision(float w);

private:
    Vehicle*        vehicle;
    Competitor*     competitor;
    Starter*        starter;
    unsigned int    viewNum;
    unsigned int    viewMask;
    bool            recenterView;
    bool            firstPressed;
    float           distance;
    float           savedDistance;
    float           lastVehicleDistance;
    float           savedVehicleDistance;
    unsigned int    stageTime;
    unsigned int    savedStageTime;
    unsigned int    stagePenaltyTime;
    unsigned int    savedStagePenaltyTime;
    float           suspensionSpringModifier;
    float           suspensionDamperModifier;
    float           brakeBalance;
    bool            loaded;
    irr::core::vector3df savedPos;
    irr::core::vector3df savedRot;
    float           savedSpeed;

    ItinerManager::itinerPointList_t::const_iterator prevItinerIt;
    ItinerManager::itinerPointList_t::const_iterator currItinerIt;
    ItinerManager::itinerPointList_t::const_iterator savedPrevItinerIt;
    ItinerManager::itinerPointList_t::const_iterator savedCurrItinerIt;
    static ItinerManager::itinerPointList_t helperItinerPointList;
    //ItinerManager::itinerList_t::const_iterator nextItinerIt;

    WayPointManager::wayPointNumSet_t passedWayPoints;
    WayPointManager::wayPointNumSet_t savedPassedWayPoints;
};

inline Vehicle* Player::getVehicle()
{
    return vehicle;
}

inline Competitor* Player::getCompetitor()
{
    return competitor;
}

inline Starter* Player::getStarter()
{
    return starter;
}

inline void Player::setStarter(Starter* starter)
{
    this->starter = starter;
    if (vehicle)
    {
        if (starter)
        {
            vehicle->setVehicleCollisionCB(this);
        }
        else
        {
            vehicle->setVehicleCollisionCB(0);
        }
    }
}

inline bool Player::getFirstPressed() const
{
    return firstPressed;
}

inline bool Player::isFirstPressed() const
{
    return firstPressed;
}

inline void Player::setFirstPressed()
{
    firstPressed = true;
}

inline const irr::core::matrix4& Player::getViewPos() const
{
    //assert(vehicle);
    return vehicle->getViewPos(CALC_VIEW(viewNum, viewMask));
}

inline const irr::core::matrix4& Player::getViewDest() const
{
    //assert(vehicle);
    return vehicle->getViewDest(CALC_VIEW(viewNum, viewMask));
}

inline const irr::core::matrix4& Player::getVehicleMatrix() const
{
    //assert(vehicle);
    return vehicle->getMatrix();
}

inline float Player::getVehicleSpeed() const
{
    //assert(vehicle);
    return vehicle->getSpeed();
}
inline int Player::getVehicleGear() const
{
    //assert(vehicle);
    return vehicle->getGear();
}

inline unsigned int Player::getViewNum() const
{
    return viewNum;
}

inline unsigned int Player::getViewMask() const
{
    return viewMask;
}

inline bool Player::getRecenterView() const
{
    return recenterView;
}

inline void Player::clearRecenterView()
{
    recenterView = false;
}

inline void Player::lookLeft(bool set)
{
    if (set)
    {
        viewMask |= VIEW_LEFT;
    }
    else
    {
        viewMask &= ~VIEW_LEFT;
    }
    recenterView = true;
}

inline void Player::lookRight(bool set)
{
    if (set)
    {
        viewMask |= VIEW_RIGHT;
    }
    else
    {
        viewMask &= ~VIEW_RIGHT;
    }
    recenterView = true;
}

inline void Player::lookCenter(bool set)
{
    viewMask = VIEW_CENTER;
    recenterView = true;
}

inline void Player::switchToNextView()
{
    SWITCH_TO_NEXT_VIEW(viewNum);
    recenterView = true;
}

inline float Player::getDistance() const
{
    return distance;
}

inline void Player::resetDistance()
{
    distance = 0.f;
}

inline void Player::update()
{
    if (vehicle==0) return;

    float vehicleDistance = vehicle->getDistance();
    distance += vehicleDistance - lastVehicleDistance;
    lastVehicleDistance = vehicleDistance;
    savedPos = vehicle->getMatrix().getTranslation() + OffsetManager::getInstance()->getOffset();
    savedRot = vehicle->getMatrix().getRotationDegrees();
    savedSpeed = vehicle->getSpeed();
}

inline void Player::stepItiner()
{
    //Stage* stage = RaceManager::getInstance()->getCurrentStage();
    //printf("try stepItiner\n");
    if (isCurrItinerValid()/*stage && currItinerIt != stage->getItinerPointList().end()*/)
    {
        //printf("stepItiner\n");
        prevItinerIt = currItinerIt;
        currItinerIt++;
    }
}

inline void Player::stepBackItiner()
{
    Stage* stage = RaceManager::getInstance()->getCurrentStage();
    //printf("try stepBackItiner\n");
    if (isPrevItinerValid()/*stage && currItinerIt != stage->getItinerPointList().end()*/)
    {
        //printf("stepBackItiner\n");
        currItinerIt = prevItinerIt;
        if (prevItinerIt != stage->getItinerPointList().begin())
        {
            prevItinerIt--;
        }
        else
        {
            prevItinerIt = stage->getItinerPointList().end();
        }
    }
}

inline const ItinerManager::itinerPointList_t::const_iterator& Player::getPrevItiner()
{
    return prevItinerIt;
}

inline const ItinerManager::itinerPointList_t::const_iterator& Player::getCurrItiner()
{
    return currItinerIt;
}

inline bool Player::isPrevItinerValid()
{
    return isItinerValid(prevItinerIt);
}

inline bool Player::isCurrItinerValid()
{
    return isItinerValid(currItinerIt);
}

inline bool Player::isItinerValid(const ItinerManager::itinerPointList_t::const_iterator& itinerIt)
{
    Stage* stage = RaceManager::getInstance()->getCurrentStage();
    return (stage!=0 && itinerIt != stage->getItinerPointList().end());
}

inline bool Player::addPassedWayPointNum(unsigned int wpNum)
{
    return passedWayPoints.insert(wpNum).second;
}

inline bool Player::isPassedWayPointNum(unsigned int wpNum) const
{
    return passedWayPoints.count(wpNum) > 0;
}

inline size_t Player::getPassedWayPointsCount() const
{
    return passedWayPoints.size();
}
inline unsigned int Player::getStageTime()
{
    return stageTime;
}

inline unsigned int Player::getStagePenaltyTime()
{
    return stagePenaltyTime;
}

inline void Player::setStageTime(unsigned int stageTime, unsigned int stagePenaltyTime)
{
    this->stageTime = stageTime;
    this->stagePenaltyTime = stagePenaltyTime;
}

inline float Player::getSuspensionSpringModifier() const
{
    return suspensionSpringModifier;
}

inline float Player::getSuspensionDamperModifier() const
{
    return suspensionDamperModifier;
}

inline float Player::getBrakeBalance() const
{
    return brakeBalance;
}

inline void Player::setSuspensionSpringModifier(float suspensionSpringModifier)
{
    this->suspensionSpringModifier = suspensionSpringModifier;
    if (vehicle)
    {
        vehicle->modifySuspensionSpring(suspensionSpringModifier);
    }
}

inline void Player::setSuspensionDamperModifier(float suspensionDamperModifier)
{
    this->suspensionDamperModifier = suspensionDamperModifier;
    if (vehicle)
    {
        vehicle->modifySuspensionDamper(suspensionDamperModifier);
    }
}

inline void Player::setBrakeBalance(float brakeBalance)
{
    this->brakeBalance = brakeBalance;
    if (vehicle)
    {
        vehicle->modifyBrakeBalance(brakeBalance);
    }
}

inline const irr::core::vector3df& Player::getSavedPos() const
{
    return savedPos;
}

inline const irr::core::vector3df& Player::getSavedRot() const
{
    return savedRot;
}

#endif // PLAYER_H
