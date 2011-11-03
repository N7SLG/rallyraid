
#include "GamePlay.h"
#include "stdafx.h"
#include "RaceManager.h"
#include "Race.h"
#include "Day.h"
#include "Stage.h"
#include "TheGame.h"
#include "TheEarth.h"
#include "ObjectWire.h"
#include "Player.h"
#include "VehicleType.h"
#include "VehicleTypeManager.h"
#include "Competitor.h"
#include "RaceEngine.h"
#include "stdafx.h"
#include "ConfigDirectory.h"
#include "Hud.h"
#include "error.h"
#include "Settings.h"
#include <assert.h>


GamePlay* GamePlay::gamePlay = 0;

void GamePlay::initialize()
{
    if (gamePlay == 0)
    {
        gamePlay = new GamePlay();
    }
}

void GamePlay::finalize()
{
    if (gamePlay)
    {
        delete gamePlay;
        gamePlay = 0;
    }
}


GamePlay::GamePlay()
    : currentRace(0),
      currentStage(0),
      raceState(),
      raceEngine(0),
      loadableGames()
{
    refreshLoadableGames();
}

GamePlay::~GamePlay()
{
    if (raceEngine)
    {
        delete raceEngine;
        raceEngine = 0;
    }
    clearStageStateList(raceState);
}

void GamePlay::startNewGame(Race* race, VehicleType* vehicleType)
{
    if (vehicleType)
    {
        Stage* stage = 0;
        if (race)
        {
            Day* day = 0;

            if (!race->getDayMap().empty())
            {
                day = race->getDayMap().begin()->second;
            }
            if (day && !day->getStageMap().empty())
            {
                stage = day->getStageMap().begin()->second;
            }
        }

        // release old resources
        if (raceEngine)
        {
            delete raceEngine;
            raceEngine = 0;
        }

        if (currentRace)
        {
            clearStageStateList(raceState);
            currentRace = 0;
        }
        currentRace = race;

        // reinitalize new resources
        StageState* stageState = new StageState;

        stageState->stage = stage;
        const Race::competitorMap_t& competitorMap = currentRace->getCompetitorMap();
        for (Race::competitorMap_t::const_iterator it = competitorMap.begin();
             it != competitorMap.end();
             it++)
        {
            CompetitorResult* competitorResult = new CompetitorResult(it->second, 0, 0, 0, 0);
            stageState->competitorResultListStage.push_back(competitorResult);
        }
        {
            CompetitorResult* competitorResult = new CompetitorResult(Player::getInstance()->getCompetitor(), 0, 0, 0, 0);
            stageState->competitorResultListStage.push_back(competitorResult);
        }

        raceState.push_back(stageState);

        if (Settings::getInstance()->editorMode == false)
        {
            raceEngine = new RaceEngine(stageState, raceState.size());
            clearCompetitorResultList(stageState->competitorResultListStage);
            while (raceEngine && raceEngine->update(0, irr::core::vector3df(), RaceEngine::AtStart));
        }
        else
        {
            clearCompetitorResultList(stageState->competitorResultListStage);
        }
        startStage(stage, vehicleType);
    }
}

bool GamePlay::goToNextStage()
{
    if (Player::getInstance()->getStarter() != 0 && Player::getInstance()->getStarter()->finishTime == 0)
    {
        return false;
    }

    VehicleType* vehicleType = Player::getInstance()->getVehicle()->getVehicleType(); //VehicleTypeManager::getInstance()->getVehicleType(Player::getInstance()->getCompetitor()->getVehicleTypeName());
    if (vehicleType)
    {
        Stage* stage = RaceManager::getInstance()->getNextStage();
        
        if (stage == 0)
        {
            return false;
        }

        // release old resources
        if (raceEngine)
        {
            while (raceEngine && raceEngine->update(0, irr::core::vector3df(), RaceEngine::AtTheEnd));
            delete raceEngine;
            raceEngine = 0;
        }
        
        assert(!raceState.empty());

        // reinitalize new resources
        StageState* stageState = new StageState;

        stageState->stage = stage;
        const competitorResultList_t& competitorResultList = raceState.back()->competitorResultListStage;
        for (competitorResultList_t::const_iterator it = competitorResultList.begin();
             it != competitorResultList.end();
             it++)
        {
            CompetitorResult* competitorResult = new CompetitorResult(
                (*it)->competitor, 0, 0, (*it)->globalTime, (*it)->globalPenaltyTime);
            stageState->competitorResultListStage.push_back(competitorResult);
        }

        raceState.push_back(stageState);

        if (Settings::getInstance()->editorMode == false)
        {
            raceEngine = new RaceEngine(stageState, raceState.size());
            clearCompetitorResultList(stageState->competitorResultListStage);
            while (raceEngine && raceEngine->update(0, irr::core::vector3df(), RaceEngine::AtStart));
        }
        else
        {
            clearCompetitorResultList(stageState->competitorResultListStage);
        }
        startStage(stage, vehicleType);
        return true;
    }
    return false;
}

