
#include "MenuPageInGame.h"
#include "MenuPageStage.h"
#include "MenuPageLoadSave.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"

#include "TheEarth.h"
#include "ObjectPool.h"
#include "ObjectPoolManager.h"
#include "RaceManager.h"
#include "Race.h"
#include "Settings.h"
#include "RoadManager.h"
#include "Road.h"
#include "Day.h"
#include "Stage.h"
#include "RoadType.h"
#include "ObjectWireGlobalObject.h"
#include "Competitor.h"
#include "VehicleTypeManager.h"
#include "VehicleType.h"
#include "GamePlay.h"
#include "Hud.h"
#include "VehicleManager.h"
#include "FontManager.h"
#include "Player.h"
#include "Vehicle.h"
#include "RaceEngine.h"
#include <assert.h>


MenuPageInGame* MenuPageInGame::menuPageInGame = 0;

MenuPageInGame::MenuPageInGame()
    : window(0),
      staticTextRaceName(0),
      tableStages(0),
      tableCompetitors(0),
      tableCompetitorsG(0),
      tableCompetitorsInStage(0),
      buttonLoad(0),
      willOpenOtherWindow(false)
{
    menuPageInGame = this;
    window = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(0, 0, TheGame::getInstance()->getScreenSize().Width, TheGame::getInstance()->getScreenSize().Height),
        0,
        MI_WINDOW);
    window->setScaleImage(true);
    window->setImage(TheGame::getInstance()->getDriver()->getTexture("data/bg/3.jpg"));

    int line = 60;
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONBACK,
        L"Back To Race");

    line += 30;
    buttonNextStage = TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONNEXTSTAGE,
        L"Go To Next Stage");

    line += 30;
    buttonLoad = TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONLOAD,
        L"Load Game");

    line += 30;
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONSAVE,
        L"Save Game");

    line += 30;
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONOPTIONS,
        L"Options");

    line += 30;
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(10,line,140,line+20),
        window,
        MI_BUTTONEXIT,
        L"Exit");

    staticTextRaceName = TheGame::getInstance()->getEnv()->addStaticText(L"",
        irr::core::recti(window->getRelativePosition().getSize().Width/2 - 400,54,window->getRelativePosition().getSize().Width/2 + 400,90),
        //irr::core::recti(120,54,1200,88),
        false, false, window, 0, false);
    staticTextRaceName->setOverrideFont(FontManager::getInstance()->getFont(FontManager::FONT_VERDANA_22PX_BORDER/*SPECIAL18*/));
    staticTextRaceName->setOverrideColor(irr::video::SColor(255, 255, 255, 255));
    staticTextRaceName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);

    // ----------------------------
    // Stages
    // ----------------------------
    tableStages = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/8, (window->getRelativePosition().getSize().Height)/3), irr::core::dimension2di(window->getRelativePosition().getSize().Width/8-2,(window->getRelativePosition().getSize().Height*2)/3-2)),
        window,
        MI_TABLESTAGES,
        true);

    tableStages->addColumn(L"Stage");
    tableStages->setColumnWidth(0, tableStages->getRelativePosition().getSize().Width-16);

    // ----------------------------
    // Competitors
    // ----------------------------
    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(window->getRelativePosition().getSize().Width/4+2, (window->getRelativePosition().getSize().Height)/3-34), irr::core::dimension2di((window->getRelativePosition().getSize().Width*5)/8-2,(window->getRelativePosition().getSize().Height*2)/3-2+34)),
        window,
        true,
        true,
        0);

    irr::gui::IGUITab* tabStage = tc->addTab(L"Stage (Finished)", 0);

    tableCompetitors = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabStage->getRelativePosition().getSize()),
        tabStage,
        MI_TABLECOMPETITORS,
        true);

    tableCompetitors->addColumn(L"Pos.");
    tableCompetitors->setColumnWidth(0, 40);
    tableCompetitors->setColumnOrdering(0, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"No");
    tableCompetitors->setColumnWidth(1, 40);
    tableCompetitors->setColumnOrdering(1, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"Name");
    tableCompetitors->setColumnWidth(2, 240);
    tableCompetitors->setColumnOrdering(2, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"Vehicle");
    tableCompetitors->setColumnWidth(3, 190);
    tableCompetitors->setColumnOrdering(3, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"Time");
    tableCompetitors->setColumnWidth(4, 80);
    tableCompetitors->setColumnOrdering(4, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"Variation");
    tableCompetitors->setColumnWidth(5, 85);
    tableCompetitors->setColumnOrdering(5, irr::gui::EGCO_NONE);
    tableCompetitors->addColumn(L"Penalty");
    tableCompetitors->setColumnWidth(6, 85);
    tableCompetitors->setColumnOrdering(6, irr::gui::EGCO_NONE);
    //tableCompetitors->getRelativePosition().getSize().Width-(16+tableCompetitors->getColumnWidth(0)+tableCompetitors->getColumnWidth(1)+tableCompetitors->getColumnWidth(2)+tableCompetitors->getColumnWidth(3))
    
    irr::gui::IGUITab* tabGlobal = tc->addTab(L"Overall", 0);

    tableCompetitorsG = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabGlobal->getRelativePosition().getSize()),
        tabGlobal,
        MI_TABLECOMPETITORSG,
        true);

    tableCompetitorsG->addColumn(L"Pos.");
    tableCompetitorsG->setColumnWidth(0, 40);
    tableCompetitorsG->setColumnOrdering(0, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"No");
    tableCompetitorsG->setColumnWidth(1, 40);
    tableCompetitorsG->setColumnOrdering(1, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"Name");
    tableCompetitorsG->setColumnWidth(2, 240);
    tableCompetitorsG->setColumnOrdering(2, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"Vehicle");
    tableCompetitorsG->setColumnWidth(3, 190);
    tableCompetitorsG->setColumnOrdering(3, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"Time");
    tableCompetitorsG->setColumnWidth(4, 80);
    tableCompetitorsG->setColumnOrdering(4, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"Variation");
    tableCompetitorsG->setColumnWidth(5, 85);
    tableCompetitorsG->setColumnOrdering(5, irr::gui::EGCO_NONE);
    tableCompetitorsG->addColumn(L"Penalty");
    tableCompetitorsG->setColumnWidth(6, 85);
    tableCompetitorsG->setColumnOrdering(6, irr::gui::EGCO_NONE);

    irr::gui::IGUITab* tabInStage = tc->addTab(L"Stage (Ongoing)", 0);

    tableCompetitorsInStage = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabGlobal->getRelativePosition().getSize()),
        tabInStage,
        MI_TABLECOMPETITORSINSTAGE,
        true);

    tableCompetitorsInStage->addColumn(L"Start Pos.");
    tableCompetitorsInStage->setColumnWidth(0, 60);
    tableCompetitorsInStage->setColumnOrdering(0, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"No");
    tableCompetitorsInStage->setColumnWidth(1, 40);
    tableCompetitorsInStage->setColumnOrdering(1, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"Name");
    tableCompetitorsInStage->setColumnWidth(2, 210);
    tableCompetitorsInStage->setColumnOrdering(2, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"Vehicle");
    tableCompetitorsInStage->setColumnWidth(3, 160);
    tableCompetitorsInStage->setColumnOrdering(3, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"Start Time");
    tableCompetitorsInStage->setColumnWidth(4, 80);
    tableCompetitorsInStage->setColumnOrdering(4, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"%");
    tableCompetitorsInStage->setColumnWidth(5, 50);
    tableCompetitorsInStage->setColumnOrdering(5, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"Penalty");
    tableCompetitorsInStage->setColumnWidth(6, 70);
    tableCompetitorsInStage->setColumnOrdering(6, irr::gui::EGCO_NONE);
    tableCompetitorsInStage->addColumn(L"Repair");
    tableCompetitorsInStage->setColumnWidth(7, 70);
    tableCompetitorsInStage->setColumnOrdering(7, irr::gui::EGCO_NONE);


    window->setVisible(false);
}

