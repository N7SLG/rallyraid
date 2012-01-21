
#include "RaceEngine.h"
#include "Settings.h"

#include "GamePlay.h"
#include "GamePlay_defs.h"
#include "TheGame.h"
#include "FontManager.h"
#include "stdafx.h"
#include "RaceManager.h"
#include "Race.h"
#include "Day.h"
#include "Stage.h"
#include "Competitor.h"
#include "Vehicle.h"
#include "VehicleType.h"
#include "VehicleTypeManager.h"
#include "OffsetObject.h"
#include "OffsetManager.h"
#include "WStringConverter.h"
#include "MessageManager.h"
#include "Player.h"
#include "AIPoint.h"
#include <assert.h>


#define FAR_VALUE (800.f)
#define NEAR_VALUE (500.f)
#ifdef USE_EDITOR
# define START_SECS 10
#else
# define START_SECS 60
#endif
#define REACHED_POINT_DIST      15.f
#define REACHED_POINT_DIST_NEAR 20.f
#define ANGLE_LIMIT             45.f
#define ANGLE_LIMIT_MAX         (ANGLE_LIMIT * 3.f)
#define ANGLE_LIMIT_MIN         10.f
#define SPEED_ANGLE_LIMIT_MAX   120.f
#define SPEED_ANGLE_LIMIT_MIN   20.f
#define BRAKE_SPEED_LIMIT_MIN   30.f
#define BRAKE_SPEED_LIMIT_MAX   110.f
#define SPEED_ANGLE_LIMIT_MAX_SPEED     BRAKE_SPEED_LIMIT_MIN
#define SPEED_ANGLE_LIMIT_MIN_SPEED     150.f
#define AI_STEP                 (1.15f) // orig: (1.05f)


// normalize angle between 0 and 360
static float normalizeAngle(float &angle)
{
    while (angle >= 360.f) angle -= 360.f;
    while (angle < 0.f) angle += 360.f;
    return angle;
}

static float normalizeAngle180(float &angle)
{
    while (angle > 180.f) angle -= 360.f;
    while (angle < -180.f) angle += 360.f;
    return angle;
}

static float calcDesiredSpeed(float angleToNextAbs, float speedMax)
{
    //const float angleLimit = SPEED_ANGLE_LIMIT;           // for the steer calculation
    //const float angleLimitMax = SPEED_ANGLE_LIMIT_MAX;    // for the desired speed calculation
    //const float angleLimitMin = SPEED_ANGLE_LIMIT_MIN;    // for the desired speed calculation

    const float speedRange = SPEED_ANGLE_LIMIT_MIN_SPEED - SPEED_ANGLE_LIMIT_MAX_SPEED;
    const float angleRange = SPEED_ANGLE_LIMIT_MAX - SPEED_ANGLE_LIMIT_MIN;

    float desiredSpeed = SPEED_ANGLE_LIMIT_MIN_SPEED - (((angleToNextAbs - SPEED_ANGLE_LIMIT_MIN)/angleRange)*speedRange);//(speedMax - angleToNextAbs + angleLimitMin);
    
    if (desiredSpeed < SPEED_ANGLE_LIMIT_MAX_SPEED) desiredSpeed = SPEED_ANGLE_LIMIT_MAX_SPEED;

    return desiredSpeed;
    /*
    //const float angleLimitMax = 180.f;
    const float brakeSpeedLimitMin = BRAKE_SPEED_LIMIT_MIN;             // for the desired speed calculation
    const float brakeSpeedLimitMax = speedMax; //BRAKE_SPEED_LIMIT_MAX;             // for the desired speed calculation
    const float angleLimitSize = angleLimitMax - angleLimitMin;
    const float brakeSpeedLimitSize = brakeSpeedLimitMax - brakeSpeedLimitMin;

    const float low = 0.5f;
    const float high = 1 / low; //2.0f;
    const float size = high - low;

    float x = ((angleToNextAbs - angleLimitMin) * (size / angleLimitSize)) + low;

    float y = ((1 / x) - low);
//    float x = ((angleLimitMax - angleToNextAbs) * (size / angleLimitSize)) + low;

//    float y = -((1 / x) - low - size);

    return brakeSpeedLimitMin + ((y * brakeSpeedLimitSize) / size);
    */
}


Starter::Starter(Stage* stage,
                 RaceEngine* raceEngine,
                 Competitor* competitor,
                 unsigned int startingCD,
                 unsigned int place,
                 unsigned int globalTime,
                 unsigned int globalPenaltyTime
)
    : raceEngine(raceEngine), stage(stage), competitor(competitor),
      globalTime(globalTime), globalPenaltyTime(globalPenaltyTime),
      startingCD(startingCD), startTime(0), finishTime(0), penaltyTime(0),
      prevPointNum(0), nextPointNum(0), currentPos(),
      passedDistance(0.f), distanceStep(0.f), 
      crashedForever(false), crashTime(0),
      prevPoint(stage->getAIPointList().begin()),
      nextPoint(stage->getAIPointList().begin()),
      visible(false), vehicle(0),
      forResetCnt(0), forBigResetCnt(0),
      currentRand(0.f), nameText(0), nameTextOffsetObject(0),
      dir(), stageRand(0.f), lastCrashUpdate(0),
      lastAngleToNext(0.0f), lastAngleToNextAbs(180.f),
      collisionCD(0)
{
    const static float placeHardening[3] = {20.f, 8.f, 4.f};
    irr::core::stringw namePlusCar = L"";
    namePlusCar += competitor->getNum();
    namePlusCar += L" ";
    namePlusCar += (competitor->getName() + " (" + VehicleTypeManager::getInstance()->getVehicleType(competitor->getVehicleTypeName())->getLongName() + ")").c_str();
    nameText = TheGame::getInstance()->getSmgr()->addTextSceneNode(
        /*env->getBuiltInFont()*/ FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_10PX),
        namePlusCar.c_str(),
        irr::video::SColor(255, 255, 255, 0));
    //nameText->setMaterialType((video::E_MATERIAL_TYPE)myMaterialType_transp_road);
    //nameText->setScale(irr::core::vector3df(3.0f, 3.0f, 3.0f));
    nameText->setVisible(false);
    if (place < 3)
    {
        stageRand = ((float)(rand() % 100) - 50.f - placeHardening[place]) * ((110.f - (float)competitor->getStrength())/200.f);
    }
    else
    {
        stageRand = ((float)(rand() % 100) - 50.f) * ((110.f - (float)competitor->getStrength())/200.f);
    }

    if (!competitor->getAi())
    {
        assert(Player::getInstance()->getStarter()==0);
        Player::getInstance()->setStarter(this);
        //vehicle = Player::getInstance()->getVehicle();
    }
}