bool GamePlay::loadGame(const std::string& saveName)
{
    assert(Settings::getInstance()->editorMode == false);

    dprintf(MY_DEBUG_NOTE, "GamePlay::loadGame(): load save game: %s\n", saveName.c_str());

    bool ret = true;
    Stage* stage = 0;

    // release old resources
    if (raceEngine)
    {
        delete raceEngine;
        raceEngine = 0;
    }

    if (currentRace)
    {
        clearStageStateList(raceState);
        currentRace = 0;
    }

    ret = readStageStateList(SAVE_STATE(saveName), raceState);
    if (ret)
    {
        assert(!raceState.empty());
        stage = raceState.back()->stage;
        currentRace = stage->getParent()->getParent();

        stage->readPreData();

        raceEngine = new RaceEngine(stage);
        ret = raceEngine->load(SAVE_ENGINE(saveName), currentRace);
        if (ret)
        {
            ret = Player::getInstance()->load(SAVE_PLAYER(saveName), stage);
            if (!ret)
            {
                dprintf(MY_DEBUG_ERROR, "GamePlay::loadGame(): unable to load player: %s\n", saveName.c_str());
            }
        }
        else
        {
            dprintf(MY_DEBUG_ERROR, "GamePlay::loadGame(): unable to load race engine: %s\n", saveName.c_str());
        }
    }
    else
    {
        dprintf(MY_DEBUG_ERROR, "GamePlay::loadGame(): unable to read stage state list: %s\n", saveName.c_str());
    }

    if (ret)
    {
        VehicleType* vehicleType = VehicleTypeManager::getInstance()->getVehicleType(Player::getInstance()->getCompetitor()->getVehicleTypeName());
        startStage(stage, vehicleType, Player::getInstance()->getSavedPos(), Player::getInstance()->getSavedRot(), true);
    }
    else
    {
        dprintf(MY_DEBUG_ERROR, "GamePlay::loadGame(): unable to load save game: %s\n", saveName.c_str());
    }

    return ret;
}

bool GamePlay::saveGame(const std::string& saveName)
{
    assert(Settings::getInstance()->editorMode == false);

    dprintf(MY_DEBUG_NOTE, "GamePlay::saveGame(): save game: %s\n", saveName.c_str());

    bool ret = ((raceEngine != 0) && (currentRace != 0) && (!raceState.empty()));

    if (ret)
    {
        ret = ConfigDirectory::mkdir(SAVE_DIR);
        if (ret)
        {
            ret = ConfigDirectory::mkdir(SAVE_DIR_DIR(saveName));
            if (ret)
            {
                ret = writeStageStateList(SAVE_STATE(saveName), raceState);
                if (ret)
                {
                    ret = raceEngine->save(SAVE_ENGINE(saveName));
                    if (ret)
                    {
                        ret = Player::getInstance()->save(SAVE_PLAYER(saveName));
                    }
                }
            }
        }
    }
    if (!ret)
    {
        dprintf(MY_DEBUG_ERROR, "GamePlay::saveGame(): unable to write save game: %s\n", saveName.c_str());
        //PrintMessage(1, "Unable to save game: [%s]", saveName.c_str());
    }
    return ret;
}

