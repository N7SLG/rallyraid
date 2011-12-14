
#include "MenuPageEditor.h"
#include "TheGame.h"
#include "stdafx.h"
#include "WStringConverter.h"
#include "ConfigDirectory.h"

#include "TheEarth.h"
#include "ObjectWire.h"
#include "ObjectPool.h"
#include "ObjectPoolManager.h"
#include "ObjectWireGlobalObject.h"
#include "RaceManager.h"
#include "Race.h"
#include "Settings.h"
#include "RoadManager.h"
#include "RoadTypeManager.h"
#include "Road.h"
#include "Day.h"
#include "Stage.h"
#include "RoadType.h"
#include "RoadTypeManager.h"
#include "ItinerManager.h"
#include "ItinerPoint.h"
#include "AIPoint.h"
#include "WayPointManager.h"
#include "WayPoint.h"
#include "GamePlay.h"
#include "Player.h"
#include "Competitor.h"
#include "VehicleType.h"
#include "VehicleTypeManager.h"
#include "Terrain_defs.h"
#include "MessageManager.h"
#include "error.h"


MenuPageEditor* MenuPageEditor::menuPageEditor = 0;

MenuPageEditor::MenuPageEditor()
    : window(0),
      tableSelected(0),
      tableAction(0),
      tableTiles(0),
      tableObjectWire(0),
      tableObjectWireTiles(0),
      tableObjectPool(0),
      tableRaceManager(0),
      tableRoadManagerS(0),
      tableRoadManagerV(0),
      tableRoadTypes(0),
      tableRoads(0),
      tableItiner(0),
      tableItiner2(0),
      editBoxNewRoadFilename(0),
      editBoxNewRoadName(0),
      editBoxNewRoadDataFilename(0),
      editBoxItinerGD(0),
      editBoxItinerLD(0),
      editBoxItinerDescription(0),
      editBoxResetX(0),
      editBoxResetY(0),
      editBoxResetZ(0),
      editBoxScale(0),
      editBoxRot(0),
      checkBoxRender(0),
      checkBoxRenderAllRoads(0),
      staticTextItinerGD(0),
      itinerImage(0),
      itinerImage2(0),
      objectImage(0),
      objectImage2(0),
      roadImage(0),
      buttonAction(0),
      currentAction(A_None),
      material(),
      lastTick(0),
      doRender(true),
      renderAllRoads(false)
{
    menuPageEditor = this;
    material.MaterialType = irr::video::EMT_SOLID;
    material.setFlag(irr::video::EMF_LIGHTING, false);
    material.Thickness = 2.0f;

    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width-350, 50, TheGame::getInstance()->getScreenSize().Width-10, TheGame::getInstance()->getScreenSize().Height-300),
        false,
        L"Editor",
        0,
        MI_WINDOW);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(2,22,42,42),
        window,
        MI_BUTTONREFRESH,
        L"Refresh", L"Refresh the content of this window.");

    buttonAction = TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(44,22,84,42),
        window,
        MI_BUTTONACTION,
        L"action", L"Switch to game mode, then use left mouse button for the action.");
