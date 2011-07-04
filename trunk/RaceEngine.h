
#ifndef RACEENGINE_H
#define RACEENGINE_H

#include <list>
#include <set>
#include <irrlicht.h>
#include "AIPoint.h"

class RaceEngine;
class OffsetObject;
class Vehicle;
class Stage;
class Race;
class Competitor;
class CompetitorResult;
class StageState;

class Starter
{
public:
    Starter(Stage* stage,
            RaceEngine* raceEngine,
            Competitor* competitor,
            unsigned int startingCD,
            unsigned int place,
            unsigned int globalTime,
            unsigned int globalPenalityTime);
    ~Starter();
    
    bool update(unsigned int currentTime, const irr::core::vector3df& apos, bool camActive);
    void switchToVisible();
    void switchToNotVisible();
    void goToNextPoint(unsigned int currentTime, bool camActive);
    void calculateTo(const irr::core::vector3df& nextPointPos);

    Stage* stage;
    Competitor* competitor;
    unsigned int startingCD;
    unsigned int startTime;
    unsigned int finishTime;
    unsigned int penalityTime;
    //int nextPoint;
    AIPoint::AIPointList_t::const_iterator prevPoint;
    AIPoint::AIPointList_t::const_iterator nextPoint;
    unsigned int nextPointNum;
    irr::core::vector3df currentPos;
    bool crashed;
    bool visible;
    Vehicle* vehicle;
    RaceEngine* raceEngine;
    unsigned int forResetCnt;
    unsigned int forBigResetCnt;
    unsigned int forNonResetCnt;
    float currentRand;
    irr::scene::ITextSceneNode* nameText;
    OffsetObject* nameTextOffsetObject;
    irr::core::vector3df dir;
    float passedDistance;
    float distanceStep;
    float stageRand;
    unsigned int globalTime;
    unsigned int globalPenalityTime;
};

class RaceEngine
{
public:
    enum UpdateWhen
    {
        AtStart = 0,
        InTheMiddle,
        AtTheEnd
    };
    
    RaceEngine(StageState* stageState, int stageNum);
    RaceEngine(Stage* stage);
    ~RaceEngine();
    bool update(unsigned int tick, const irr::core::vector3df& apos, UpdateWhen when = InTheMiddle);
    void updateShowNames();
    bool save(const std::string& filename);
    bool load(const std::string& filename, Race* race);
    bool isRaceFinished() {return raceFinished;}

    // functions called by the Starter
    void addUpdater(Starter* starter);
    void removeUpdater(Starter* starter);
    unsigned int insertIntoFinishedState(Starter* starter);

    //core::array<Competitor*> &getFinishedState() {return finishedState;}
    //core::array<Starter*> &getStarters() {return starters;}
    //int insertIntoFinishedState(Competitor* competitor);
    //void refreshBigTerrain(BigTerrain* p_bigTerrain);
    //static void clearStates();
    //static void refreshStates(RaceEngine* re);
    //static core::array<Competitor*> &getRaceState() {return raceState;}
    //static core::array<Competitor*> &getStageState() {return stageState;}
    //static core::array<Starter*> &getStageStateStarters() {return stageStateStarters;}

private:
    typedef std::list<Starter*> starterList_t;
    typedef std::set<Starter*> starterSet_t;

    Stage* stage;
    starterList_t starters;
    starterSet_t cupdaters;
    unsigned int lastMTick;
    unsigned int lastCTick;
    unsigned int currentTime;
    bool raceFinished;
};

#endif // RACEENGINE_H