Starter::~Starter()
{
    if (!competitor->getAi())
    {
        assert(Player::getInstance()->getStarter()==this);
        vehicle = 0;
        visible = false;
        if (Settings::getInstance()->AIPlayer)
        {
            raceEngine->removeUpdater(this);
        }
        Player::getInstance()->setStarter(0);
    }
    if (vehicle)
    {
        delete vehicle;
        vehicle = 0;
    }
    if (nameText)
    {
        nameText->remove();
        nameText = 0;
    }
    if (nameTextOffsetObject)
    {
        nameTextOffsetObject->setNode(0);
        nameTextOffsetObject->removeFromManager();
        delete nameTextOffsetObject;
        nameTextOffsetObject = 0;
    }
}

void Starter::handleHardCollision(float w)
{
    dprintf(MY_DEBUG_NOTE, "AI collision, w: %f\n", w);
    assert(vehicle);
    if (w > 0.001f)
    {
        penaltyTime += unsigned int(w*(float)COLLISION_PENALTY/2);
    }
}

void Starter::handleSoftCollision(float w)
{
    assert(vehicle);
    // do some avoiding stuff
    if (w < 0.0f)
    {
        collisionCD = -6;
    }
    else
    {
        collisionCD = 6;
    }
}

bool Starter::update(unsigned int currentTime, const irr::core::vector3df& apos, bool camActive)
{
    if (finishTime) return false;
    float distFromMe = currentPos.getDistanceFrom(apos);

    if (!competitor->getAi())
    {
        Player::getInstance()->setStageTime((currentTime - startTime), penaltyTime);
        if (!stage->getAIPointList().empty())
        {
            irr::core::vector3df cp(OffsetManager::getInstance()->getOffset()+Player::getInstance()->getVehicleMatrix().getTranslation());
            float distToFinish = cp.getDistanceFrom(stage->getAIPointList().back()->getPos());

            if (distToFinish < 32.f)
            {
                unsigned int missedWPs = (RaceManager::getInstance()->getCurrentStage()->getWayPointList().size() - Player::getInstance()->getPassedWayPointsCount());
                unsigned int WPPenalty =  missedWPs * (WP_MISS_PENALTY - (15*Settings::getInstance()->difficulty));
                unsigned int originalPenalty = penaltyTime;

                penaltyTime += WPPenalty;
                
                finishTime = currentTime - startTime;
                globalTime += finishTime;
                globalPenaltyTime += penaltyTime;

                Player::getInstance()->setStageTime(finishTime, penaltyTime);

                unsigned int position = raceEngine->insertIntoFinishedState(this);
                irr::core::stringw str = L"";
            
                str += competitor->getNum();
                str += L" ";
                str += competitor->getName().c_str();
                str += L" finished the stage, time: ";
                WStringConverter::addTimeToStr(str, finishTime+penaltyTime);
                if (penaltyTime)
                {
                    str += L", including penalty: ";
                    if (originalPenalty)
                    {
                        WStringConverter::addTimeToStr(str, originalPenalty);
                        if (WPPenalty)
                        {
                            str += L" + ";
                        }
                    }
                    if (WPPenalty)
                    {
                        WStringConverter::addTimeToStr(str, WPPenalty);
                        str += L" because of ";
                        str += missedWPs;
                        str += L" missed WPs";
                    }
                }
                str += L",  position: ";
                str += position;
                MessageManager::getInstance()->addText(str.c_str(), 12);
            }
        }
        if (!Settings::getInstance()->AIPlayer)
        {
            return true;
        }
        Player::getInstance()->setFirstPressed();
        // else let the AI drive the car
        //printf("starter fall through, visible: %u, vehicle: %p\n", visible, vehicle);
    }
    
    if (visible && vehicle)
    {
        if (distFromMe > FAR_VALUE || !camActive)
        {
            switchToNotVisible();
            return true;
        }
        
        if (competitor->getAi())
        {
            if (crashedForever || crashTime)
            {
                vehicle->setHandbrake(1.0f);
                vehicle->setSteer(0.1f);
                vehicle->setTorque(0.0f);
            
                if (crashTime && lastCrashUpdate != currentTime)
                {
                    lastCrashUpdate = currentTime;
                    crashTime--;
                }
            
                return true;
            }
            else
            {
                if ((currentTime % competitor->getNum()) == 0)
                {
                    generateRandomFailure(camActive);
                }
            }
        }
        
        // base calculation
        irr::core::vector3df cp(OffsetManager::getInstance()->getOffset()+vehicle->getMatrix().getTranslation());
        currentPos = cp;
        irr::core::vector2df currentPos2d(currentPos.X, currentPos.Z);
        irr::core::vector3df nextPointPos((*nextPoint)->getPos());
        irr::core::vector2df nextPointPos2d(nextPointPos.X, nextPointPos.Z);
        float distToNextPoint = currentPos.getDistanceFrom(nextPointPos);
        if (distToNextPoint < REACHED_POINT_DIST)
        {
            passedDistance = (*nextPoint)->getGlobalDistance();
            goToNextPoint(currentTime, camActive);
            if (finishTime)
            {
                if (competitor->getAi())
                {
                    switchToNotVisible();
                }
                return true;
            }
            nextPointPos = (*nextPoint)->getPos();
            nextPointPos2d = irr::core::vector2df(nextPointPos.X, nextPointPos.Z);
            distToNextPoint = currentPos.getDistanceFrom(nextPointPos);
        }
        
        
        float torque = 1.0f;
        float brake = 0.0f;
        float brake2 = 0.0f;
        float steer = 0.0f;
        float vehicleAngle = vehicle->getAngle(); normalizeAngle(vehicleAngle);
        const float speed = vehicle->getSpeed();
        //const float speedLimitLow = (((m_bigTerrain->getSpeed()-25.f-(float)(difficulty*5)) * ((float)competitor->strength+currentRand+stageRand)) / 100.f);
        const float speedLimitDist = 40.f;
        const float speedLimitLow = (((vehicle->getVehicleType()->getMaxSpeed()-
            speedLimitDist-
            ((float)Settings::getInstance()->difficulty*vehicle->getVehicleType()->getMaxSpeed()/DIFFICULTY_SPEED_STEP_DIVIDER)) *
            ((float)competitor->getStrength()+currentRand+stageRand)) / 100.f);
        const float speedLimitHigh = speedLimitLow + speedLimitDist;
        const float angleLimit = ANGLE_LIMIT;           // for the steer calculation
        const float angleLimitMax = ANGLE_LIMIT_MAX;    // for the desired speed calculation
        const float angleLimitMin = ANGLE_LIMIT_MIN;    // for the desired speed calculation
        const float speedAngleLimitMax = SPEED_ANGLE_LIMIT_MAX;
        const float speedAngleLimitMaxSpeed = SPEED_ANGLE_LIMIT_MAX_SPEED;
        const float speedAngleLimitMin = SPEED_ANGLE_LIMIT_MIN;
        //const float speedAngleLimitMinSpeed = SPEED_ANGLE_LIMIT_MIN_SPEED;
        //const float angleLimitMax = 180.f;
        //const float brakeSpeedLimitMin = BRAKE_SPEED_LIMIT_MIN;         // for the desired speed calculation
        //const float brakeSpeedLimitMax = BRAKE_SPEED_LIMIT_MAX;         // for the desired speed calculation
        
        if (fabsf(speed) < 2.5f)
        {
            forResetCnt++;
            if (forResetCnt >= 20)
            {
                dprintf(MY_DEBUG_INFO, "-------------\nreset AI car %d\nbrc: %u\n------------\n", competitor->getNum(), forBigResetCnt);
                forBigResetCnt++;
                if (forBigResetCnt < 2)
                {
                    penaltyTime += RESET_PENALTY/2;
                    cp += irr::core::vector3df(3.f, 3.f, 3.f); // we don't know why stop: tree or felt over
                }
                else
                {
                    penaltyTime += RESET_PENALTY;
                    cp = nextPointPos;
                    cp.Y += 3.0f;
                    forBigResetCnt = 0;
                }
                vehicle->reset(cp-OffsetManager::getInstance()->getOffset());
                forResetCnt=0;
                currentPos = cp;
                currentPos2d = irr::core::vector2df(currentPos.X, currentPos.Z);
            }
        }
        
        if (vehicle->getDamagePercentage() > 30.f)
        {
            unsigned int damage = vehicle->getDamagePercentage();
            unsigned int penalty = (REPAIR_PENALTY/3)*damage;
            dprintf(MY_DEBUG_INFO, "------------\nrepair AI car %d, penalty: %d\n------------\n", competitor->getNum(), penalty);
            vehicle->repair();
        }
        
        float nextPointAngle = (float)(nextPointPos2d - currentPos2d).getAngle();
        normalizeAngle(nextPointAngle);
        float nextNextPointAngle = nextPointAngle;
        AIPoint::AIPointList_t::const_iterator nextNextPoint = nextPoint;
        nextNextPoint++;
        if (nextNextPoint != stage->getAIPointList().end())
        {
            irr::core::vector2df nextNextPointPos2d((*nextNextPoint)->getPos().X, (*nextNextPoint)->getPos().Z);
            nextNextPointAngle = (float)(nextNextPointPos2d - nextPointPos2d).getAngle();
            normalizeAngle(nextNextPointAngle);
        }
        float angleToNext = nextPointAngle - vehicleAngle; normalizeAngle180(angleToNext);
        float angleToNextNext = nextNextPointAngle - vehicleAngle; normalizeAngle180(angleToNextNext);
        float angleToNextAbs = fabsf(angleToNext);
        float angleToNextNextAbs = fabsf(angleToNextNext);
        
        //pdprintf(printf("nextPoint: %d, va: %f, nToA %f\n", nextPoint, vehicleAngle, angleToNext);)
        //pdprintf(printf("speed: %f, dist: %f, nA %f\n", speed, distToNextPoint, nextPointAngle);)
        //if (competitor->getNum() == 456)
        //{
        //    printf("sp: %d (%d), di: %d, a2n %f, a2n %f\n",
        //        (int)speed, (int)(speedLimitHigh+6.f), (int)distToNextPoint, angleToNext, angleToNextNext);
        //}
        
        // calculate steer
        if (angleToNext > angleLimit)
        {
            // big difference +
            steer = 1.0f;
            //if (competitor->getNum() == 456) printf("s1");
        }
        else
        if (angleToNext < -angleLimit)
        {
            // big difference -
            steer = -1.0f;
            //if (competitor->getNum() == 456) printf("s2");
        }
        else
        if (angleToNextAbs > 1.5f)
        {
            // small difference
            steer = angleToNext / (angleLimit*2.0f);
            if ((lastAngleToNext > 0.0f && angleToNext < 0.0f) ||
                (lastAngleToNext < 0.0f && angleToNext > 0.0f))
            {
                //if (competitor->getNum() == 456) printf("sh");
                steer *= 0.3f; // 0.3
            }
            else
            {
                if (angleToNextAbs > lastAngleToNextAbs)
                {
                    steer *= 3.0f; // 3.0
                    //if (competitor->getNum() == 456) printf("sd");
                    if (steer < -1.0f) steer = -1.0f;
                    if (steer >  1.0f) steer =  1.0f;
                }
                else
                {
                    //if (competitor->getNum() == 456) printf("s3");
                }
            }
        }
        else
        {
            //if (competitor->getNum() == 456) printf("s4");
        }
        lastAngleToNext = angleToNext;
        lastAngleToNextAbs = angleToNextAbs;
        
        float desiredSpeed = speedLimitHigh + 10.f;
        if (speed > desiredSpeed) desiredSpeed = speed;

        // calculate a desired speed to the given angle of next point
        if (angleToNextAbs > speedAngleLimitMin)
        {
            if (angleToNextAbs > speedAngleLimitMax)
            {
                desiredSpeed = speedAngleLimitMaxSpeed;
                //if (competitor->getNum() == 456) printf("n1");
            }
            else
            {
                desiredSpeed = calcDesiredSpeed(angleToNextAbs, desiredSpeed);//brakeSpeedLimitMax - angleToNextAbs;
                //if (competitor->getNum() == 456) printf("n2");
            }
        }
        else
        {
            //if (competitor->getNum() == 456) printf("n0");
        }

        // calculate a desired speed to the given angle of next next point if we are near
        if (distToNextPoint<(REACHED_POINT_DIST_NEAR+(speed*0.5f)) && angleToNextNextAbs > speedAngleLimitMin)
        {
            float desiredSpeed2 = speedLimitHigh;
            if (speed > desiredSpeed2) desiredSpeed2 = speed;

            if (angleToNextNextAbs > speedAngleLimitMax)
            {
                desiredSpeed2 = speedAngleLimitMaxSpeed;
                //if (competitor->getNum() == 456) printf("nn1");
            }
            else
            {
                desiredSpeed2 = calcDesiredSpeed(angleToNextNextAbs, desiredSpeed2);//brakeSpeedLimitMax - angleToNextNextAbs;
                //if (competitor->getNum() == 456) printf("nn2");
            }

            if (desiredSpeed2 < desiredSpeed)
            {
                desiredSpeed = desiredSpeed2;
                //if (competitor->getNum() == 456) printf("nn3");
            }
        }
        else
        {
            //if (competitor->getNum() == 456) printf("nn0");
        }
        
        //if (competitor->getNum() == 499) printf("s: %d, ds: %d\n", (int)speed, (int)desiredSpeed);

        if (speed < desiredSpeed - 5.f)
        {
            // calculate torque base
            // below the low limit - full throttle
            // abovw the high limit - 0 torque
            // between them linear interpolate or something
            if (speed < speedLimitLow)
            {
                torque = 1.0f;
            }
            else if (speed > speedLimitHigh)
            {
                torque = 0.0f;
            }
            else
            {
                torque = 0.75f + ((speedLimitHigh - speed)/speedLimitDist) * 0.25f;
            }
        }
        else if (desiredSpeed - 5.0f < speed && speed < desiredSpeed + 5.0f)
        {
            torque = 0.0f;
        }
        else // desiredSpeed + 5.0f < speed
        {
            torque = 0.0f;
            float speedDiff = speed - desiredSpeed;

            brake = 1.0f;
            if (speedDiff < speedLimitDist)
            {
                brake = speedDiff / speedLimitDist;
            }
        }

        if (collisionCD > 0)
        {
            collisionCD--;
            brake = 1.0f;
            steer = 1.0f;
        } else if (collisionCD < 0)
        {
            collisionCD++;
            brake = 1.0f;
            steer = -1.0f;
        }

        //if (competitor->getNum() == 456)
        //{
        //    printf("\nt: %f, b: %f, s: %f, desiredSpeed: %f\n", torque, brake, steer, desiredSpeed);
        //}
        vehicle->setSteer(steer);
        if (brake > 0.9f)
        {
            vehicle->setHandbrake(brake);
            vehicle->setTorque(brake); // + value is brake
            //if (competitor->getNum() == 499) printf("s: %d, ds: %d, t: -, b: %f\n", (int)speed, (int)desiredSpeed, brake);
        }
        else if (brake > 0.0001f)
        {
            vehicle->setTorque(brake); // + value is brake
            vehicle->setHandbrake(0.0f);
            //if (competitor->getNum() == 499) printf("s: %d, ds: %d, t: -, b: %f\n", (int)speed, (int)desiredSpeed, brake);
        }
        else
        {
            vehicle->setTorque(-torque); // - value is accelerate
            vehicle->setHandbrake(0.0f);
            //if (competitor->getNum() == 499) printf("s: %d, ds: %d, t: %f, b: -\n", (int)speed, (int)desiredSpeed, torque);
        }
        passedDistance += distanceStep/10.f;
    }
    else // not visible or there is no free pool vehicle
    {
        if (distFromMe < NEAR_VALUE && camActive)
        {
            switchToVisible();
            if (visible)
            {
                return true;
            }
        }
        
        if (crashedForever || crashTime)
        {
            if (crashTime && lastCrashUpdate != currentTime)
            {
                lastCrashUpdate = currentTime;
                crashTime--;
            }
            if (crashedForever)
            {
                return camActive;
            }
            
            return true;
        }
        else
        {
            if ((currentTime % competitor->getNum()) == 0)
            {
                generateRandomFailure(camActive);
                if (crashedForever || crashTime)
                {
                    if (!finishTime && nextPoint != stage->getAIPointList().begin() && nextPoint != stage->getAIPointList().end())
                    {
                        irr::core::vector3df cp((*prevPoint)->getPos());
                        irr::core::vector3df ncp((*nextPoint)->getPos());
                        dir = (ncp - cp);
                        dir.rotateXZBy(90.0f);
                        dir.setLength(10.f);
                        currentPos = cp + dir;
                    }
                }
            }
        }
        
        irr::core::vector3df oldPos(currentPos);
        
        passedDistance += distanceStep;
        if (passedDistance > (*nextPoint)->getGlobalDistance()-0.0001f)
        {
            goToNextPoint(currentTime, camActive);
        }
        if (!finishTime && nextPoint != stage->getAIPointList().begin() && nextPoint != stage->getAIPointList().end())
        {
            irr::core::vector3df cp((*prevPoint)->getPos());
            irr::core::vector3df ncp((*nextPoint)->getPos());
            dir = (ncp - cp) * ((passedDistance-(*prevPoint)->getGlobalDistance())/((*nextPoint)->getLocalDistance()));
            currentPos = cp + dir;
        }
    }
    return true;
}

