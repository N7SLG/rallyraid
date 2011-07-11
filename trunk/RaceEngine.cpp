
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
#define REACHED_POINT_DIST 15.f
#define REACHED_POINT_DIST_NEAR 20.f
#define ANGLE_LIMIT 30.f
#define AI_STEP (1.15f) // orig: (1.05f)


// normalize angle between 0 and 360
static float normalizeAngle(float &angle)
{
    while (angle > 360.f) angle -= 360.f;
    while (angle < 0.f) angle += 360.f;
    return angle;
}

static float normalizeAngle180(float &angle)
{
    while (angle > 180.f) angle -= 360.f;
    while (angle < -180.f) angle += 360.f;
    return angle;
}

Starter::Starter(Stage* stage,
                 RaceEngine* raceEngine,
                 Competitor* competitor,
                 unsigned int startingCD,
                 unsigned int place,
                 unsigned int globalTime,
                 unsigned int globalPenalityTime
)
    : stage(stage), raceEngine(raceEngine),
      competitor(competitor), startingCD(startingCD),
      startTime(0), finishTime(0), penalityTime(0),
      prevPoint(stage->getAIPointList().begin()),
      nextPoint(stage->getAIPointList().begin()),
      nextPointNum(0), currentPos(),
      crashed(false), visible(false), vehicle(0), forResetCnt(0),
      forBigResetCnt(0), forNonResetCnt(500),
      currentRand(0.f), nameText(0), nameTextOffsetObject(0),
      passedDistance(0.f), distanceStep(0.f), stageRand(0.f),
      globalTime(globalTime), globalPenalityTime(globalPenalityTime)
{
    const static float placeHardening[3] = {20.f, 8.f, 4.f};
    irr::core::stringw namePlusCar = L"";
    namePlusCar += (competitor->getName() + " (" + competitor->getVehicleTypeName() + ")").c_str();
    nameText = TheGame::getInstance()->getSmgr()->addTextSceneNode(
        /*env->getBuiltInFont()*/ FontManager::getInstance()->getFont(FontManager::FONT_SPECIAL14),
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
    }
}

Starter::~Starter()
{
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
    if (!competitor->getAi())
    {
        assert(Player::getInstance()->getStarter()==this);
        Player::getInstance()->setStarter(0);
    }
}