MenuPageInGame::~MenuPageInGame()
{
    //window->remove();
    menuPageInGame = 0;
    close();
}

bool MenuPageInGame::OnEvent(const irr::SEvent &event)
{
    //dprintf(MY_DEBUG_NOTE, "MenuPageEditor::OnEvent()\n");
    if (event.EventType == irr::EET_GUI_EVENT)
    {
        int id = event.GUIEvent.Caller->getID();

        switch (event.GUIEvent.EventType)
        {
            case irr::gui::EGET_ELEMENT_CLOSED:
            {
                switch (id)
                {
                    case MI_WINDOW:
                        //dprintf(MY_DEBUG_NOTE, "event on close editor window\n");
                        MenuManager::getInstance()->close();
                        return true;
                        break;
                    default:
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_BUTTON_CLICKED:
            {
                switch (id)
                {
                    case MI_BUTTONBACK:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::backbutton::clicked\n");
                        willOpenOtherWindow = false;
                        MenuManager::getInstance()->close();
                        return true;
                        break;
                    case MI_BUTTONLOAD:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::loadbutton::clicked\n");
                        willOpenOtherWindow = true;
                        MenuPageLoadSave::menuPageLoadSave->load = true;
                        MenuPageLoadSave::menuPageLoadSave->prevMP = MenuManager::MP_INGAME;
                        MenuManager::getInstance()->close();
                        MenuManager::getInstance()->open(MenuManager::MP_LOADSAVE);
                        return true;
                        break;
                    case MI_BUTTONSAVE:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::loadbutton::clicked\n");
                        willOpenOtherWindow = true;
                        MenuPageLoadSave::menuPageLoadSave->load = false;
                        MenuPageLoadSave::menuPageLoadSave->prevMP = MenuManager::MP_INGAME;
                        MenuManager::getInstance()->close();
                        MenuManager::getInstance()->open(MenuManager::MP_LOADSAVE);
                        return true;
                        break;
                    case MI_BUTTONOPTIONS:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::optionsbutton::clicked\n");
                        MenuManager::getInstance()->open(MenuManager::MP_OPTIONS);
                        return true;
                        break;
                    case MI_BUTTONEXIT:
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::exitbutton::clicked\n");
                        TheGame::getInstance()->setTerminate();
                        return true;
                        break;
                    case MI_BUTTONNEXTSTAGE:
                    {
                        Stage* nextStage = RaceManager::getInstance()->getNextStage();
                        dprintf(MY_DEBUG_NOTE, "ingamemenu::nextstagebutton::clicked: selected next stage: %s\n",
                            nextStage?nextStage->getName().c_str():"-");
                        if (nextStage)
                        {
                            willOpenOtherWindow = true;
                            MenuPageStage::menuPageStage->selectedRace = 0;
                            MenuPageStage::menuPageStage->selectedStage = nextStage;
                            MenuPageStage::menuPageStage->selectedVehicleType = Player::getInstance()->getVehicle()->getVehicleType();
                            MenuPageStage::menuPageStage->newRace = false;
                            MenuManager::getInstance()->close();
                            MenuManager::getInstance()->open(MenuManager::MP_STAGE);
                            //GamePlay::getInstance()->startNewGame(selectedRace, selectedVehicleType);
                        }
                        return true;
                        break;
                    }
                };
                break;
            }
            
            case irr::gui::EGET_TABLE_CHANGED:
            {
                switch (id)
                {
                    case MI_TABLESTAGES:
                        refreshCompetitors((StageState*)tableStages->getCellData(tableStages->getSelected(), 0));
                        return true;
                        break;
                };
                break;
            }
        };
    }
    return false;
}

void MenuPageInGame::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageInGame::open()\n");
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(tableStages);
    
    // stop necessarry elements in the game
    VehicleManager::getInstance()->pause();
    TheGame::getInstance()->setInGame(false);
    Hud::getInstance()->setVisible(false);
}

void MenuPageInGame::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageInGame::close()\n");
    window->setVisible(false);

    // start necessarry elements in the game if returns
    if (!willOpenOtherWindow)
    {
        TheGame::getInstance()->setInGame(true);
        Hud::getInstance()->setVisible(true);
        VehicleManager::getInstance()->resume();
    }
    willOpenOtherWindow = false;
}