/*
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(44,22,84,42),
        window,
        MI_BUTTONCREATEROAD,
        L"new road");
*/
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(86,22,146,42),
        window,
        MI_BUTTONACTIVATE,
        L"activate race", L"Activate the current race/stage.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(148,22,188,42),
        window,
        MI_BUTTONRESET,
        L"reset", L"Put the car to a give position by the tile input.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(190,22,230,42),
        window,
        MI_BUTTONRELOAD,
        L"reload", L"Reset the car into the camera position.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(232,22,280,42),
        window,
        MI_BUTTONJUMPEND,
        L"jump end", L"Jump to the last itiner point of the current stage.");

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 44), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 46)),
        window,
        true,
        true,
        MI_TABCONTROL);

    // ----------------------------
    // Selected tab
    // ----------------------------
    irr::gui::IGUITab* tabSelected = tc->addTab(L"S", MI_TABSELECTED);

    checkBoxRender = TheGame::getInstance()->getEnv()->addCheckBox(doRender,
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(100, 20)),
        tabSelected,
        MI_CBRENDER,
        L"render helpers");

    editBoxResetX = TheGame::getInstance()->getEnv()->addEditBox(L"tileX",
        irr::core::recti(irr::core::position2di(110, 0), irr::core::dimension2di(48, 20)),
        true,
        tabSelected,
        MI_EBRESETX);

    editBoxResetY = TheGame::getInstance()->getEnv()->addEditBox(L"30",
        irr::core::recti(irr::core::position2di(160, 0), irr::core::dimension2di(48, 20)),
        true,
        tabSelected,
        MI_EBRESETY);

    editBoxResetZ = TheGame::getInstance()->getEnv()->addEditBox(L"tileY",
        irr::core::recti(irr::core::position2di(210, 0), irr::core::dimension2di(48, 20)),
        true,
        tabSelected,
        MI_EBRESETZ);

    checkBoxRenderAllRoads = TheGame::getInstance()->getEnv()->addCheckBox(renderAllRoads,
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(100, 20)),
        tabSelected,
        MI_CBRENDERALLROADS,
        L"render all roads");

    tableSelected = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 44), irr::core::dimension2di(tabSelected->getRelativePosition().getSize().Width, (tabSelected->getRelativePosition().getSize().Height-44)/3-1)),
        tabSelected,
        MI_TABLESELECTED,
        true);

    tableSelected->addColumn(L"item");
    tableSelected->setColumnWidth(0, 100);
    tableSelected->addColumn(L"selected");
    tableSelected->setColumnWidth(1, 200);

    tableAction = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 44+(tabSelected->getRelativePosition().getSize().Height-44)/3+1), irr::core::dimension2di(tabSelected->getRelativePosition().getSize().Width, ((tabSelected->getRelativePosition().getSize().Height-44)*2)/3-1)),
        tabSelected,
        MI_TABLEACTION,
        true);

    tableAction->addColumn(L"Action");
    tableAction->setColumnWidth(0, 200);
    tableAction->addRow(A_None);
    tableAction->addRow(A_AddObjectGlobal);
    tableAction->addRow(A_AddObjectRace);
    tableAction->addRow(A_AddObjectDay);
    tableAction->addRow(A_AddObjectStage);
    tableAction->addRow(A_AddAIPoint);
    tableAction->addRow(A_AddItinerPoint);
    tableAction->addRow(A_AddWayPoint);
    tableAction->addRow(A_AddHeightModifier);
    tableAction->addRow(A_AddHeightModifierLine);
    tableAction->addRow(A_AddHeightModifierSquare);
    tableAction->addRow(A_AddRoadPoint);
    tableAction->addRow(A_AddRoadPointBegin);
    tableAction->addRow(A_RemoveObjectGlobal);
    tableAction->addRow(A_RemoveObjectRace);
    tableAction->addRow(A_RemoveObjectDay);
    tableAction->addRow(A_RemoveObjectStage);
    tableAction->addRow(A_RemoveAIPoint);
    tableAction->addRow(A_RemoveItinerPoint);
    tableAction->addRow(A_RemoveWayPoint);
    tableAction->addRow(A_RemoveRoadPoint);
    tableAction->addRow(A_RemoveRoadPointBegin);
    tableAction->setCellText(A_None, 0, L"none");
    tableAction->setCellText(A_AddObjectGlobal, 0, L"------------[ ADD ]------------"/*L"not used"*//*L"add object global"*/);
    tableAction->setCellText(A_AddObjectRace, 0, L"add object race");
    tableAction->setCellText(A_AddObjectDay, 0, L"add object day");
    tableAction->setCellText(A_AddObjectStage, 0, L"add object stage");
    tableAction->setCellText(A_AddAIPoint, 0, L"add AI point");
    tableAction->setCellText(A_AddItinerPoint, 0, L"add itiner point (1x)");
    tableAction->setCellText(A_AddWayPoint, 0, L"add waypoint (1x)");
    tableAction->setCellText(A_AddHeightModifier, 0, L"add height modifier");
    tableAction->setCellText(A_AddHeightModifierLine, 0, L"add height modifier line (1x)");
    tableAction->setCellText(A_AddHeightModifierSquare, 0, L"add height modifier square (1x)");
    tableAction->setCellText(A_AddRoadPoint, 0, L"add road point");
    tableAction->setCellText(A_AddRoadPointBegin, 0, L"add road point begin");
    tableAction->setCellText(A_RemoveObjectGlobal, 0, L"----------[ REMOVE ]----------"/*L"not used"*//*L"remove object global"*/);
    tableAction->setCellText(A_RemoveObjectRace, 0, L"remove object race");
    tableAction->setCellText(A_RemoveObjectDay, 0, L"remove object day");
    tableAction->setCellText(A_RemoveObjectStage, 0, L"remove object stage");
    tableAction->setCellText(A_RemoveRoadPoint, 0, L"remove road point");
    tableAction->setCellText(A_RemoveRoadPointBegin, 0, L"remove road point begin");
    tableAction->setCellText(A_RemoveItinerPoint, 0, L"remove itiner point");
    tableAction->setCellText(A_RemoveAIPoint, 0, L"remove AI point");
    tableAction->setCellText(A_RemoveWayPoint, 0, L"remove waypoint");

    // ----------------------------
    // Tiles tab
    // ----------------------------
    irr::gui::IGUITab* tabTiles = tc->addTab(L"Tls", MI_TABTILES);

    tableTiles = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabTiles->getRelativePosition().getSize()),
        tabTiles,
        MI_TABLETILES,
        true);

    tableTiles->addColumn(L"#");
    tableTiles->addColumn(L"X");
    tableTiles->setColumnWidth(1, 50);
    tableTiles->addColumn(L"Y");
    tableTiles->setColumnWidth(2, 50);
    tableTiles->addColumn(L"in use");
    tableTiles->addColumn(L"roads");

    // ----------------------------
    // ObjectWire tab
    // ----------------------------
    irr::gui::IGUITab* tabObjectWire = tc->addTab(L"OW_G", MI_TABOBJECTWIRE);

    tableObjectWire = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabObjectWire->getRelativePosition().getSize()),
        tabObjectWire,
        MI_TABLEOBJECTWIRE,
        true);

    tableObjectWire->addColumn(L"#");
    tableObjectWire->addColumn(L"pos");
    tableObjectWire->addColumn(L"objects");

    // ----------------------------
    // ObjectWireTiles tab
    // ----------------------------
    irr::gui::IGUITab* tabObjectWireTiles = tc->addTab(L"OW_T", MI_TABOBJECTWIRETILES);

    tableObjectWireTiles = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabObjectWireTiles->getRelativePosition().getSize()),
        tabObjectWireTiles,
        MI_TABLEOBJECTWIRETILES,
        true);

    tableObjectWireTiles->addColumn(L"#");
    tableObjectWireTiles->addColumn(L"X");
    tableObjectWireTiles->addColumn(L"Y");
    tableObjectWireTiles->addColumn(L"objects");
    tableObjectWireTiles->addColumn(L"visible");

    // ----------------------------
    // ObjectPool tab
    // ----------------------------
    irr::gui::IGUITab* tabObjectPool = tc->addTab(L"OPool", MI_TABOBJECTPOOL);

    TheGame::getInstance()->getEnv()->addStaticText(L"Scale (XYZ)",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabObjectPool)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxScale = TheGame::getInstance()->getEnv()->addEditBox(L"1",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 0), irr::core::dimension2di(68, 20)),
        true,
        tabObjectPool,
        MI_EBSCALE);

    TheGame::getInstance()->getEnv()->addStaticText(L"Rotation (Y)",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH+70, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabObjectPool)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxRot = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH*2+70, 0), irr::core::dimension2di(68, 20)),
        true,
        tabObjectPool,
        MI_EBROT);

    objectImage = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(0, 1*22), irr::core::dimension2di(64,64)),
        tabObjectPool);
    objectImage->setScaleImage(true);
    objectImage->setImage(0);

    objectImage2 = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(96, 1*22), irr::core::dimension2di(64,64)),
        tabObjectPool);
    objectImage2->setScaleImage(true);
    objectImage2->setImage(0);

    tableObjectPool = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 22+66), irr::core::dimension2di(tabObjectPool->getRelativePosition().getSize().Width, tabObjectPool->getRelativePosition().getSize().Height-(22+66))),
        tabObjectPool,
        MI_TABLEOBJECTPOOL,
        true);

    tableObjectPool->addColumn(L"#");
    tableObjectPool->addColumn(L"name");
    tableObjectPool->setColumnWidth(1, 80);
    tableObjectPool->addColumn(L"cat");
    tableObjectPool->addColumn(L"num");
    tableObjectPool->addColumn(L"in use");
    tableObjectPool->addColumn(L"in cache");

    // ----------------------------
    // RaceManager tab
    // ----------------------------
    irr::gui::IGUITab* tabRaceManager = tc->addTab(L"RaM", MI_TABRACEMANAGER);

    tableRaceManager = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabRaceManager->getRelativePosition().getSize()),
        tabRaceManager,
        MI_TABLERACEMANAGER,
        true);

    tableRaceManager->addColumn(L"#");
    tableRaceManager->addColumn(L"name");
    tableRaceManager->setColumnWidth(1, 80);
    tableRaceManager->addColumn(L"long name");
    tableRaceManager->setColumnWidth(2, 180);

    // ----------------------------
    // RoadManager tab
    // ----------------------------
    irr::gui::IGUITab* tabRoadManager = tc->addTab(L"RoM", MI_TABROADMANAGER);

    tableRoadManagerS = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabRoadManager->getRelativePosition().getSize().Width, (tabRoadManager->getRelativePosition().getSize().Height-66)/3-2)),
        tabRoadManager,
        MI_TABLEROADMANAGERS,
        true);

    tableRoadManagerS->addColumn(L"#");
    tableRoadManagerS->addColumn(L"tile");
    tableRoadManagerS->addColumn(L"size");

    tableRoadManagerV = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, ((tabRoadManager->getRelativePosition().getSize().Height-66)*1)/3+2), irr::core::dimension2di(tabRoadManager->getRelativePosition().getSize().Width, (tabRoadManager->getRelativePosition().getSize().Height-66)/3-2)),
        tabRoadManager,
        MI_TABLEROADMANAGERV,
        true);

    tableRoadManagerV->addColumn(L"#");
    tableRoadManagerV->addColumn(L"road");
    tableRoadManagerV->addColumn(L"begin");
    tableRoadManagerV->addColumn(L"end");

    roadImage = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(0, ((tabRoadManager->getRelativePosition().getSize().Height-66)*2)/3+2+1), irr::core::dimension2di(64,64)),
        tabRoadManager);
    roadImage->setScaleImage(true);
    roadImage->setImage(0);

    tableRoadTypes = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, ((tabRoadManager->getRelativePosition().getSize().Height-66)*2)/3+2+66), irr::core::dimension2di(tabRoadManager->getRelativePosition().getSize().Width, (tabRoadManager->getRelativePosition().getSize().Height-66)/3-2)),
        tabRoadManager,
        MI_TABLEROADTYPES,
        true);

    tableRoadTypes->addColumn(L"#");
    tableRoadTypes->addColumn(L"type");
    tableRoadTypes->setColumnWidth(1, 100);

    /*
    // ----------------------------
    // Roads tab
    // ----------------------------
    irr::gui::IGUITab* tabRoads = tc->addTab(L"Roads", MI_TABROADS);

    editBoxNewRoadName = TheGame::getInstance()->getEnv()->addEditBox(L"new road name",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width, 20)),
        true,
        tabRoads,
        MI_EBNEWROADNAME);

    editBoxNewRoadFilename = TheGame::getInstance()->getEnv()->addEditBox(L"new road filename",
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width, 20)),
        true,
        tabRoads,
        MI_EBNEWROADFILENAME);

    editBoxNewRoadDataFilename = TheGame::getInstance()->getEnv()->addEditBox(L"new road data filename",
        irr::core::recti(irr::core::position2di(0, 2*22), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width, 20)),
        true,
        tabRoads,
        MI_EBNEWROADDATAFILENAME);

    tableRoads = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 3*22), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width, tabRoads->getRelativePosition().getSize().Height-(3*22))),
        //irr::core::recti(irr::core::position2di(0, 0), tabRoads->getRelativePosition().getSize()),
        tabRoads,
        MI_TABLEROADS,
        true);

    tableRoads->addColumn(L"#");
    tableRoads->addColumn(L"name");
    tableRoads->setColumnWidth(1, 60);
    tableRoads->addColumn(L"type");
    tableRoads->setColumnWidth(2, 60);
    tableRoads->addColumn(L"size");
    tableRoads->addColumn(L"loaded");
    tableRoads->setColumnWidth(4, 30);
    tableRoads->addColumn(L"filename");
    tableRoads->addColumn(L"data");
    */

    // ----------------------------
    // Itiner tab
    // ----------------------------
    irr::gui::IGUITab* tabItiner = tc->addTab(L"Iti", MI_TABITINER);

    /*editBoxItinerGD = TheGame::getInstance()->getEnv()->addEditBox(L"global distance",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width, 20)),
        true,
        tabItiner,
        MI_EBITINERGD);
*/
    
    staticTextItinerGD = TheGame::getInstance()->getEnv()->addStaticText(L"0",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width, 20)),
        true,
        false,
        tabItiner,
        MI_STITINERGD);

    TheGame::getInstance()->getEnv()->addStaticText(L"Local dist.",
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabItiner)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxItinerLD = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 22), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabItiner,
        MI_EBITINERLD);

    TheGame::getInstance()->getEnv()->addStaticText(L"Description",
        irr::core::recti(irr::core::position2di(0, 2*22), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabItiner)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxItinerDescription = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 2*22), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabItiner,
        MI_EBITINERDESCRIPTION);

    itinerImage = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(0, 3*22), irr::core::dimension2di(64,64)),
        tabItiner);
    itinerImage->setScaleImage(true);
    itinerImage->setImage(0);

    itinerImage2 = TheGame::getInstance()->getEnv()->addImage(
        irr::core::recti(irr::core::position2di(96, 3*22), irr::core::dimension2di(64,64)),
        tabItiner);
    itinerImage2->setScaleImage(true);
    itinerImage2->setImage(0);

  /* delme
    tableItiner = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 3*22), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width, tabItiner->getRelativePosition().getSize().Height-(3*22))),
        //irr::core::recti(irr::core::position2di(0, 0), tabRoads->getRelativePosition().getSize()),
        tabItiner,
        MI_TABLEITINER,
        true);
  */

    tableItiner = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 3*22+66), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width, (tabItiner->getRelativePosition().getSize().Height-(3*22+66))/2 - 1)),
        //irr::core::recti(irr::core::position2di(0, 0), tabRoads->getRelativePosition().getSize()),
        tabItiner,
        MI_TABLEITINER,
        true);

    tableItiner->addColumn(L"itiner image");
    tableItiner->setColumnWidth(0, 200);

    tableItiner2 = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 3*22+66 + (tabItiner->getRelativePosition().getSize().Height-(3*22+66))/2 + 1), irr::core::dimension2di(tabItiner->getRelativePosition().getSize().Width, (tabItiner->getRelativePosition().getSize().Height-(3*22+66))/2-1)),
        //irr::core::recti(irr::core::position2di(0, 0), tabRoads->getRelativePosition().getSize()),
        tabItiner,
        MI_TABLEITINER2,
        true);

    tableItiner2->addColumn(L"itiner image 2");
    tableItiner2->setColumnWidth(0, 200);

    window->setVisible(false);
}