void Starter::switchToVisible()
{
    dprintf(MY_DEBUG_NOTE, "%d became visible\n", competitor->getNum());
    float rot = 0.f;
    bool addImpulse = false;
    if (nextPoint != stage->getAIPointList().begin() && nextPoint != stage->getAIPointList().end())
    {
        irr::core::vector2df cp((*nextPoint)->getPos().X, (*nextPoint)->getPos().Z);
        irr::core::vector2df pcp((*prevPoint)->getPos().X, (*prevPoint)->getPos().Z);
        rot = (float)(cp-pcp).getAngle();
        dir = ((*nextPoint)->getPos() - (*prevPoint)->getPos());
        dir.normalize();
        addImpulse = true;
        collisionCD = 0;
    }
    vehicle = new Vehicle(competitor->getVehicleTypeName(),
        irr::core::vector3df(currentPos.X, currentPos.Y+1.7f, currentPos.Z),
        irr::core::vector3df(0.f, rot, 0.f));
    vehicle->setNameText(nameText);
    vehicle->setVehicleCollisionCB(this);
    if (addImpulse)
    {
        vehicle->addStartImpulse(60.f, dir);
    }
    raceEngine->addUpdater(this);
    if (Settings::getInstance()->showNames)
    {
        nameText->setVisible(true);
    }
    //nameTextOffsetObject = new OffsetObject(nameText, true);
    //nameTextOffsetObject->addToManager();
    visible = true;
    lastAngleToNextAbs = 180.f;
    dprintf(MY_DEBUG_NOTE, "%d became visible end\n", competitor->getNum());
}