void MenuPageInGame::refresh()
{
    assert(!GamePlay::getInstance()->raceState.empty());

    buttonLoad->setEnabled(!GamePlay::getInstance()->loadableGames.empty());
    buttonNextStage->setEnabled(RaceManager::getInstance()->getNextStage()!=0 && Player::getInstance()->getStarter()==0);

    if (RaceManager::getInstance()->getCurrentRace())
    {
        irr::core::stringw str;
        str += RaceManager::getInstance()->getCurrentRace()->getLongName().c_str();
        staticTextRaceName->setText(str.c_str());
    }

    refreshStages();
    refreshCompetitors(GamePlay::getInstance()->raceState.back());
}

void MenuPageInGame::refreshStages()
{
    // ----------------------------
    // Stages
    // ----------------------------
    tableStages->clearRows();

    const stageStateList_t& stageStateList = GamePlay::getInstance()->raceState;
    unsigned int i = 0;
    for (stageStateList_t::const_iterator it = stageStateList.begin();
         it != stageStateList.end();
         it++, i++)
    {
        irr::core::stringw str;
        
        tableStages->addRow(i);

        str = L"";
        str += (*it)->stage->getLongName().c_str();
        tableStages->setCellText(i, 0, str.c_str());
        tableStages->setCellData(i, 0, (void*)(*it));
    }
}