void GamePlay::startStage(Stage* stage, VehicleType* vehicleType, const irr::core::vector3df& initPos, const irr::core::vector3df& initRot, bool forceReload)
{
    irr::core::vector3df initialPos(initPos);
    irr::core::vector3df initialRot(initRot);
    irr::core::vector3df initialDir(1.f, 0.f, 0.f);
    
    dprintf(MY_DEBUG_INFO, "GamePlay::startGame(): stage: %p, vehicleType: %p\n", stage, vehicleType);
    assert(vehicleType);

    if (stage && !forceReload)
    {
        unsigned int size = stage->getAIPointList().size();
        
        if (size > 0)
        {
            AIPoint::AIPointList_t::const_iterator aiit = stage->getAIPointList().begin();
            initialPos = (*aiit)->getPos();
            
            if (size > 1)
            {
                aiit++;
                initialDir = (*aiit)->getPos() - initialPos;
                initialDir.normalize();
                
                irr::core::vector2df toDeg(initialDir.X, initialDir.Z);
                initialRot = irr::core::vector3df(0.0f, (float)toDeg.getAngle(), 0.0f);
            }
            
            initialPos.Y += 2.5f;
        }
    }
    else
    {
        initialDir = initialRot.rotationToDirection(initialDir);
    }

    ObjectWire::getInstance()->reset();
    Player::getInstance()->finalizeVehicle();
    OffsetManager::getInstance()->reset();
    TheGame::getInstance()->reset(initialPos, initialDir);

    if (stage)
    {
        RaceManager::getInstance()->activateStage(stage, forceReload);
    }
    currentStage = stage;

    TheEarth::getInstance()->createFirst(initialPos, initialDir);
    Player::getInstance()->initializeVehicle(vehicleType->getName(), initialPos+initialDir, initialRot, currentStage);

    Hud::getInstance()->updateRoadBook();

    TheGame::getInstance()->doFewSteps(60);
    TheGame::getInstance()->resetTick();
}

void GamePlay::update(unsigned int tick, const irr::core::vector3df& apos, bool force)
{
    //printf("GamePlay::update(): raceEngine: %p, playerstarter: %p, time: %u\n",
    //    raceEngine, Player::getInstance()->getStarter(), Player::getInstance()->getStarter() ? Player::getInstance()->getStarter()->startTime : 0);
    if (raceEngine)
    {
        if (Player::getInstance()->getStarter() == 0 ||
            Player::getInstance()->getStarter()->startTime==0 ||
            (Player::getInstance()->getStarter()->startTime!=0 && (Player::getInstance()->getFirstPressed() /*|| Settings::getInstance()->AIPlayer*/ || force)))
        {
            raceEngine->update(tick, apos, RaceEngine::InTheMiddle);
        }
    }
}