void Starter::switchToNotVisible()
{
    dprintf(MY_DEBUG_NOTE, "%d became not visible\n", competitor->getNum());
    visible = false;
    if (vehicle)
    {
        irr::core::vector3df cp(OffsetManager::getInstance()->getOffset()+vehicle->getMatrix().getTranslation());
        //vehicle->setNameText(0);
        //vehicle->setVehicleCollisionCB(0);
        delete vehicle;
        vehicle = 0;
        currentPos = cp;
        calculateTo(currentPos/*(*nextPoint)->getPos()*/);
        nameText->setVisible(false);
        if (nameTextOffsetObject)
        {
            nameTextOffsetObject->setNode(0);
            nameTextOffsetObject->removeFromManager();
            delete nameTextOffsetObject;
            nameTextOffsetObject = 0;
        }
    }
    raceEngine->removeUpdater(this);
}

void Starter::goToNextPoint(unsigned int currentTime, bool camActive)
{
    //printf("reset cnt: %u\n", competitor->getNum());
    forResetCnt = 0;
    forBigResetCnt = 0;
    if (nextPoint != stage->getAIPointList().end())
    {
        if (visible)
        {
            passedDistance = (*nextPoint)->getGlobalDistance();
            prevPoint = nextPoint;
            prevPointNum = nextPointNum;
            nextPoint++;
            nextPointNum++;
            lastAngleToNextAbs = 180.f;
        }
        else
        {
            while (nextPoint != stage->getAIPointList().end() &&
                   passedDistance > (*nextPoint)->getGlobalDistance())
            {
                prevPoint = nextPoint;
                prevPointNum = nextPointNum;
                nextPoint++;
                nextPointNum++;
            }
        }
    }
    if (nextPoint == stage->getAIPointList().end())
    {
        //if (competitor->getAi())
        {
            // no more point finish the stage
            finishTime = currentTime - startTime;
            globalTime += finishTime;
            globalPenaltyTime += penaltyTime;
            
            unsigned int position = raceEngine->insertIntoFinishedState(this);
            if (camActive)
            {
                irr::core::stringw str = L"";
                
                str += competitor->getNum();
                str += L" ";
                str += competitor->getName().c_str();
                str += L" finished the stage, time: ";
                WStringConverter::addTimeToStr(str, finishTime+penaltyTime);
                str += L",  position: ";
                str += position;
                MessageManager::getInstance()->addText(str.c_str(), 6);
            }
        }
    }
    else
    {
        if (nextPoint != stage->getAIPointList().begin())
        {
            irr::core::vector3df cp((*prevPoint)->getPos());
            irr::core::vector3df ncp((*nextPoint)->getPos());
            dir = (ncp - cp) * ((passedDistance-(*prevPoint)->getGlobalDistance())/((*nextPoint)->getLocalDistance()));
            currentPos = cp; // + dir;
        }
        //currentRand = (((float)(rand() % 100) - 50.f) / 1000.f) + ((float)competitor->strength / 50000.f);
        currentRand = ((float)(rand() % 100) - (60.f * ((float)competitor->getStrength()/100.f))) * ((110.f - (float)competitor->getStrength())/200.f);
        calculateTo(currentPos/*(*nextPoint)->getPos()*/);
    }
}