void MenuPageInGame::refreshCompetitors(StageState* stageState)
{
    /*
    tableCompetitors->addColumn(L"Pos");
    tableCompetitors->addColumn(L"Time");
    tableCompetitors->addColumn(L"Num");
    tableCompetitors->addColumn(L"Name");
    */
    // ----------------------------
    // Stage result
    // ----------------------------
    tableCompetitors->clearRows();

    const competitorResultList_t& competitorResultList = stageState->competitorResultListStage;
    const irr::video::SColor playerColor(255, 255, 0, 0);
    bool player = false;
    unsigned int i = 0;
    unsigned int currTime = 0;
    unsigned int prevTime = 0;
    for (competitorResultList_t::const_iterator it = competitorResultList.begin();
         it != competitorResultList.end();
         it++, i++)
    {
        irr::core::stringw str;
        player = ((*it)->competitor == Player::getInstance()->getCompetitor());
        
        tableCompetitors->addRow(i);

        str = L"";
        str += (i+1);
        tableCompetitors->setCellText(i, 0, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 0, playerColor);
        //tableStages->setCellData(i, 0, (void*)(*it));

        str = L"";
        str += (*it)->competitor->getNum();
        tableCompetitors->setCellText(i, 1, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 1, playerColor);

        str = L"";
        str += (*it)->competitor->getName().c_str();
        tableCompetitors->setCellText(i, 2, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 2, playerColor);

        VehicleType* vt = VehicleTypeManager::getInstance()->getVehicleType((*it)->competitor->getVehicleTypeName());
        str = L"";
        if (vt)
        {
            str += vt->getLongName().c_str();
        }
        else
        {
            str += L"-";
        }
        tableCompetitors->setCellText(i, 3, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 3, playerColor);

        currTime = (*it)->stageTime + (*it)->stagePenaltyTime;
        str = L"";
        WStringConverter::addTimeToStr(str, currTime);
        tableCompetitors->setCellText(i, 4, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 4, playerColor);
        
        if (it != competitorResultList.begin())
        {
            str = L"+";
            WStringConverter::addTimeToStr(str, currTime - prevTime);
            tableCompetitors->setCellText(i, 5, str.c_str());
        }
        else // comment this out, if you need diff calculation for the previous competitor not to the first
        {
            prevTime = currTime;
        }
        if (player) tableCompetitors->setCellColor(i, 5, playerColor);

        str = L"";
        if ((*it)->stagePenaltyTime) WStringConverter::addTimeToStr(str, (*it)->stagePenaltyTime);
        tableCompetitors->setCellText(i, 6, str.c_str());
        if (player) tableCompetitors->setCellColor(i, 6, playerColor);
    }

    tableCompetitorsG->clearRows();

    const competitorResultList_t& competitorResultListG = stageState->competitorResultListOverall;
    i = 0;
    currTime = 0;
    prevTime = 0;
    for (competitorResultList_t::const_iterator it = competitorResultListG.begin();
         it != competitorResultListG.end();
         it++, i++)
    {
        irr::core::stringw str;
        player = ((*it)->competitor == Player::getInstance()->getCompetitor());
        
        tableCompetitorsG->addRow(i);

        str = L"";
        str += (i+1);
        tableCompetitorsG->setCellText(i, 0, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 0, playerColor);
        //tableStages->setCellData(i, 0, (void*)(*it));

        str = L"";
        str += (*it)->competitor->getNum();
        tableCompetitorsG->setCellText(i, 1, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 1, playerColor);

        str = L"";
        str += (*it)->competitor->getName().c_str();
        tableCompetitorsG->setCellText(i, 2, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 2, playerColor);

        VehicleType* vt = VehicleTypeManager::getInstance()->getVehicleType((*it)->competitor->getVehicleTypeName());
        str = L"";
        if (vt)
        {
            str += vt->getLongName().c_str();
        }
        else
        {
            str += L"-";
        }
        tableCompetitorsG->setCellText(i, 3, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 3, playerColor);

        currTime = (*it)->globalTime + (*it)->globalPenaltyTime;
        str = L"";
        WStringConverter::addTimeToStr(str, currTime);
        tableCompetitorsG->setCellText(i, 4, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 4, playerColor);

        if (it != competitorResultListG.begin())
        {
            str = L"+";
            WStringConverter::addTimeToStr(str, currTime - prevTime);
            tableCompetitorsG->setCellText(i, 5, str.c_str());
        }
        else // comment this out, if you need diff calculation for the previous competitor not to the first
        {
            prevTime = currTime;
        }
        if (player) tableCompetitorsG->setCellColor(i, 5, playerColor);

        str = L"";
        if ((*it)->globalPenaltyTime) WStringConverter::addTimeToStr(str, (*it)->globalPenaltyTime);
        tableCompetitorsG->setCellText(i, 6, str.c_str());
        if (player) tableCompetitorsG->setCellColor(i, 6, playerColor);
    }

    tableCompetitorsInStage->clearRows();

    if (GamePlay::getInstance()->raceEngine)
    {
        const RaceEngine::starterList_t& starters = GamePlay::getInstance()->raceEngine->starters;
        unsigned int baseCD = 0;
        i = 0;
        for (RaceEngine::starterList_t::const_iterator it = starters.begin();
             it != starters.end();
             it++, i++)
        {
            irr::core::stringw str;
            player = ((*it)->competitor == Player::getInstance()->getCompetitor());
        
            tableCompetitorsInStage->addRow(i);

            str = L"";
            str += (i+1);
            tableCompetitorsInStage->setCellText(i, 0, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 0, playerColor);

            str = L"";
            str += (*it)->competitor->getNum();
            tableCompetitorsInStage->setCellText(i, 1, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 1, playerColor);

            str = L"";
            str += (*it)->competitor->getName().c_str();
            tableCompetitorsInStage->setCellText(i, 2, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 2, playerColor);

            VehicleType* vt = VehicleTypeManager::getInstance()->getVehicleType((*it)->competitor->getVehicleTypeName());
            str = L"";
            if (vt)
            {
                str += vt->getLongName().c_str();
            }
            else
            {
                str += L"-";
            }
            tableCompetitorsInStage->setCellText(i, 3, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 3, playerColor);

            str = L"";
            if ((*it)->startingCD)
            {
                baseCD += (*it)->startingCD;
                WStringConverter::addTimeToStr(str, baseCD);
            }
            else
            {
                str += L"started";
            }
            tableCompetitorsInStage->setCellText(i, 4, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 4, playerColor);

            str = L"";
            if ((*it)->startingCD)
            {
                str += L"-";
            }
            else
            {
                unsigned int perc = 0;
                if (player)
                {
                    //str += L"~";
                    if (RaceManager::getInstance()->getCurrentStage()->getAIPointList().size())
                    {
                        perc = (unsigned int)((Player::getInstance()->getVehicle()->getDistance() * 100.f) / RaceManager::getInstance()->getCurrentStage()->getAIPointList().back()->getGlobalDistance());
                        if (perc > 100)
                        {
                            perc = 100;
                        }
                    }
                }
                else
                {
                    if (RaceManager::getInstance()->getCurrentStage()->getAIPointList().size())
                    {
                        perc = ((*it)->prevPointNum * 100) / RaceManager::getInstance()->getCurrentStage()->getAIPointList().size();
                    }
                }
                str += perc;
                str += L"%";
            }
            tableCompetitorsInStage->setCellText(i, 5, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 5, playerColor);

            str = L"";
            if ((*it)->startingCD == 0 && (*it)->penaltyTime)
            {
                WStringConverter::addTimeToStr(str, (*it)->penaltyTime);
            }
            tableCompetitorsInStage->setCellText(i, 6, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 6, playerColor);

            str = L"";
            if ((*it)->startingCD == 0)
            {
                if ((*it)->crashedForever)
                {
                    str += L"out";
                }
                else if ((*it)->crashTime)
                {
                    WStringConverter::addTimeToStr(str, (*it)->crashTime);
                }
            }
            tableCompetitorsInStage->setCellText(i, 7, str.c_str());
            if (player) tableCompetitorsInStage->setCellColor(i, 7, playerColor);
        }
    }
}