MenuPageEditor::~MenuPageEditor()
{
    //window->remove();
    close();
    menuPageEditor = 0;
}

bool MenuPageEditor::OnEvent(const irr::SEvent &event)
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
                        TheGame::getInstance()->resetMouseCursor();
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
                    case MI_BUTTONREFRESH:
                        //dprintf(MY_DEBUG_NOTE, "editor::refreshbutton clicked\n");
                        refresh();
                        return true;
                        break;
                    case MI_BUTTONACTION:
                        buttonAction->setText(L"ACTION");
                        MenuManager::getInstance()->clearEventReceiver();
                        break;
                    case MI_BUTTONCREATEROAD:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::newRoad\n");
                        std::string roadName;
                        std::string roadFilename;
                        std::string roadDataFilename;
                        WStringConverter::toString(editBoxNewRoadFilename->getText(), roadFilename);
                        WStringConverter::toString(editBoxNewRoadName->getText(), roadName);
                        WStringConverter::toString(editBoxNewRoadDataFilename->getText(), roadDataFilename);
                        RoadManager::roadMap_t::const_iterator rit = RoadManager::getInstance()->roadMap.find(roadName);
                        if (rit == RoadManager::getInstance()->roadMap.end() &&
                            !roadName.empty() &&
                            RoadTypeManager::getInstance()->editorRoadType)
                        {
                            RoadManager::getInstance()->roadMap[roadName] =
                                new Road(roadFilename, roadName, roadDataFilename, RoadTypeManager::getInstance()->editorRoadType, true);
                            refreshRoads();
                        }
                        else
                        {
                            PrintMessage(101, "Unable to create road with name: '%s' and type '%s'.",
                                roadName.c_str(), 
                                RoadTypeManager::getInstance()->editorRoadType?RoadTypeManager::getInstance()->editorRoadType->name.c_str():"<empty>");
                        }
                        return true;
                        break;
                    }
                    case MI_BUTTONACTIVATE:
                        dprintf(MY_DEBUG_NOTE, "editor::activatebutton clicked\n");
                        if (RaceManager::getInstance()->editorStage)
                        {
                            RaceManager::getInstance()->activateStage(RaceManager::getInstance()->editorStage);
                            if (!RaceManager::getInstance()->editorStage->itinerPointList.empty())
                            {
                                ItinerManager::getInstance()->editorGlobalDistance = RaceManager::getInstance()->editorStage->itinerPointList.back()->getGlobalDistance();
                            }
                            else
                            {
                                ItinerManager::getInstance()->editorGlobalDistance = 0;
                            }
                            refreshSelected();
                            refreshItinerGD();
                        }
                        else
                        {
                            PrintMessage(102, "Unable to activate race/stage, because no stage is selected.");
                        }
                        return true;
                        break;
                    case MI_BUTTONRESET:
                    {
                        int resetX = 0;
                        int resetY = 0;
                        int resetZ = 0;
                        WStringConverter::toInt(editBoxResetX->getText(), resetX);
                        WStringConverter::toInt(editBoxResetY->getText(), resetY);
                        WStringConverter::toInt(editBoxResetZ->getText(), resetZ);
                        if (resetX != 0 && resetY != 0 && resetZ != 0)
                        {
                            if (resetX < 0) resetX = -resetX;
                            if (resetZ > 0) resetZ = -resetZ;
                            printf("reset: %d, %f (%d), %d\n", resetX, (float)TheEarth::getInstance()->getEarthHeight(resetX, -resetZ)+(float)resetY, resetY, resetZ);
                            MenuManager::getInstance()->close();
                            GamePlay::getInstance()->startStage(
                                RaceManager::getInstance()->getCurrentStage(),
                                VehicleTypeManager::getInstance()->getVehicleType(Player::getInstance()->getCompetitor()->getVehicleTypeName()),
                                irr::core::vector3df((float)resetX*TILE_SIZE_F+TILE_HSIZE_F,(float)TheEarth::getInstance()->getEarthHeight(resetX, -resetZ)+(float)resetY,(float)resetZ*TILE_SIZE_F-TILE_HSIZE_F),
                                Player::getInstance()->getSavedRot(),
                                true);
                        }
                        else
                        {
                            printf("unable to reset because reset fields are not int\n");
                            MessageManager::getInstance()->addText(L"unable to reset because reset fields are not int", 1);
                            PrintMessage(103, "Unable to reset because reset fields are not int.");
                        }
                        return true;
                        break;
                    }
                    case MI_BUTTONRELOAD:
                    {
                        MenuManager::getInstance()->close();
                        GamePlay::getInstance()->startStage(
                            RaceManager::getInstance()->getCurrentStage(),
                            VehicleTypeManager::getInstance()->getVehicleType(Player::getInstance()->getCompetitor()->getVehicleTypeName()),
                            TheGame::getInstance()->getCamera()->getPosition()+OffsetManager::getInstance()->getOffset()
                            /*Player::getInstance()->getSavedPos()*/,
                            Player::getInstance()->getSavedRot(),
                            true);
                        return true;
                        break;
                    }
                    case MI_BUTTONJUMPEND:
                    {
                        if (RaceManager::getInstance()->editorStage && !RaceManager::getInstance()->editorStage->itinerPointList.empty())
                        {
                            ItinerPoint* ip = RaceManager::getInstance()->editorStage->itinerPointList.back();
                            MenuManager::getInstance()->close();
                            GamePlay::getInstance()->startStage(
                                RaceManager::getInstance()->getCurrentStage(),
                                VehicleTypeManager::getInstance()->getVehicleType(Player::getInstance()->getCompetitor()->getVehicleTypeName()),
                                ip->getPos(),
                                Player::getInstance()->getSavedRot(),
                                true);
                        }
                        else
                        {
                            PrintMessage(104, "Unable to jump to the end because %s.", RaceManager::getInstance()->editorStage==0?"no stage selected":"itiner point list is empty of the stage");
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
                    case MI_TABLEACTION:
                        currentAction = (Action)tableAction->getSelected();
                        return true;
                        break;
                    case MI_TABLEOBJECTPOOL:
                        ObjectPoolManager::getInstance()->editorPool = (ObjectPool*)tableObjectPool->getCellData(tableObjectPool->getSelected(), 0);
                        objectImage->setImage(((ObjectPool*)tableObjectPool->getCellData(tableObjectPool->getSelected(), 0))->texture);
                        objectImage2->setImage(((ObjectPool*)tableObjectPool->getCellData(tableObjectPool->getSelected(), 0))->texture2);
                        refreshSelected();
                        return true;
                        break;
                    case MI_TABLERACEMANAGER:
                        RaceManager::getInstance()->editorRace = (Race*)tableRaceManager->getCellData(tableRaceManager->getSelected(), 0);
                        refreshSelected();
                        return true;
                        break;
                    case MI_TABLEROADTYPES:
                        RoadTypeManager::getInstance()->editorRoadType = (RoadType*)tableRoadTypes->getCellData(tableRoadTypes->getSelected(), 0);
                        roadImage->setImage(((RoadType*)tableRoadTypes->getCellData(tableRoadTypes->getSelected(), 0))->texture);
                        refreshSelected();
                        return true;
                        break;
                    case MI_TABLEROADS:
                        RoadManager::getInstance()->editorRoad = (Road*)tableRoads->getCellData(tableRoads->getSelected(), 0);
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        return true;
                        break;
                    case MI_TABLEITINER:
                    {
                        WStringConverter::toString(tableItiner->getCellText(tableItiner->getSelected(), 0), ItinerManager::getInstance()->editorItinerImageName);
                        itinerImage->setImage((irr::video::ITexture*)tableItiner->getCellData(tableItiner->getSelected(), 0));
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        return true;
                        break;
                    }
                    case MI_TABLEITINER2:
                    {
                        WStringConverter::toString(tableItiner2->getCellText(tableItiner2->getSelected(), 0), ItinerManager::getInstance()->editorItinerImageName2);
                        itinerImage2->setImage((irr::video::ITexture*)tableItiner2->getCellData(tableItiner2->getSelected(), 0));
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        return true;
                        break;
                    }
                };
                break;
            }
            case irr::gui::EGET_TABLE_SELECTED_AGAIN:
            {
                switch (id)
                {
                    case MI_TABLERACEMANAGER:
                        RaceManager::getInstance()->editorRace = (Race*)tableRaceManager->getCellData(tableRaceManager->getSelected(), 0);
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORRACE);
                        return true;
                        break;
                    case MI_TABLESELECTED:
                        switch (tableSelected->getSelected())
                        {
                            case 1:
                                if (RaceManager::getInstance()->editorRace) MenuManager::getInstance()->open(MenuManager::MP_EDITORRACE);
                                break;
                            case 2:
                                if (RaceManager::getInstance()->editorDay) MenuManager::getInstance()->open(MenuManager::MP_EDITORDAY);
                                break;
                            case 3:
                                if (RaceManager::getInstance()->editorStage) MenuManager::getInstance()->open(MenuManager::MP_EDITORSTAGE);
                                break;
                            case 4:
                                if (RoadManager::getInstance()->editorRoad) MenuManager::getInstance()->open(MenuManager::MP_EDITORROAD);
                            case 5:
                                //if (RoadTypeManager::getInstance()->editorRoadType) MenuManager::getInstance()->open(MenuManager::MP_EDITORROADTYPE);
                                break;
                        }
                        return true;
                        break;
                    case MI_TABLEROADS:
                        RoadManager::getInstance()->editorRoad = (Road*)tableRoads->getCellData(tableRoads->getSelected(), 0);
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORROAD);
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_EDITBOX_ENTER:
                if (id == MI_EBITINERLD)
                {
                    if (RaceManager::getInstance()->editorStage &&
                        !RaceManager::getInstance()->editorStage->AIPointList.empty() &&
                        !RaceManager::getInstance()->editorStage->itinerPointList.empty())
                    {
                        irr::core::stringw str;
                        str += (int)(RaceManager::getInstance()->editorStage->AIPointList.back()->getGlobalDistance()-RaceManager::getInstance()->editorStage->itinerPointList.back()->getGlobalDistance());
                        editBoxItinerLD->setText(str.c_str());
                    }
                    else
                    {
                        editBoxItinerLD->setText(L"0");
                    }
                }
            case irr::gui::EGET_EDITBOX_CHANGED:
            {
                switch (id)
                {
                    case MI_EBNEWROADNAME:
                        refreshRoadEditBoxes(editBoxNewRoadName->getText());
                        break;
                    /*
                    case MI_EBITINERGD:
                        WStringConverter::toFloat(editBoxItinerGD->getText(), ItinerManager::getInstance()->editorGlobalDistance);
                        dprintf(MY_DEBUG_INFO, "set itiner global distance: %f\n", ItinerManager::getInstance()->editorGlobalDistance);
                        break;
                    */
                    case MI_EBITINERLD:
                        WStringConverter::toFloat(editBoxItinerLD->getText(), ItinerManager::getInstance()->editorLocalDistance);
                        dprintf(MY_DEBUG_INFO, "set itiner local distance: %f\n", ItinerManager::getInstance()->editorLocalDistance);
                        break;
                    case MI_EBITINERDESCRIPTION:
                        WStringConverter::toString(editBoxItinerDescription->getText(), ItinerManager::getInstance()->editorDescription);
                        dprintf(MY_DEBUG_INFO, "set itiner description: [%s]\n", ItinerManager::getInstance()->editorDescription.c_str());
                        break;
                    case MI_EBSCALE:
                        WStringConverter::toFloat(editBoxScale->getText(), ObjectPoolManager::getInstance()->editorScale);
                        dprintf(MY_DEBUG_INFO, "set object scale: %f\n", ObjectPoolManager::getInstance()->editorScale);
                        break;
                    case MI_EBROT:
                        WStringConverter::toFloat(editBoxRot->getText(), ObjectPoolManager::getInstance()->editorRot);
                        dprintf(MY_DEBUG_INFO, "set object rotation: %f\n", ObjectPoolManager::getInstance()->editorRot);
                        break;
                }
                break;
            }
            case irr::gui::EGET_CHECKBOX_CHANGED:
            {
                switch (id)
                {
                    case MI_CBRENDER:
                        doRender = checkBoxRender->isChecked();
                        dprintf(MY_DEBUG_INFO, "set render to: %u\n", doRender);
                        break;
                    case MI_CBRENDERALLROADS:
                        renderAllRoads = checkBoxRenderAllRoads->isChecked();
                        dprintf(MY_DEBUG_INFO, "set render all roads to: %u\n", renderAllRoads);
                        break;
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageEditor::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditor::open()\n");
    currentAction = A_None;
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageEditor::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditor::close()\n");
    currentAction = A_None;
    window->setVisible(false);
}

void MenuPageEditor::refresh()
{
    refreshSelected();
    refreshAction();
    refreshTiles();
    refreshObjectWire();
    refreshObjectWireTiles();
    refreshObjectPool();
    refreshRaceManager();
    refreshRoadManager();
    refreshRoads();
    refreshItiner();
}

void MenuPageEditor::refreshSelected()
{
    // ----------------------------
    // Selected
    // ----------------------------
    tableSelected->clearRows();

    const TheEarth::tileMap_t& tileMap = TheEarth::getInstance()->getTileMap();
    unsigned int i = 0;
    irr::core::stringw str;
        
    tableSelected->addRow(i);
    str = L"object";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (ObjectPoolManager::getInstance()->editorPool)
    {
        str += ObjectPoolManager::getInstance()->editorPool->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());

    i = 1;
    tableSelected->addRow(i);
    str = L"race";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (RaceManager::getInstance()->editorRace)
    {
        str += RaceManager::getInstance()->editorRace->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());
    tableSelected->setCellData(i, 1, (void*)RaceManager::getInstance()->editorRace);

    i = 2;
    tableSelected->addRow(i);
    str = L"day";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (RaceManager::getInstance()->editorDay)
    {
        str += RaceManager::getInstance()->editorDay->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());
    tableSelected->setCellData(i, 1, (void*)RaceManager::getInstance()->editorDay);

    i = 3;
    tableSelected->addRow(i);
    str = L"stage";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (RaceManager::getInstance()->editorStage)
    {
        str += RaceManager::getInstance()->editorStage->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());
    tableSelected->setCellData(i, 1, (void*)RaceManager::getInstance()->editorStage);

    i = 4;
    tableSelected->addRow(i);
    str = L"road";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (RoadManager::getInstance()->editorRoad)
    {
        str += RoadManager::getInstance()->editorRoad->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());
    tableSelected->setCellData(i, 1, (void*)RoadManager::getInstance()->editorRoad);

    i = 5;
    tableSelected->addRow(i);
    str = L"road type";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (RoadTypeManager::getInstance()->editorRoadType)
    {
        str += RoadTypeManager::getInstance()->editorRoadType->getName().c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());
    tableSelected->setCellData(i, 1, (void*)RoadTypeManager::getInstance()->editorRoadType);

    i = 6;
    tableSelected->addRow(i);
    str = L"itiner image";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (!ItinerManager::getInstance()->editorItinerImageName.empty())
    {
        str += ItinerManager::getInstance()->editorItinerImageName.c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());

    i = 7;
    tableSelected->addRow(i);
    str = L"itiner image 2";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"";
    if (!ItinerManager::getInstance()->editorItinerImageName2.empty())
    {
        str += ItinerManager::getInstance()->editorItinerImageName2.c_str();
    }
    else
    {
        str = L"none";
    }
    tableSelected->setCellText(i, 1, str.c_str());

    i = 8;
    tableSelected->addRow(i);
    str = L"WP type";
    tableSelected->setCellText(i, 0, str.c_str());
    switch (WayPointManager::getInstance()->editorWayPointType)
    {
    case WayPoint::Hidden:
        str = L"hidden - WPM";
        break;
    case WayPoint::Safety:
        str = L"safety - WPS";
        break;
    default:
        str = L"unknown";
        break;
    }
    tableSelected->setCellText(i, 1, str.c_str());

    i = 9;
    tableSelected->addRow(i);
    str = L"stage length";
    tableSelected->setCellText(i, 0, str.c_str());
    str = L"0";
    if (RaceManager::getInstance()->editorStage)
    {
        if (!RaceManager::getInstance()->editorStage->AIPointList.empty())
        {
            str = L"";
            str += (int)RaceManager::getInstance()->editorStage->AIPointList.back()->getGlobalDistance();
            if (!RaceManager::getInstance()->editorStage->itinerPointList.empty())
            {
                str += L", AI-iti: ";
                str += (int)(RaceManager::getInstance()->editorStage->AIPointList.back()->getGlobalDistance()-RaceManager::getInstance()->editorStage->itinerPointList.back()->getGlobalDistance());
            }
        }
    }
    tableSelected->setCellText(i, 1, str.c_str());
}

void MenuPageEditor::refreshAction()
{
    tableAction->setSelected((int)currentAction);
    buttonAction->setText(L"action");
}

void MenuPageEditor::activateAction()
{
    //TheGame::getInstance()->getEnv()->setFocus(buttonAction);
    /*
    irr::SEvent event;
    event.EventType = irr::EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
    event.MouseInput.X = buttonAction->getAbsolutePosition().UpperLeftCorner.X + 1;
    event.MouseInput.Y = buttonAction->getAbsolutePosition().UpperLeftCorner.Y + 1;

    buttonAction->OnEvent(event);
    */
}

void MenuPageEditor::refreshTiles()
{
    // ----------------------------
    // Tiles
    // ----------------------------
    tableTiles->clearRows();

    const TheEarth::tileMap_t& tileMap = TheEarth::getInstance()->getTileMap();
    unsigned int i = 0;
    for (TheEarth::tileMap_t::const_iterator tit = tileMap.begin();
         tit != tileMap.end();
         tit++, i++)
    {
        irr::core::stringw str;
        
        tableTiles->addRow(i);

        str += i;
        tableTiles->setCellText(i, 0, str.c_str());

        str = L"";
        str += tit->second->getPosX();
        tableTiles->setCellText(i, 1, str.c_str());

        str = L"";
        str += tit->second->getPosY();
        tableTiles->setCellText(i, 2, str.c_str());

        str = L"";
        str += tit->second->getInUse() ? L"true" : L"false";
        tableTiles->setCellText(i, 3, str.c_str());

        str = L"";
        str += tit->second->roadRoadChunkList.size();
        tableTiles->setCellText(i, 4, str.c_str());
    }
}

void MenuPageEditor::refreshObjectWire()
{
    // ----------------------------
    // ObjectWire
    // ----------------------------
    tableObjectWire->clearRows();

    const ObjectWire::globalObjectWire_t& globalObjectWire = ObjectWire::getInstance()->globalObjectWire;
    unsigned int i = 0;
    for (ObjectWire::globalObjectWire_t::const_iterator owit = globalObjectWire.begin();
         owit != globalObjectWire.end();
         owit++, i++)
    {
        irr::core::stringw str;
        
        tableObjectWire->addRow(i);

        str += i;
        tableObjectWire->setCellText(i, 0, str.c_str());

        str = L"";
        str += owit->first;
        tableObjectWire->setCellText(i, 1, str.c_str());

        str = L"";
        str += owit->second.size();
        tableObjectWire->setCellText(i, 2, str.c_str());
    }

}

void MenuPageEditor::refreshObjectWireTiles()
{
    // ----------------------------
    // ObjectWireTiles
    // ----------------------------
    tableObjectWireTiles->clearRows();

    unsigned int i = 0;
    const unsigned int objectWireNum = Settings::getInstance()->objectWireNum;
    while (i < objectWireNum*objectWireNum)
    {
        irr::core::stringw str;
        
        tableObjectWireTiles->addRow(i);

        str += i;
        tableObjectWireTiles->setCellText(i, 0, str.c_str());

        str = L"";
        str += ObjectWire::getInstance()->tiles[i]->rpos.X;
        tableObjectWireTiles->setCellText(i, 1, str.c_str());

        str = L"";
        str += ObjectWire::getInstance()->tiles[i]->rpos.Y;
        tableObjectWireTiles->setCellText(i, 2, str.c_str());

        str = L"";
        str += ObjectWire::getInstance()->tiles[i]->objectList.size();
        tableObjectWireTiles->setCellText(i, 3, str.c_str());

        str = L"false";
        if (ObjectWire::getInstance()->tiles[i]->isVisible())
        {
            str = L"true";
        }
        tableObjectWireTiles->setCellText(i, 4, str.c_str());

        i++;
    }

}

void MenuPageEditor::refreshObjectPool()
{
    // ----------------------------
    // ObjectPool
    // ----------------------------
    tableObjectPool->clearRows();

    const ObjectPoolManager::objectPoolMap_t& objectPoolMap = ObjectPoolManager::getInstance()->getObjectPoolMap();
    unsigned int i = 0;
    for (ObjectPoolManager::objectPoolMap_t::const_iterator opit = objectPoolMap.begin();
         opit != objectPoolMap.end();
         opit++, i++)
    {
        irr::core::stringw str;
        
        tableObjectPool->addRow(i);

        str += i;
        tableObjectPool->setCellText(i, 0, str.c_str());
        tableObjectPool->setCellData(i, 0, (void*)opit->second);

        str = L"";
        str += opit->first.c_str();
        tableObjectPool->setCellText(i, 1, str.c_str());

        str = L"";
        str += opit->second->category;
        tableObjectPool->setCellText(i, 2, str.c_str());

        str = L"";
        str += opit->second->num;
        tableObjectPool->setCellText(i, 3, str.c_str());

        str = L"";
        str += opit->second->inUse;
        tableObjectPool->setCellText(i, 4, str.c_str());

        str = L"";
        str += opit->second->objectList.size();
        tableObjectPool->setCellText(i, 5, str.c_str());
    }
}

void MenuPageEditor::refreshRaceManager()
{
    // ----------------------------
    // RaceManager
    // ----------------------------
    tableRaceManager->clearRows();

    const RaceManager::raceMap_t& raceMap = RaceManager::getInstance()->getRaceMap();
    unsigned int i = 0;
    for (RaceManager::raceMap_t::const_iterator rit = raceMap.begin();
         rit != raceMap.end();
         rit++, i++)
    {
        irr::core::stringw str;
        
        tableRaceManager->addRow(i);

        str += i;
        tableRaceManager->setCellText(i, 0, str.c_str());
        tableRaceManager->setCellData(i, 0, (void*)rit->second);

        str = L"";
        str += rit->first.c_str();
        tableRaceManager->setCellText(i, 1, str.c_str());

        str = L"";
        str += rit->second->getLongName().c_str();
        tableRaceManager->setCellText(i, 2, str.c_str());
    }
}

void MenuPageEditor::refreshRoadManager()
{
    // ----------------------------
    // RoadManager
    // ----------------------------
    tableRoadManagerS->clearRows();
    /*
    tableRoadManagerS->addColumn(L"#");
    tableRoadManagerS->addColumn(L"tile");
    tableRoadManagerS->addColumn(L"size");
    */
    const RoadManager::stageRoadChunkListMap_t sMap = RoadManager::getInstance()->stageRoadChunkListMap;
    unsigned int i = 0;
    for (RoadManager::stageRoadChunkListMap_t::const_iterator sit = sMap.begin();
         sit != sMap.end();
         sit++, i++)
    {
        irr::core::stringw str;
        
        tableRoadManagerS->addRow(i);

        str += i;
        tableRoadManagerS->setCellText(i, 0, str.c_str());

        str = L"";
        str += sit->first;
        tableRoadManagerS->setCellText(i, 1, str.c_str());

        str = L"";
        str += sit->second.size();
        tableRoadManagerS->setCellText(i, 2, str.c_str());
    }

    tableRoadManagerV->clearRows();
    /*
    tableRoadManagerV->addColumn(L"#");
    tableRoadManagerV->addColumn(L"road");
    tableRoadManagerV->addColumn(L"begin");
    tableRoadManagerV->addColumn(L"end");
    */
    const roadRoadChunkSet_t cSet = RoadManager::getInstance()->visibleRoadChunkSet;
    i = 0;
    for (roadRoadChunkSet_t::const_iterator cit = cSet.begin();
         cit != cSet.end();
         cit++, i++)
    {
        irr::core::stringw str;
        
        tableRoadManagerV->addRow(i);

        str += i;
        tableRoadManagerV->setCellText(i, 0, str.c_str());

        str = L"";
        str += cit->road->getName().c_str();
        tableRoadManagerV->setCellText(i, 1, str.c_str());

        str = L"";
        str += cit->roadChunk.first;
        tableRoadManagerV->setCellText(i, 2, str.c_str());

        str = L"";
        str += cit->roadChunk.second;
        tableRoadManagerV->setCellText(i, 3, str.c_str());
    }

    tableRoadTypes->clearRows();
    /*
    tableRoadTypes->addColumn(L"#");
    tableRoadTypes->addColumn(L"type");
    */
    const RoadTypeManager::roadTypeMap_t& roadTypeMap = RoadTypeManager::getInstance()->getRoadTypeMap();
    i = 0;
    for (RoadTypeManager::roadTypeMap_t::const_iterator rit = roadTypeMap.begin();
         rit != roadTypeMap.end();
         rit++, i++)
    {
        irr::core::stringw str;
        
        tableRoadTypes->addRow(i);

        str += i;
        tableRoadTypes->setCellText(i, 0, str.c_str());
        tableRoadTypes->setCellData(i, 0, (void*)rit->second);

        str = L"";
        str += rit->second->getName().c_str();
        tableRoadTypes->setCellText(i, 1, str.c_str());
    }

}

void MenuPageEditor::refreshRoads()
{
    /*
    refreshRoadEditBoxes(editBoxNewRoadName->getText());

    tableRoads->clearRows();

    const RoadManager::roadMap_t& roadMap = RoadManager::getInstance()->getRoadMap();
    unsigned int i = 0;
    for (RoadManager::roadMap_t::const_iterator rit = roadMap.begin();
         rit != roadMap.end();
         rit++, i++)
    {
        irr::core::stringw str;
        
        tableRoads->addRow(i);

        str += i;
        tableRoads->setCellText(i, 0, str.c_str());
        tableRoads->setCellData(i, 0, (void*)rit->second);

        str = L"";
        str += rit->second->getName().c_str();
        tableRoads->setCellText(i, 1, str.c_str());

        str = L"";
        str += rit->second->roadType->getName().c_str();
        tableRoads->setCellText(i, 2, str.c_str());

        str = L"";
        str += rit->second->roadPointVector.size();
        tableRoads->setCellText(i, 3, str.c_str());

        str = L"false";
        if (rit->second->isLoaded())
        {
            str = L"true";
        }
        tableRoads->setCellText(i, 4, str.c_str());

        str = L"";
        str += rit->second->roadFilename.c_str();
        tableRoads->setCellText(i, 5, str.c_str());

        str = L"";
        str += rit->second->roadDataFilename.c_str();
        tableRoads->setCellText(i, 6, str.c_str());
    }
    */
}

void MenuPageEditor::refreshRoadEditBoxes(const wchar_t* newRoadName)
{
    irr::core::stringw str;

    str = newRoadName;
    editBoxNewRoadName->setText(str.c_str());

    str = L"data/roads/";
    str += newRoadName;
    editBoxNewRoadFilename->setText(str.c_str());

    str = L"data/roads/data/";
    str += newRoadName;
    editBoxNewRoadDataFilename->setText(str.c_str());

}

void MenuPageEditor::refreshItiner()
{
    tableItiner->clearRows();

    const ItinerManager::itinerImageMap_t& itinerImageMap = ItinerManager::getInstance()->getItinerImageMap();
    unsigned int i = 0;

    irr::core::stringw str;

    tableItiner->addRow(i);
    str = L"none";
    tableItiner->setCellText(i, 0, str.c_str());
    tableItiner->setCellData(i, 0, (void*)0);

    i++;

    for (ItinerManager::itinerImageMap_t::const_iterator it = itinerImageMap.begin();
         it != itinerImageMap.end();
         it++, i++)
    {
        tableItiner->addRow(i);

        str = L"";
        str += it->first.c_str();
        tableItiner->setCellText(i, 0, str.c_str());
        tableItiner->setCellData(i, 0, (void*)it->second);
    }

    tableItiner2->clearRows();

    const ItinerManager::itinerImageMap_t& itinerImageMap2 = ItinerManager::getInstance()->getItinerImageMap2();
    i = 0;

    tableItiner2->addRow(i);
    str = L"none";
    tableItiner2->setCellText(i, 0, str.c_str());
    tableItiner2->setCellData(i, 0, (void*)0);

    i++;

    for (ItinerManager::itinerImageMap_t::const_iterator it = itinerImageMap2.begin();
         it != itinerImageMap2.end();
         it++, i++)
    {
        tableItiner2->addRow(i);

        str = L"";
        str += it->first.c_str();
        tableItiner2->setCellText(i, 0, str.c_str());
        tableItiner2->setCellData(i, 0, (void*)it->second);
    }
}

void MenuPageEditor::refreshItinerGD()
{
    irr::core::stringw str;

    str += ItinerManager::getInstance()->editorGlobalDistance;
    staticTextItinerGD->setText(str.c_str());
}

/* static */ void MenuPageEditor::action()
{
    if (menuPageEditor)
    {
        menuPageEditor->actionP();
    }
}

/* static */ void MenuPageEditor::render()
{
    if (menuPageEditor)
    {
        menuPageEditor->renderP();
    }
}

void MenuPageEditor::actionP()
{
    unsigned int tick = TheGame::getInstance()->getTick();

    if (tick - lastTick < 500) return;

    lastTick = tick;

    irr::core::vector3df pos = TheGame::getInstance()->getCamera()->getPosition();
    pos.Y = TheEarth::getInstance()->getHeight(pos);
    printf("action at (%f, %f, %f)\n", pos.X, pos.Y, pos.Z);
    irr::core::vector3df apos = pos + OffsetManager::getInstance()->getOffset();

    switch (currentAction)
    {
    case A_AddObjectRace:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add object race editorRace: %p, editorPool: %p\n",
                RaceManager::getInstance()->editorRace, ObjectPoolManager::getInstance()->editorPool);
            if (RaceManager::getInstance()->editorRace && ObjectPoolManager::getInstance()->editorPool)
            {
                MessageManager::getInstance()->addText(L"add race object", 1);
                ObjectWireGlobalObject* go = new ObjectWireGlobalObject(ObjectPoolManager::getInstance()->editorPool,
                    apos,
                    irr::core::vector3df(0.f, ObjectPoolManager::getInstance()->editorRot, 0.f),
                    irr::core::vector3df(ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale));
                RaceManager::getInstance()->editorRace->globalObjectList.push_back(go);
                if (RaceManager::getInstance()->editorRace->active)
                {
                    ObjectWire::getInstance()->addGlobalObject(go);
                    //go->setVisible(true);
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add race object, because no race or no object type selected", 3);
            }
            break;
        }
    case A_AddObjectDay:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add object day editorDay: %p, editorPool: %p\n",
                RaceManager::getInstance()->editorDay, ObjectPoolManager::getInstance()->editorPool);
            if (RaceManager::getInstance()->editorDay && ObjectPoolManager::getInstance()->editorPool)
            {
                MessageManager::getInstance()->addText(L"add day object", 1);
                ObjectWireGlobalObject* go = new ObjectWireGlobalObject(ObjectPoolManager::getInstance()->editorPool,
                    apos,
                    irr::core::vector3df(0.f, ObjectPoolManager::getInstance()->editorRot, 0.f),
                    irr::core::vector3df(ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale));
                RaceManager::getInstance()->editorDay->globalObjectList.push_back(go);
                if (RaceManager::getInstance()->editorDay->active)
                {
                    ObjectWire::getInstance()->addGlobalObject(go);
                    //go->setVisible(true);
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add day object, because no day or no object type selected", 3);
            }
            break;
        }
    case A_AddObjectStage:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add object stage editorStage: %p, editorPool: %p\n",
                RaceManager::getInstance()->editorStage, ObjectPoolManager::getInstance()->editorPool);
            if (RaceManager::getInstance()->editorStage && ObjectPoolManager::getInstance()->editorPool)
            {
                MessageManager::getInstance()->addText(L"add stage object", 1);
                ObjectWireGlobalObject* go = new ObjectWireGlobalObject(ObjectPoolManager::getInstance()->editorPool,
                    apos,
                    irr::core::vector3df(0.f, ObjectPoolManager::getInstance()->editorRot, 0.f),
                    irr::core::vector3df(ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale, ObjectPoolManager::getInstance()->editorScale));
                RaceManager::getInstance()->editorStage->globalObjectList.push_back(go);
                if (RaceManager::getInstance()->editorStage->active)
                {
                    ObjectWire::getInstance()->addGlobalObject(go);
                    //go->setVisible(true);
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add stage object, because no stage or no object type selected", 3);
            }
            break;
        }
    case A_AddRoadPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add road point, editorRoad: %p\n", RoadManager::getInstance()->editorRoad);
            if (RoadManager::getInstance()->editorRoad)
            {
                MessageManager::getInstance()->addText(L"add road point", 1);
                RoadManager::getInstance()->editorRoad->addRoadFarPoint(pos);
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add road point, because no road selected", 3);
            }
            break;
        }
    case A_AddRoadPointBegin:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add road point begin, editorRoad: %p\n",
                RoadManager::getInstance()->editorRoad);
            if (RoadManager::getInstance()->editorRoad)
            {
                MessageManager::getInstance()->addText(L"add road point BEGIN", 1);
                RoadManager::getInstance()->editorRoad->addRoadFarPointBegin(pos);
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add road point BEGIN, because no road selected", 3);
            }
            break;
        }
    case A_AddItinerPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add itiner point editorStage: %p, GD: %f, LD: %f, iti: [%s] / [%s], desc: [%s]\n",
                RaceManager::getInstance()->editorStage,
                ItinerManager::getInstance()->editorGlobalDistance,
                ItinerManager::getInstance()->editorLocalDistance,
                ItinerManager::getInstance()->editorItinerImageName.c_str(),
                ItinerManager::getInstance()->editorItinerImageName2.c_str(),
                ItinerManager::getInstance()->editorDescription.c_str());
            if (RaceManager::getInstance()->editorStage)
            {
                MessageManager::getInstance()->addText(L"add itiner point", 1);
                ItinerManager::getInstance()->editorGlobalDistance += ItinerManager::getInstance()->editorLocalDistance;
                refreshItinerGD();
                ItinerPoint* ip = new ItinerPoint(apos,
                    RaceManager::getInstance()->editorStage->itinerPointList.size()+1,
                    ItinerManager::getInstance()->editorGlobalDistance,
                    ItinerManager::getInstance()->editorLocalDistance,
                    ItinerManager::getInstance()->editorItinerImageName,
                    ItinerManager::getInstance()->editorItinerImageName2,
                    ItinerManager::getInstance()->editorDescription);
                RaceManager::getInstance()->editorStage->itinerPointList.push_back(ip);
                refreshSelected();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add itiner point, because no stage selected", 3);
            }
            MenuManager::getInstance()->refreshEventReceiver();
            refreshAction();
            break;
        }
    case A_AddAIPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add AI point editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage)
            {
                MessageManager::getInstance()->addText(L"add AI point", 1);
                float gd = 0.f;
                float ld = 0.f;
                if (!RaceManager::getInstance()->editorStage->AIPointList.empty())
                {
                    irr::core::vector3df lastPos = RaceManager::getInstance()->editorStage->AIPointList.back()->getPos();
                    ld = lastPos.getDistanceFrom(apos);
                    gd = ld + RaceManager::getInstance()->editorStage->AIPointList.back()->getGlobalDistance();
                }
                AIPoint* aip = new AIPoint(apos, gd ,ld);
                RaceManager::getInstance()->editorStage->AIPointList.push_back(aip);
                refreshSelected();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add AI point, because no stage selected", 3);
            }
            break;
        }
    case A_AddWayPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add waypoint editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage)
            {
                float ld = 10000.f;
                if (!RaceManager::getInstance()->editorStage->wayPointList.empty())
                {
                    irr::core::vector3df lastPos = RaceManager::getInstance()->editorStage->wayPointList.back()->getPos();
                    ld = lastPos.getDistanceFrom(apos);
                }
                
                if (ld > 2500.f/*((float)(Settings::getInstance()->objectWireNum*Settings::getInstance()->objectWireSize)+200.f)*/)
                {
                    MessageManager::getInstance()->addText(L"add waypoint", 1);
                    unsigned int num = RaceManager::getInstance()->editorStage->wayPointList.size() + 1;
                    WayPoint* wpip = new WayPoint(apos, num, (WayPoint::Type)WayPointManager::getInstance()->editorWayPointType);
                    RaceManager::getInstance()->editorStage->wayPointList.push_back(wpip);
                }
                else
                {
                    dprintf(MY_DEBUG_INFO, "MenuPageEditor::action(): add waypoint not possible, because last WP is to close: %f < 2500.0\n", ld);
                    MessageManager::getInstance()->addText(L"Unable to add waypoint, because too close", 3);
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add waypoint, because no stage selected", 3);
            }
            MenuManager::getInstance()->refreshEventReceiver();
            refreshAction();
            break;
        }
    case A_AddHeightModifier:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add height modifier editorStage: %p, height: %f\n",
                RaceManager::getInstance()->editorStage, RaceManager::getInstance()->editorStage?RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y:0.0f);
            if (RaceManager::getInstance()->editorStage && 
                (RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y > 0.01f || RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y < -0.01f)
                )
            {
                MessageManager::getInstance()->addText(L"add height modifier", 1);
                RaceManager::getInstance()->editorStage->editorHeightModifier.pos.X = apos.X;
                RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Z = apos.Z;
                RaceManager::getInstance()->editorStage->heightModifierList.push_back(RaceManager::getInstance()->editorStage->editorHeightModifier);
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add height modifier, because \n\nno stage selected or \nHM value to small", 3);
            }
            break;
        }
    case A_AddHeightModifierLine:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add height modifier line editorStage: %p, height: %f\n",
                RaceManager::getInstance()->editorStage, RaceManager::getInstance()->editorStage?RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y:0.0f);
            if (RaceManager::getInstance()->editorStage &&
                (RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y > 0.01f || RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y < -0.01f) &&
                !RaceManager::getInstance()->editorStage->heightModifierList.empty())
            {
                MessageManager::getInstance()->addText(L"add height modifier line", 1);
                irr::core::vector2df pos2d = irr::core::vector2df(apos.X, apos.Z);
                irr::core::vector2df bp = irr::core::vector2df(RaceManager::getInstance()->editorStage->heightModifierList.back().pos.X,
                    (float)(RaceManager::getInstance()->editorStage->heightModifierList.back().pos.Z));
                irr::core::vector2df dir = pos2d - bp;
                irr::core::vector2df tmpp;
                irr::core::vector2di lastDetailPos;
                irr::core::vector2di currentDetailPos;
                const float dist = dir.getLength();
                float cur = 0.f;
                //printf("start draw a line, dist: %f, step: %f\n", dist, TILE_DETAIL_SCALE_F);
                while (cur + TILE_DETAIL_SCALE_F < dist && dist < 16000.f)
                {
                    cur += TILE_DETAIL_SCALE_F;
                    tmpp = bp + dir*(cur/dist);
                    currentDetailPos.X = (int)(tmpp.X / TILE_DETAIL_SCALE_F);
                    currentDetailPos.Y = (int)(tmpp.Y / TILE_DETAIL_SCALE_F);
                    //printf("try line point: cur: %f, last: (%d, %d), current: (%d, %d)\n", cur, lastDetailPos.X, lastDetailPos.Y, currentDetailPos.X, currentDetailPos.Y);
                    
                    if (lastDetailPos != currentDetailPos)
                    {
                        //printf("add line point: point: (%f, %f)\n", tmpp.X, tmpp.Y);
                        RaceManager::getInstance()->editorStage->editorHeightModifier.pos.X = tmpp.X;
                        RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Z = tmpp.Y;
                        RaceManager::getInstance()->editorStage->heightModifierList.push_back(RaceManager::getInstance()->editorStage->editorHeightModifier);
                        lastDetailPos = currentDetailPos;
                    }
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add height modifier line, because \n\nno stage selected or \nHM value to small or \nno previous point in the list", 3);
            }
            MenuManager::getInstance()->refreshEventReceiver();
            refreshAction();
            break;
        }
    case A_AddHeightModifierSquare:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): add height modifier line editorStage: %p, height: %f\n",
                RaceManager::getInstance()->editorStage, RaceManager::getInstance()->editorStage?RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y:0.0f);
            if (RaceManager::getInstance()->editorStage &&
                (RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y > 0.01f || RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y < -0.01f) &&
                !RaceManager::getInstance()->editorStage->heightModifierList.empty())
            {
                MessageManager::getInstance()->addText(L"add height modifier square", 1);
                irr::core::vector2df bp = irr::core::vector2df(RaceManager::getInstance()->editorStage->heightModifierList.back().pos.X,
                    (float)(RaceManager::getInstance()->editorStage->heightModifierList.back().pos.Z));
                bool first = true;
                float distX = fabsf(apos.X - bp.X);
                float distY = fabsf(apos.Z - bp.Y);
                irr::core::vector2di lastDetailPos;
                irr::core::vector2di currentDetailPos;
                irr::core::vector2df min;
                irr::core::vector2df max;
                if (bp.X < apos.X)
                {
                    min.X = bp.X;
                    max.X = apos.X;
                }
                else
                {
                    min.X = apos.X;
                    max.X = bp.X;
                }
                if (bp.Y < apos.Z)
                {
                    min.Y = bp.Y;
                    max.Y = apos.Z;
                }
                else
                {
                    min.Y = apos.Z;
                    max.Y = bp.Y;
                }
                while (min.X < max.X && distX < 2048.f && distY < 2048.f)
                {
                    irr::core::vector2df pos2d = min;
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        pos2d.Y -= TILE_DETAIL_SCALE_F;
                    }
                    while (pos2d.Y + TILE_DETAIL_SCALE_F < max.Y)
                    {
                        pos2d.Y += TILE_DETAIL_SCALE_F;
                        currentDetailPos.X = (int)(pos2d.X / TILE_DETAIL_SCALE_F);
                        currentDetailPos.Y = (int)(pos2d.Y / TILE_DETAIL_SCALE_F);
                        
                        if (lastDetailPos != currentDetailPos)
                        {
                            RaceManager::getInstance()->editorStage->editorHeightModifier.pos.X = pos2d.X;
                            RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Z = pos2d.Y;
                            RaceManager::getInstance()->editorStage->heightModifierList.push_back(RaceManager::getInstance()->editorStage->editorHeightModifier);
                            lastDetailPos = currentDetailPos;
                        }
                    }
                    
                    lastDetailPos.X = (int)(min.X / TILE_DETAIL_SCALE_F);
                    lastDetailPos.Y = (int)(min.Y / TILE_DETAIL_SCALE_F);
                    min.X += TILE_DETAIL_SCALE_F;
                }
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to add height modifier square, because \n\nno stage selected or \nHM value to small or \nno previous point in the list", 3);
            }
            MenuManager::getInstance()->refreshEventReceiver();
            refreshAction();
            break;
        }
    case A_RemoveObjectRace:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove object race editorRace: %p\n",
                RaceManager::getInstance()->editorRace);
            if (RaceManager::getInstance()->editorRace && !RaceManager::getInstance()->editorRace->globalObjectList.empty())
            {
                MessageManager::getInstance()->addText(L"remove race object", 1);
                ObjectWireGlobalObject* go = RaceManager::getInstance()->editorRace->globalObjectList.back();
                RaceManager::getInstance()->editorRace->globalObjectList.pop_back();
                if (RaceManager::getInstance()->editorRace->active)
                {
                    ObjectWire::getInstance()->removeGlobalObject(go, false);
                }
                delete go;
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove race object, because no race selected or object list is empty", 3);
            }
            break;
        }
    case A_RemoveObjectDay:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove object day editorDay: %p\n",
                RaceManager::getInstance()->editorDay);
            if (RaceManager::getInstance()->editorDay && !RaceManager::getInstance()->editorDay->globalObjectList.empty())
            {
                MessageManager::getInstance()->addText(L"remove day object", 1);
                ObjectWireGlobalObject* go = RaceManager::getInstance()->editorDay->globalObjectList.back();
                RaceManager::getInstance()->editorDay->globalObjectList.pop_back();
                if (RaceManager::getInstance()->editorDay->active)
                {
                    ObjectWire::getInstance()->removeGlobalObject(go, false);
                }
                delete go;
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove day object, because no day selected or object list is empty", 3);
            }
            break;
        }
    case A_RemoveObjectStage:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove object stage editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage && !RaceManager::getInstance()->editorStage->globalObjectList.empty())
            {
                MessageManager::getInstance()->addText(L"remove stage object", 1);
                ObjectWireGlobalObject* go = RaceManager::getInstance()->editorStage->globalObjectList.back();
                RaceManager::getInstance()->editorStage->globalObjectList.pop_back();
                if (RaceManager::getInstance()->editorStage->active)
                {
                    ObjectWire::getInstance()->removeGlobalObject(go, false);
                }
                delete go;
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove stage object, because no stage selected or object list is empty", 3);
            }
            break;
        }
    case A_RemoveRoadPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove road point, editorRoad: %p\n", RoadManager::getInstance()->editorRoad);
            if (RoadManager::getInstance()->editorRoad)
            {
                MessageManager::getInstance()->addText(L"remove road point", 1);
                RoadManager::getInstance()->editorRoad->removeRoadPoint();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove road point, because no road selected", 3);
            }
            break;
        }
    case A_RemoveRoadPointBegin:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove road point begin, editorRoad: %p\n", RoadManager::getInstance()->editorRoad);
            if (RoadManager::getInstance()->editorRoad)
            {
                MessageManager::getInstance()->addText(L"remove road point BEGIN", 1);
                RoadManager::getInstance()->editorRoad->removeRoadPointBegin();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove road point BEGIN, because no road selected", 3);
            }
            break;
        }
    case A_RemoveItinerPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove itiner point editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage && !RaceManager::getInstance()->editorStage->itinerPointList.empty())
            {
                MessageManager::getInstance()->addText(L"remove itiner point", 1);
                ItinerPoint* ip = RaceManager::getInstance()->editorStage->itinerPointList.back();
                ItinerManager::getInstance()->editorGlobalDistance -= ip->getLocalDistance();
                if (ItinerManager::getInstance()->editorGlobalDistance < 0.f) ItinerManager::getInstance()->editorGlobalDistance = 0.f;
                refreshItinerGD();
                RaceManager::getInstance()->editorStage->itinerPointList.pop_back();
                if (RaceManager::getInstance()->editorStage->active)
                {
                    ObjectWire::getInstance()->removeGlobalObject(ip, false);
                }
                delete ip;
                refreshSelected();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove itiner point, because no stage selected or itiner point list is empty", 3);
            }
            break;
        }
    case A_RemoveAIPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove AI point editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage && !RaceManager::getInstance()->editorStage->AIPointList.empty())
            {
                MessageManager::getInstance()->addText(L"remove AI point", 1);
                AIPoint* aip = RaceManager::getInstance()->editorStage->AIPointList.back();
                RaceManager::getInstance()->editorStage->AIPointList.pop_back();
                if (RaceManager::getInstance()->editorStage->active)
                {
                    ObjectWire::getInstance()->removeGlobalObject(aip, false);
                }
                delete aip;
                refreshSelected();
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove AI point, because no stage selected or AI point list is empty", 3);
            }
            break;
        }
    case A_RemoveWayPoint:
        {
            dprintf(MY_DEBUG_NOTE, "MenuPageEditor::action(): remove waypoint editorStage: %p\n",
                RaceManager::getInstance()->editorStage);
            if (RaceManager::getInstance()->editorStage && !RaceManager::getInstance()->editorStage->wayPointList.empty())
            {
                MessageManager::getInstance()->addText(L"remove waypoint", 1);
                WayPoint* wpip = RaceManager::getInstance()->editorStage->wayPointList.back();
                RaceManager::getInstance()->editorStage->wayPointList.pop_back();
                delete wpip;
            }
            else
            {
                MessageManager::getInstance()->addText(L"Unable to remove waypoint, because no stage selected or waypoint list is empty", 3);
            }
            break;
        }
    default:
        dprintf(MY_DEBUG_ERROR, "MenuPageEditor::action(): no current action: %d\n", (int)currentAction);
        MessageManager::getInstance()->addText(L"no current action", 2);
    }
}