void Starter::calculateTo(const irr::core::vector3df& nextPointPos)
{
    distanceStep = 0;
    if (!stage->getAIPointList().empty() && stage->getStageTime() > 0)
    {
        distanceStep = (stage->getAIPointList().back()->getGlobalDistance() / (float)stage->getStageTime()) *
            (((float)competitor->getStrength()+currentRand+stageRand) / 100.f);
    }
}

void Starter::generateRandomFailure(bool camActive)
{
    const unsigned int limit = 101 - competitor->getStrength();
    const unsigned int value = rand() % 10000;

    dprintf(MY_DEBUG_NOTE, "Starter::generateRandomFailure(): num: %u, limit: %u, value: %u\n",
        competitor->getNum(), limit, value);

    if (value < limit)
    {
        const unsigned int time = rand() % 1830;
        dprintf(MY_DEBUG_NOTE, "Starter::generateRandomFailure(): time: %u\n", time);
        if (time > 1800)
        {
            crashedForever = true;
        }
        else
        {
            crashTime = time + 60; // wait at least for 60 secs
        }
    }
}

RaceEngine::RaceEngine(StageState* stageState,
                       int stageNum)
    : stage(stageState->stage),
      starters(), cupdaters(),
      lastMTick(0), lastCTick(0), currentTime(0),
      raceFinished(false)
{
    unsigned int i = 0;
    for (competitorResultList_t::const_iterator it = stageState->competitorResultListStage.begin();
         it != stageState->competitorResultListStage.end();
         it++, i++)
    {
        unsigned int startCD = START_SECS;
        if (stageNum == 0)
        {
            if (i < 19)
            {
                startCD = 3 * 60;
            }
            else
            if (i < 39)
            {
                startCD = 60;
            }
            else
            {
                startCD = 30;
            }
        }
        else
        {
            if (i < 9)
            {
                startCD = 2 * 60;
            }
            else
            if (i < 19)
            {
                startCD = 60;
            }
            else
            {
                startCD = 30;
            }
        }
        Starter* starter = new Starter(stage, this, (*it)->competitor, startCD, i, (*it)->globalTime, (*it)->globalPenaltyTime);
        starters.push_back(starter);
    }
}