bool Starter::update(unsigned int currentTime, const irr::core::vector3df& apos, bool camActive)
{
    if (finishTime) return false;
    float distFromMe = currentPos.getDistanceFrom(apos);

    if (!competitor->getAi())
    {
        Player::getInstance()->setStageTime((currentTime - startTime) + penalityTime);
        if (!stage->getAIPointList().empty())
        {
            irr::core::vector3df cp(OffsetManager::getInstance()->getOffset()+Player::getInstance()->getVehicleMatrix().getTranslation());
            float distToFinish = cp.getDistanceFrom(stage->getAIPointList().back()->getPos());

            if (distToFinish < 5.f)
            {
                finishTime = currentTime - startTime;
                globalTime += finishTime;
                globalPenalityTime += penalityTime;
        
                unsigned int position = raceEngine->insertIntoFinishedState(this);
                irr::core::stringw str = L"";
            
                str += competitor->getNum();
                str += L" ";
                str += competitor->getName().c_str();
                str += L" finished the stage, time: ";
                WStringConverter::addTimeToStr(str, finishTime+penalityTime);
                str += L",  position: ";
                str += position;
                MessageManager::getInstance()->addText(str.c_str(), 2);
            }
        }
        return true;
    }
    
    if (visible && vehicle)
    {
        if (distFromMe > FAR_VALUE || !camActive)
        {
            switchToNotVisible();
            return true;
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
            goToNextPoint(currentTime, camActive);
            if (finishTime)
            {
                switchToNotVisible();
                return true;
            }
            nextPointPos = (*nextPoint)->getPos();
            nextPointPos2d = irr::core::vector2df(nextPointPos.X, nextPointPos.Z);
            distToNextPoint = currentPos.getDistanceFrom(nextPointPos);
        }
        
        
        float torque = 1.0f;
        float brake = 0.0f;
        float steer = 0.0f;
        float vehicleAngle = vehicle->getAngle(); normalizeAngle(vehicleAngle);
        const float speed = vehicle->getSpeed();
        //const float speedLimitLow = (((m_bigTerrain->getSpeed()-25.f-(float)(difficulty*5)) * ((float)competitor->strength+currentRand+stageRand)) / 100.f);
        const float speedLimitDist = 40.f;
        const float speedLimitLow = (((vehicle->getVehicleType()->getMaxSpeed()-speedLimitDist-(float)(Settings::getInstance()->difficulty*5)) * ((float)competitor->getStrength()+currentRand+stageRand)) / 100.f);
        const float speedLimitHigh = speedLimitLow + speedLimitDist;
        const float angleLimit = ANGLE_LIMIT;
        const float angleLimitMax = 180.f;
        const float brakeSpeedLimit = 30.f;
        const float brakeSpeedLimitMax = 5.f;
        
        if (fabsf(speed) < 0.5f)
        {
            forResetCnt++;
            if (forResetCnt >= 20)
            {
                dprintf(MY_DEBUG_INFO, "-------------\nreset AI car %d\nnrc: %u\nbrc: %u\n------------\n", competitor->getNum(), forNonResetCnt, forBigResetCnt);
                if (forNonResetCnt >= 500)
                {
                    penalityTime += RESET_PENALITY/2;
                    cp += irr::core::vector3df(3.f, 3.f, 3.f); // we don't know why stop: tree or felt over
                    forBigResetCnt++;
                    if (forBigResetCnt >= 2)
                    {
                        forNonResetCnt = 0;
                        forBigResetCnt = 0;
                    }
                }
                else
                {
                    penalityTime += RESET_PENALITY;
                    cp = nextPointPos;
                    cp.Y += 3.0f;
                }
                vehicle->reset(cp-OffsetManager::getInstance()->getOffset());
                forResetCnt=0;
                currentPos = cp;
                currentPos2d = irr::core::vector2df(currentPos.X, currentPos.Z);
            }
        }
        else
        {
            forNonResetCnt++;
            if (forNonResetCnt >= 100000)
            {
                forNonResetCnt = 500;
            }
        }
        /*
        if (vehicle->getDemagePer() > 20.f)
        {
            float demage = vehicle->getDemagePer();
            int penality = (int)(3.f*demage);
            for (int i = 0; i < 4; i++) 
            {
                if (!vehicle->isTyreConnected(i))
                {
                    penality += 15;
                }
            }
            dprintf(MY_DEBUG_INFO, "------------\nrepair AI car %d, penality: %d\n------------\n", competitor->num, penality);
            competitor->lastPenalityTime += penality;
            vehicle->repair();
        }
        */
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
        
        //pdprintf(printf("nextPoint: %d, va: %f, nToA %f\n", nextPoint, vehicleAngle, angleToNext);)
        //pdprintf(printf("speed: %f, dist: %f, nA %f\n", speed, distToNextPoint, nextPointAngle);)
        
        // calculate torque base
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
        
        // calculate torque more, and brake
        if (angleToNext > angleLimit)
        {
            // big difference
            if (speed > brakeSpeedLimit)
            {
                brake = 1.0f;
            }
            steer = 1.0f;
        }
        else
        if (angleToNext < -angleLimit)
        {
            // big difference
            if (speed > brakeSpeedLimit)
            {
                brake = 1.0f;
            }
            steer = -1.0f;
        }
        else
        {
            // small difference
            steer = angleToNext / (angleLimit*2.0f);
        }
        
        // if we are near to the nextPoint calculate some to the next-next point
        if (distToNextPoint<(REACHED_POINT_DIST_NEAR+(speed*0.5f)) && fabsf(angleToNextNext) > (angleLimit*2.f)/3.f)
        {
            float brakeMulti = 0.0f;
            if (speed > brakeSpeedLimit*4)
            {
                brake = 1.0f;
            }
            else
            if (speed > brakeSpeedLimit)
            {
                brake = (speed - brakeSpeedLimit) / (brakeSpeedLimit*3);
            }
            
            if (fabsf(angleToNextNext) > (angleLimit*2.f))
            {
                brakeMulti = 1.0f;
            }
            else
            if (fabsf(angleToNextNext) > (angleLimit*2.f)/3.f)
            {
                brakeMulti = (fabsf(angleToNextNext) - (angleLimit*2.f)/3.f) / (angleLimit+(angleLimit/3.f));
            }
            brake = brake * brakeMulti;
        }
        vehicle->setHandbrake(brake);
        vehicle->setSteer(steer);
        vehicle->setTorque(torque);
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
    dprintf(MY_DEBUG_INFO, "%d became visible\n", competitor->getNum());
    float rot = 0.f;
    if (nextPoint != stage->getAIPointList().begin() && nextPoint != stage->getAIPointList().end())
    {
        irr::core::vector2df cp((*nextPoint)->getPos().X, (*nextPoint)->getPos().Z);
        irr::core::vector2df pcp((*prevPoint)->getPos().X, (*prevPoint)->getPos().Z);
        rot = (float)(cp-pcp).getAngle();
    }
    vehicle = new Vehicle(competitor->getVehicleTypeName(),
        irr::core::vector3df(currentPos.X, currentPos.Y+1.7f, currentPos.Z),
        irr::core::vector3df(0.f, rot, 0.f));
    //vehicle->setNameText(nameText);
    raceEngine->addUpdater(this);
    if (Settings::getInstance()->showNames)
    {
        nameText->setVisible(true);
    }
    nameTextOffsetObject = new OffsetObject(nameText, true);
    nameTextOffsetObject->addToManager();
    visible = true;
    dprintf(MY_DEBUG_INFO, "%d became visible end\n", competitor->getNum());
}

void Starter::switchToNotVisible()
{
    dprintf(MY_DEBUG_INFO, "%d became not visible\n", competitor->getNum());
    visible = false;
    if (vehicle)
    {
        irr::core::vector3df cp(OffsetManager::getInstance()->getOffset()+vehicle->getMatrix().getTranslation());
        //vehicle->setNameText(0);
        delete vehicle;
        vehicle = 0;
        currentPos = cp;
        calculateTo((*nextPoint)->getPos());
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
    if (nextPoint != stage->getAIPointList().end())
    {
        if (visible)
        {
            passedDistance = (*nextPoint)->getGlobalDistance();
            prevPoint = nextPoint;
            nextPoint++;
            nextPointNum++;
        }
        else
        {
            while (nextPoint != stage->getAIPointList().end() &&
                   passedDistance > (*nextPoint)->getGlobalDistance())
            {
                prevPoint = nextPoint;
                nextPoint++;
                nextPointNum++;
            }
        }
    }
    if (nextPoint == stage->getAIPointList().end())
    {
        // no more point finish the stage
        finishTime = currentTime - startTime;
        globalTime += finishTime;
        globalPenalityTime += penalityTime;
        
        unsigned int position = raceEngine->insertIntoFinishedState(this);
        if (camActive)
        {
            irr::core::stringw str = L"";
            
            str += competitor->getNum();
            str += L" ";
            str += competitor->getName().c_str();
            str += L" finished the stage, time: ";
            WStringConverter::addTimeToStr(str, finishTime+penalityTime);
            str += L",  position: ";
            str += position;
            MessageManager::getInstance()->addText(str.c_str(), 2);
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
        calculateTo((*nextPoint)->getPos());
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

RaceEngine::RaceEngine(StageState* stageState,
                       int stageNum)
    : stage(stageState->stage),
      starters(), cupdaters(),
      lastMTick(0), lastCTick(0), currentTime(0),
      raceFinished(false)
{
    unsigned int i = 0;
    for (competitorResultList_t::const_iterator it = stageState->competitorResultListStage.begin();
         it != stageState->competitorResultListStage.begin();
         it++, i++)
    {
        unsigned int startCD = START_SECS;
#ifndef USE_EDITOR
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
#endif
        Starter* starter = new Starter(stage, this, (*it)->competitor, startCD, i, (*it)->globalTime, (*it)->globalPenalityTime);
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
            if ((*it)->startingCD > 0)
            {
                updates++;
                (*it)->startingCD--;
                if ((*it)->startingCD==0)
                {
                    (*it)->startTime = currentTime;
                    if ((*it)->competitor->getAi())
                    {
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
             it++)
        {
            if ((*it)->visible)
            {
                (*it)->update(currentTime, apos, when == InTheMiddle);
            }
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
        starter->finishTime, starter->penalityTime, starter->globalTime, starter->globalPenalityTime);
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
        ret = fprintf(f, "starter[%u]: %u %u %u %u %u %u %u %u %u %f %f %f %f\n",
                      i,
                      (*it)->competitor->getNum(),
                      (*it)->globalTime,
                      (*it)->globalPenalityTime,
                      (*it)->startingCD,
                      (*it)->startTime,
                      (*it)->finishTime,
                      (*it)->penalityTime,
                      (*it)->nextPointNum,
                      (*it)->currentPos.X,
                      (*it)->currentPos.Y,
                      (*it)->currentPos.Z,
                      (*it)->passedDistance
                      );
        if (ret < 13)
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
        ret = fscanf_s(f, "%u %u %u %u %u %u %u %f %f %f %f\n",
                      &starter->globalTime,
                      &starter->globalPenalityTime,
                      &starter->startingCD,
                      &starter->startTime,
                      &starter->finishTime,
                      &starter->penalityTime,
                      &starter->nextPointNum,
                      &starter->currentPos.X,
                      &starter->currentPos.Y,
                      &starter->currentPos.Z,
                      &starter->passedDistance
                      );
        if (ret < 11)
        {
            printf("error reading save file ret %d errno %d\n", ret, errno);
            fclose(f);
            return false;
        }

        unsigned int npn = 0;
        AIPoint::AIPointList_t::const_iterator aiit = stage->getAIPointList().begin();
        for (;
             aiit != stage->getAIPointList().end() && npn < starter->nextPointNum;
             aiit++, npn++)
        {
        }
        starter->nextPoint = aiit;
        starters.push_back(starter);
        //printf("starter->finishTime: %u\n", starter->finishTime);
        //if (starter->finishTime!=0) insertIntoFinishedState(starter->competitor);
        if ((starter->finishTime)!=0) insertIntoFinishedState(starter);
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