unsigned int GamePlay::competitorFinished(CompetitorResult* competitorResult)
{
    unsigned int insertPos = 1;

    assert(!raceState.empty());

    StageState* stageState = raceState.back();

    // update stage list
    if (stageState->competitorResultListStage.empty())
    {
        stageState->competitorResultListStage.push_back(competitorResult);
    }
    else
    {
        bool inserted = false;
        for (competitorResultList_t::const_iterator it = stageState->competitorResultListStage.begin();
             it != stageState->competitorResultListStage.end();
             it++, insertPos++)
        {
            if (((*it)->stageTime+(*it)->stagePenaltyTime) > (competitorResult->stageTime+competitorResult->stagePenaltyTime))
            {
                stageState->competitorResultListStage.insert(it, competitorResult);
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            stageState->competitorResultListStage.push_back(competitorResult);
        }
    }

    // update overall state
    if (stageState->competitorResultListOverall.empty())
    {
        stageState->competitorResultListOverall.push_back(competitorResult);
    }
    else
    {
        bool inserted = false;
        for (competitorResultList_t::const_iterator it = stageState->competitorResultListOverall.begin();
             it != stageState->competitorResultListOverall.end();
             it++)
        {
            if (((*it)->globalTime+(*it)->globalPenaltyTime) > (competitorResult->globalTime+competitorResult->globalPenaltyTime))
            {
                stageState->competitorResultListOverall.insert(it, competitorResult);
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            stageState->competitorResultListOverall.push_back(competitorResult);
        }
    }

    return insertPos;
}

void GamePlay::refreshLoadableGames()
{
    loadableGames.clear();

    ConfigDirectory::fileList_t fileList;
    
    dprintf(MY_DEBUG_NOTE, "Read [%s] directory:\n", SAVE_DIR.c_str());

    bool ret = ConfigDirectory::load(SAVE_DIR.c_str(), "racestate", fileList);
    
    if (!ret)
    {
        dprintf(MY_DEBUG_WARNING, "unable to read [%s] directory\n", SAVE_DIR.c_str());
        return;
    }
    
    for (ConfigDirectory::fileList_t::const_iterator it = fileList.begin();
         it != fileList.end();
         it++)
    {
        std::string saveName = it->c_str();
        char raceName[256];
        FILE* f;
        int rc;
        
        errno_t error = fopen_s(&f, SAVE_STATE(saveName).c_str(), "r");
        if (error)
        {
            printf("stage state file unable to open: %s\n", SAVE_STATE(saveName).c_str());
            continue;
        }

        rc = fscanf_s(f, "%s\n", raceName, 255);
        if (rc < 1)
        {
            printf("stage state unable to read race name: %s\n", SAVE_STATE(saveName).c_str());
            fclose(f);
            continue;
        }

        Race* race = RaceManager::getInstance()->getRace(raceName);
        if (race == 0)
        {
            printf("unable to find race: %s\n", raceName);
            fclose(f);
            continue;
        }
        loadableGames[saveName] = race->getLongName();
        fclose(f);
    }
}

/* static */ bool GamePlay::readStageStateList(const std::string& filename, stageStateList_t& stageStateList)
{
    FILE* f;
    int ret = 0;
    char raceName[256];
    char dayName[256];
    char stageName[256];
    Race* race = 0;
    Day* day = 0;
    Stage* stage = 0;
    unsigned long numOfStages;


    if (!stageStateList.empty())
    {
        dprintf(MY_DEBUG_WARNING, "stage state already exists: %s\n", filename.c_str());
        return true;
    }

    errno_t error = fopen_s(&f, filename.c_str(), "r");
    if (error)
    {
        printf("stage state file unable to open: %s\n", filename.c_str());
        return false;
    }
    
    ret = fscanf_s(f, "%s\n", raceName, 255);
    if (ret < 1)
    {
        printf("stage state unable to read race name: %s\n", filename.c_str());
        fclose(f);
        return false;
    }
    race = RaceManager::getInstance()->getRace(raceName);
    if (race == 0)
    {
        printf("unable to find race: %s\n", raceName);
        fclose(f);
        return false;
    }
    race->readPreData();

    ret = fscanf_s(f, "%lu\n", &numOfStages);
    if (ret < 1)
    {
        printf("stage state unable to read number of stages: %s\n", filename.c_str());
        fclose(f);
        return false;
    }

    for (unsigned int i = 0; i < numOfStages; i++)
    {
        ret = fscanf_s(f, "%s\n%s\n", stageName, 255, dayName, 255);
        if (ret < 2)
        {
            printf("stage state unable to read stage and day name: %s\n", filename.c_str());
            fclose(f);
            return false;
        }
        StageState* stageState = new StageState;
        day = race->getDay(dayName);
        if (day == 0)
        {
            printf("unable to find day: %s (%s)\n", dayName, raceName);
            fclose(f);
            return false;
        }
        stage = day->getStage(stageName);
        if (stage == 0)
        {
            printf("unable to find stage: %s (%s, %s)\n", stageName, dayName, raceName);
            fclose(f);
            return false;
        }
        stageState->stage = stage;

        bool retb = readCompetitorResultList(f, race, stageState->competitorResultListStage);
        if (!retb)
        {
            printf("unable to read competitor list stage\n");
            fclose(f);
            return false;
        }
        retb = readCompetitorResultList(f, race, stageState->competitorResultListOverall);
        if (!retb)
        {
            printf("unable to read competitor list overall\n");
            fclose(f);
            return false;
        }

        stageStateList.push_back(stageState);
    }
    fclose(f);
    return true;
}

/* static */ bool GamePlay::writeStageStateList(const std::string& filename, const stageStateList_t& stageStateList)
{
    FILE* f;
    int ret = 0;
    Race* race = 0;
    Day* day = 0;
    Stage* stage = 0;

    if (stageStateList.empty())
    {
        return true;
    }

    stage = stageStateList.front()->stage;
    day = stage->getParent();
    race = day->getParent();

    errno_t error = fopen_s(&f, filename.c_str(), "w");
    if (error)
    {
        printf("unable to open stage state file for write %s\n", filename.c_str());
        return false;
    }

    ret = fprintf(f, "%s\n", race->getName().c_str());

    ret = fprintf(f, "%lu\n", stageStateList.size());
    for (stageStateList_t::const_iterator it = stageStateList.begin(); 
         it != stageStateList.end();
         it++)
    {
        stage = (*it)->stage;
        day = stage->getParent();
        assert(race == day->getParent());
        ret = fprintf(f, "%s\n", stage->getName().c_str());
        ret = fprintf(f, "%s\n", day->getName().c_str());
        writeCompetitorResultList(f, (*it)->competitorResultListStage);
        writeCompetitorResultList(f, (*it)->competitorResultListOverall);
    }

    fclose(f);
    return true;
}

/* static */ bool GamePlay::readCompetitorResultList(FILE* f, Race* race, competitorResultList_t& competitorResultList)
{
    int ret = 0;
    unsigned long numOfComps = 0;
    unsigned int num = 0;
    unsigned int stageTime = 0;
    unsigned int globalTime = 0;
    unsigned int stagePenaltyTime = 0;
    unsigned int globalPenaltyTime = 0;

    ret = fscanf_s(f, "%lu\n", &numOfComps);
    if (ret < 1)
    {
        printf("stage state unable to read number of competitor list\n");
        fclose(f);
        return false;
    }

    for (unsigned int i = 0; i < numOfComps; i++)
    {
        ret = fscanf_s(f, "%u %u %u %u %u\n", &num, &stageTime, &globalTime, &stagePenaltyTime, &globalPenaltyTime);
        if (ret < 5)
        {
            printf("stage state unable to read competitor list\n");
            fclose(f);
            return false;
        }
        Competitor* competitor = 0;
        Race::competitorMap_t::const_iterator cit = race->getCompetitorMap().find(num);
        if (cit != race->getCompetitorMap().end())
        {
            competitor = cit->second;
        }
        else if (Player::getInstance()->getCompetitor()->getNum() == num)
        {
            competitor = Player::getInstance()->getCompetitor();
        }
        else
        {
            printf("stage state unable to find competitor: %u (%s)\n", num, race->getName().c_str());
            fclose(f);
            return false;
        }
        CompetitorResult* competitorResult = new CompetitorResult(competitor, stageTime, stagePenaltyTime, globalTime, globalPenaltyTime);

        competitorResultList.push_back(competitorResult);
    }
    return true;
}

/* static */ bool GamePlay::writeCompetitorResultList(FILE* f, const competitorResultList_t& competitorResultList)
{
    int ret = 0;
    ret = fprintf(f, "%lu\n", competitorResultList.size());

    for (competitorResultList_t::const_iterator it = competitorResultList.begin(); 
         it != competitorResultList.end();
         it++)
    {
        ret = fprintf(f, "%u %u %u %u %u\n", (*it)->competitor->getNum(), (*it)->stageTime, (*it)->globalTime,
            (*it)->stagePenaltyTime, (*it)->globalPenaltyTime);
    }
    return true;
}

/* static */ void GamePlay::clearStageStateList(stageStateList_t& stageStateList)
{
    for (stageStateList_t::const_iterator it = stageStateList.begin(); 
         it != stageStateList.end();
         it++)
    {
        clearCompetitorResultList((*it)->competitorResultListStage);
        clearCompetitorResultList((*it)->competitorResultListOverall, false);
        delete (*it);
    }
    stageStateList.clear();
}

/* static */ void GamePlay::clearCompetitorResultList(competitorResultList_t& competitorResultList, bool doDelete)
{
    if (doDelete)
    {
        for (competitorResultList_t::const_iterator it = competitorResultList.begin(); 
             it != competitorResultList.end();
             it++)
        {
            delete (*it);
        }
    }
    competitorResultList.clear();
}