RaceEngine::RaceEngine(Stage* stage)
    : stage(stage),
      starters(), cupdaters(),
      lastMTick(0), lastCTick(0), currentTime(0),
      raceFinished(false)
{
}

RaceEngine::~RaceEngine()
{
    for (starterList_t::const_iterator it = starters.begin();
         it != starters.end();
         it++)
    {
        delete (*it);
    }
    starters.clear();
    cupdaters.clear();
}

bool RaceEngine::update(unsigned int tick, const irr::core::vector3df& apos, UpdateWhen when)
{
    const unsigned int mtick = tick/1000;
    const unsigned int ctick = tick/100;
    int updates = 0;
    
    //printf("raceupdate: raceFinished: %u\n", raceFinished);

    if (raceFinished) return false;
    
    if (mtick != lastMTick || when != InTheMiddle)
    {
        //pdprintf(printf("race engine update\n");)
        currentTime++;
        
        for (starterList_t::const_iterator it = starters.begin();
             it != starters.end();
             )
        {
            //pdprintf(printf("race engine %d. sCD %d, nextPoint %d/%d, pd %f, finishTime %d\n",
            //    i, starters[i]->startingCD, starters[i]->nextPoint, m_bigTerrain->getAIPoints().size(),
            //    starters[i]->passedDistance, starters[i]->finishTime);)
/*
            if (m_bigTerrain->getAIPoints().size()!=starters[i]->nextPoint && when == AtTheEnd)
            {
                printf("race engine %d. sCD %d, nextPoint %d/%d, passedfinishTime %d\n",
                    i, starters[i]->startingCD, starters[i]->nextPoint, m_bigTerrain->getAIPoints().size(), starters[i]->finishTime);
            }
*/
//            if (when == AtTheEnd) device->sleep(100);
            //printf("%u: startcd: %u, startTime: %u, finsishTime: %u\n",
            //    (*it)->competitor->getNum(), (*it)->startingCD, (*it)->startTime, (*it)->finishTime);
            if ((*it)->startingCD > 0)
            {
                updates++;
                (*it)->startingCD--;
                if ((*it)->startingCD==0)
                {
                    (*it)->startTime = currentTime;
                    if (!(*it)->competitor->getAi())
                    {
                        (*it)->startTime++; // one sec need to the TheGame::doFewSteps(), which run for one sec virtually
                        if (Settings::getInstance()->AIPlayer)
                        {
                            addUpdater(*it);
                            (*it)->visible = true;
                            (*it)->goToNextPoint(currentTime, when == InTheMiddle);
                        }
                        return false;
                    }
                    (*it)->goToNextPoint(currentTime, when == InTheMiddle);
                }
                else
                    break;
            }
            else
            if ((*it)->finishTime==0 /*&& (*it)->competitor->getAi()*/)
            {
                if ((*it)->update(currentTime, apos, when == InTheMiddle))
                {
                    updates++;
                }
            }

            if ((*it)->finishTime)
            {
                starterList_t::const_iterator delIt = it;
                it++;
                delete (*delIt);
                starters.erase(delIt);
            }
            else
            {
                it++;
            }
        }
        
        //pdprintf(printf("race engine update end, updates: %d\n", updates);)
        lastMTick = mtick;
    }
    else
    if (ctick != lastCTick)
    {
        for (starterSet_t::const_iterator it = cupdaters.begin();
             it != cupdaters.end();
             )
        {
            starterSet_t::const_iterator nextIt = it;
            nextIt++;
            assert((*it)->visible);
            //if ((*it)->visible)
            //{
                (*it)->update(currentTime, apos, when == InTheMiddle);
            //}
            it = nextIt;
        }
        lastCTick = ctick;
    }
    
    if (updates==0 && when == AtTheEnd)
    {
        raceFinished = true;
    }
    return updates!=0;
}