void MenuPageEditor::renderP()
{
    if (/*!checkBoxRender->isChecked()*/!doRender) return;

    TheGame::getInstance()->getDriver()->setMaterial(material);
    TheGame::getInstance()->getDriver()->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

    if (RaceManager::getInstance()->editorRace)
    {
        RaceManager::editorRenderObjects(RaceManager::getInstance()->editorRace->globalObjectList);
        if (renderAllRoads)
        {
            RoadManager::editorRenderRoads(RaceManager::getInstance()->editorRace->roadMap);
        }
    }
    if (RaceManager::getInstance()->editorDay)
    {
        RaceManager::editorRenderObjects(RaceManager::getInstance()->editorDay->globalObjectList);
        if (renderAllRoads)
        {
            RoadManager::editorRenderRoads(RaceManager::getInstance()->editorDay->roadMap);
        }
    }
    if (RaceManager::getInstance()->editorStage)
    {
        RaceManager::editorRenderObjects(RaceManager::getInstance()->editorStage->globalObjectList);
        ItinerManager::editorRenderItinerPointList(RaceManager::getInstance()->editorStage->itinerPointList);
        AIPoint::editorRenderAIPointList(RaceManager::getInstance()->editorStage->AIPointList);
        WayPointManager::editorRenderWayPointList(RaceManager::getInstance()->editorStage->wayPointList);
        if (renderAllRoads)
        {
            RoadManager::editorRenderRoads(RaceManager::getInstance()->editorStage->roadMap);
        }
    }
    if (RoadManager::getInstance()->editorRoad)
    {
        RoadManager::getInstance()->editorRoad->editorRender(true);
    }
}