void RaceEngine::addUpdater(Starter* starter)
{
    cupdaters.insert(starter);
}

void RaceEngine::removeUpdater(Starter* starter)
{
    cupdaters.erase(starter);
}

unsigned int RaceEngine::insertIntoFinishedState(Starter* starter)
{
    /*
    int i = 0;
    while (i < finishedState.size() && competitor->lastTime >= finishedState[i]->lastTime)
    {
        i++;
    }
    finishedState.insert(competitor, i);
    return (i+1);
    */
    //assert(0 && "nyi");
    CompetitorResult* competitorResult = new CompetitorResult(starter->competitor,
        starter->finishTime, starter->penaltyTime, starter->globalTime, starter->globalPenaltyTime);
    return GamePlay::getInstance()->competitorFinished(competitorResult);
}

bool RaceEngine::save(const std::string& filename)
{
    FILE* f;
    int ret;
    
    errno_t error = fopen_s(&f, filename.c_str(), "w");
    if (error)
    {
        printf("unable to open race engine file for write %s\n", filename.c_str());
        return false;
    }

    ret = fprintf(f, "current_time: %u\n", currentTime);
    if (ret < 1)
    {
        printf("error writing save file ret %d errno %d\n", ret, errno);
        fclose(f);
        return false;
    }
    
    ret = fprintf(f, "starters: %lu\n", starters.size());
    if (ret < 1)
    {
        printf("error writing save file ret %d errno %d\n", ret, errno);
        fclose(f);
        return false;
    }
    
    unsigned int i = 0;
    for (starterList_t::const_iterator it = starters.begin();
         it != starters.end();
         it++, i++)
    {
        ret = fprintf(f, "starter[%u]: %u %u %u %u %u %u %u %u %u %f %f %f %f %f %u %u\n",
                      i,
                      (*it)->competitor->getNum(),
                      (*it)->globalTime,
                      (*it)->globalPenaltyTime,
                      (*it)->startingCD,
                      (*it)->startTime,
                      (*it)->finishTime,
                      (*it)->penaltyTime,
                      (*it)->prevPointNum,
                      (*it)->nextPointNum,
                      (*it)->currentPos.X,
                      (*it)->currentPos.Y,
                      (*it)->currentPos.Z,
                      (*it)->passedDistance,
                      (*it)->distanceStep,
                      (*it)->crashedForever,
                      (*it)->crashTime
                      );
        if (ret < 17)
        {
            printf("error writing save file ret %d errno %d\n", ret, errno);
            fclose(f);
            return false;
        }
    }
    fclose(f);
    return true;
}

bool RaceEngine::load(const std::string& filename, Race* race)
{
    FILE* f;
    int ret;
    unsigned long startersSize = 0;
    unsigned int tmpi;
    unsigned int compnum;

    errno_t error = fopen_s(&f, filename.c_str(), "r");
    if (error)
    {
        printf("race engine file unable to open: %s\n", filename.c_str());
        return false;
    }
    
    ret = fscanf_s(f, "current_time: %u\n", &currentTime);
    if (ret < 1)
    {
        printf("error reading save file ret %d errno %d\n", ret, errno);
        fclose(f);
        return false;
    }
    
    ret = fscanf_s(f, "starters: %lu\n", &startersSize);
    if (ret < 1)
    {
        printf("error reading save file ret %d errno %d\n", ret, errno);
        fclose(f);
        return false;
    }
    
    for (starterList_t::const_iterator it = starters.begin();
         it != starters.end();
         it++)
    {
        delete (*it);
    }
    starters.clear();
    cupdaters.clear();
    
    for (unsigned int i = 0; i < startersSize; i++)
    {
        Starter* starter = 0;
        unsigned int j = 0;
        
        ret = fscanf_s(f, "starter[%u]: %u", &tmpi, &compnum);
        if (ret < 2)
        {
            printf("error reading save file ret %d errno %d\n", ret, errno);
            fclose(f);
            return false;
        }
        dprintf(MY_DEBUG_INFO, "read stater[%u], num: %u\n", tmpi, compnum);
        
        Competitor* competitor = race->getCompetitor(compnum);

        if (competitor == 0 && compnum == Player::getInstance()->getCompetitor()->getNum())
        {
            competitor = Player::getInstance()->getCompetitor();
        }

        if (competitor)
        {
            starter = new Starter(stage, this, competitor, 0, tmpi, 0, 0);
            dprintf(MY_DEBUG_NOTE, "found competitor, create starter %p\n", starter);
        }
        else
        {
            printf("error reading save file num %d not found in raceState\n", compnum);
            fclose(f);
            return false;
        }
        ret = fscanf_s(f, "%u %u %u %u %u %u %u %u %f %f %f %f %f %u %u\n",
                      &starter->globalTime,
                      &starter->globalPenaltyTime,
                      &starter->startingCD,
                      &starter->startTime,
                      &starter->finishTime,
                      &starter->penaltyTime,
                      &starter->prevPointNum,
                      &starter->nextPointNum,
                      &starter->currentPos.X,
                      &starter->currentPos.Y,
                      &starter->currentPos.Z,
                      &starter->passedDistance,
                      &starter->distanceStep,
                      &starter->crashedForever,
                      &starter->crashTime
                      );
        if (ret < 15)
        {
            printf("error reading save file ret %d errno %d\n", ret, errno);
            fclose(f);
            return false;
        }

        unsigned int npn = 0;
        unsigned int ppn = 0;
        AIPoint::AIPointList_t::const_iterator aiit = stage->getAIPointList().begin();
        for (;
             aiit != stage->getAIPointList().end() && ppn < starter->prevPointNum;
             aiit++, ppn++)
        {
        }
        starter->prevPoint = aiit;

        aiit = stage->getAIPointList().begin();
        for (;
             aiit != stage->getAIPointList().end() && npn < starter->nextPointNum;
             aiit++, npn++)
        {
        }
        starter->nextPoint = aiit;
        //printf("starter->finishTime: %u\n", starter->finishTime);
        //if (starter->finishTime!=0) insertIntoFinishedState(starter->competitor);
        if ((starter->finishTime)!=0)
        {
            assert(0);
            insertIntoFinishedState(starter);
        }
        else
        {
            starters.push_back(starter);
            if (!starter->competitor->getAi() && Settings::getInstance()->AIPlayer)
            {
                addUpdater(starter);
                starter->visible = true;
            }
        }
    }
    fclose(f);
    return true;
}

void RaceEngine::updateShowNames()
{
    for (starterList_t::const_iterator it = starters.begin();
         it != starters.end();
         it++)
    {
        if ((*it)->visible)
        {
            (*it)->nameText->setVisible(Settings::getInstance()->showNames);
        }
    }
}


#if 0
void RaceEngine::clearStates()
{
    raceState.clear();
    stageState.clear();
    stageStateStarters.clear();
}

void RaceEngine::refreshStates(RaceEngine* re)
{
    clearStates();

    for (int i = 0; i < re->starters.size(); i++)
    {
        int j = 0;
        //if (re->starters[i]->competitor->lastTime == 0) continue;
        while (j < raceState.size() &&
               (
                (re->starters[i]->competitor->lastTime==0 && raceState[j]->lastTime!=0)
                ||
                (re->starters[i]->competitor->globalTime >= raceState[j]->globalTime &&
                 re->starters[i]->competitor->lastTime==0 && raceState[j]->lastTime==0
                )
                ||
                (re->starters[i]->competitor->globalTime >= raceState[j]->globalTime &&
                 re->starters[i]->competitor->lastTime!=0 && raceState[j]->lastTime!=0
                )
               )
              )
        {
            j++;
        }
        raceState.insert(re->starters[i]->competitor, j);

        j = 0;
        // update the player pos
        if (re->getStarters()[i]->competitor == playerCompetitor &&
            bigTerrain && car)
        {
            if (playerCompetitor->lastTime == 0)
            {
                float dist = 100000.f;
                const irr::core::vector3df my_pos = car->getMatrix().getTranslation() + offsetManager->getOffset();
                for (int k = 0; k < bigTerrain->getAIPoints().size(); k++)
                {
                    float cd = bigTerrain->getAIPoints()[k]->getPosition().getDistanceFrom(my_pos);
                    if (cd < dist)
                    {
                        dist = cd;
                        re->getStarters()[i]->nextPoint = k;
                    }
                }
            }
            else
            {
                re->getStarters()[i]->nextPoint = bigTerrain->getAIPoints().size() - 1;
            }
        }
        while (j < stageStateStarters.size() &&
               (
                (re->getStarters()[i]->competitor->lastTime == 0 && stageStateStarters[j]->competitor->lastTime != 0)
                ||
                (re->getStarters()[i]->nextPoint <= stageStateStarters[j]->nextPoint &&
                 re->getStarters()[i]->competitor->lastTime == 0 &&
                 stageStateStarters[j]->competitor->lastTime == 0
                )
                ||
                (re->getStarters()[i]->competitor->lastTime >= stageStateStarters[j]->competitor->lastTime &&
                 re->getStarters()[i]->competitor->lastTime != 0 &&
                 stageStateStarters[j]->competitor->lastTime != 0
                )
               )
              )
        {
            j++;
        }
        stageStateStarters.insert(re->getStarters()[i], j);
        stageState.insert(re->getStarters()[i]->competitor, j);
    }
}


void RaceEngine::refreshBigTerrain(BigTerrain* p_bigTerrain)
{
    m_bigTerrain = p_bigTerrain;
    for (int i = 0; i < starters.size(); i++)
    {
        starters[i]->m_bigTerrain = m_bigTerrain;
        starters[i]->calculateTo(m_bigTerrain->getAIPoints()[starters[i]->nextPoint]->getPosition());
    }
}

#endif // 0
