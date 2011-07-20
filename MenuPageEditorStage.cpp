
#include "MenuPageEditorStage.h"
#include "MenuPageEditor.h"
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
#include "RoadTypeManager.h"
#include "Road.h"
#include "Day.h"
#include "Stage.h"
#include "RoadType.h"
#include "ObjectWireGlobalObject.h"
#include "Competitor.h"
#include "ItinerManager.h"
#include "ItinerPoint.h"
#include "AIPoint.h"
#include "WayPointManager.h"
#include "WayPoint.h"


MenuPageEditorStage::MenuPageEditorStage()
    : window(0),
      tableGlobalObjects(0),
      tableRoads(0),
      tableItiner(0),
      tableAI(0),
      tableWP(0),
      tableHM(0),
      editBoxLongName(0),
      editBoxShortDescription(0),
      editBoxNewRoadFilename(0),
      editBoxNewRoadName(0),
      editBoxNewRoadDataFilename(0),
      editBoxStageTime(0),
      editBoxImage(0),
      editBoxHMHeight(0),
      editBoxHMRadius(0)
{
    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width-350, 50, TheGame::getInstance()->getScreenSize().Width-10, TheGame::getInstance()->getScreenSize().Height-150),
        false,
        L"Editor - Stage",
        0,
        MI_WINDOW);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(2,22,42,42),
        window,
        MI_BUTTONREFRESH,
        L"Refresh");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(44,22,84,42),
        window,
        MI_BUTTONSAVE,
        L"Save");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(86,22,126,42),
        window,
        MI_BUTTONCREATEROAD,
        L"new road");

    editBoxLongName = TheGame::getInstance()->getEnv()->addEditBox(L"long name",
        irr::core::recti(irr::core::position2di(2, 44), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, 20)),
        true,
        window,
        MI_EBLONGNAME);

    editBoxShortDescription = TheGame::getInstance()->getEnv()->addEditBox(L"short description",
        irr::core::recti(irr::core::position2di(2, 66), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, 20)),
        true,
        window,
        MI_EBSHORTDESCRIPTION);

    editBoxStageTime = TheGame::getInstance()->getEnv()->addEditBox(L"1000",
        irr::core::recti(irr::core::position2di(2, 88), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, 20)),
        true,
        window,
        MI_EBSTAGETIME);

    editBoxImage = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(2, 110), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, 20)),
        true,
        window,
        MI_EBIMAGE);

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 132), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 134)),
        window,
        true,
        true,
        MI_TABCONTROL);

    // ----------------------------
    // GlobalObject tab
    // ----------------------------
    irr::gui::IGUITab* tabGlobalObjects = tc->addTab(L"GO", MI_TABGLOBALOBJECTS);

    tableGlobalObjects = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabGlobalObjects->getRelativePosition().getSize()),
        tabGlobalObjects,
        MI_TABLEGLOBALOBJECTS,
        true);

    tableGlobalObjects->addColumn(L"#");
    tableGlobalObjects->addColumn(L"object");
    tableGlobalObjects->setColumnWidth(1, 80);
    tableGlobalObjects->addColumn(L"X");
    tableGlobalObjects->addColumn(L"Y");
    tableGlobalObjects->addColumn(L"visible");

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

    // ----------------------------
    // Itiner tab
    // ----------------------------
    irr::gui::IGUITab* tabItiner = tc->addTab(L"Iti", MI_TABITINER);

    tableItiner = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabItiner->getRelativePosition().getSize()),
        tabItiner,
        MI_TABLEITINER,
        true);

    tableItiner->addColumn(L"#");
    tableItiner->addColumn(L"image");
    tableItiner->setColumnWidth(1, 100);
    tableItiner->addColumn(L"X");
    tableItiner->addColumn(L"Y");
    tableItiner->addColumn(L"GD");
    tableItiner->setColumnWidth(4, 60);
    tableItiner->addColumn(L"LD");
    tableItiner->setColumnWidth(5, 60);
    tableItiner->addColumn(L"description");
    tableItiner->setColumnWidth(6, 100);

    // ----------------------------
    // AI tab
    // ----------------------------
    irr::gui::IGUITab* tabAI = tc->addTab(L"AI", MI_TABAI);

    tableAI = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabAI->getRelativePosition().getSize()),
        tabAI,
        MI_TABLEAI,
        true);

    tableAI->addColumn(L"#");
    tableAI->addColumn(L"X");
    tableAI->addColumn(L"Y");
    tableAI->addColumn(L"GD");
    tableAI->setColumnWidth(3, 60);
    tableAI->addColumn(L"LD");
    tableAI->setColumnWidth(4, 60);

    // ----------------------------
    // WP tab
    // ----------------------------
    irr::gui::IGUITab* tabWP = tc->addTab(L"WP", MI_TABWP);

    tableWP = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabWP->getRelativePosition().getSize()),
        tabWP,
        MI_TABLEWP,
        true);

    tableWP->addColumn(L"#");
    tableWP->addColumn(L"X");
    tableWP->addColumn(L"Y");

    // ----------------------------
    // HeightManager tab
    // ----------------------------
    irr::gui::IGUITab* tabHM = tc->addTab(L"HM", MI_TABHM);

    editBoxHMHeight = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabHM->getRelativePosition().getSize().Width/3-2, 20)),
        true,
        tabHM,
        MI_EBHMHEIGHT);

/*    editBoxHMRadius = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(tabHM->getRelativePosition().getSize().Width/3, 0), irr::core::dimension2di(tabHM->getRelativePosition().getSize().Width/3-2, 20)),
        true,
        tabHM,
        MI_EBHMRADIUS);
*/
    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(irr::core::position2di(tabHM->getRelativePosition().getSize().Width*2/3, 0), irr::core::dimension2di(tabHM->getRelativePosition().getSize().Width/3-2, 20)),
        tabHM,
        MI_BUTTONHMSET, L"set");

    tableHM = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(tabHM->getRelativePosition().getSize().Width, tabHM->getRelativePosition().getSize().Height-22)),
        //irr::core::recti(irr::core::position2di(0, 0), tabRoads->getRelativePosition().getSize()),
        tabHM,
        MI_TABLEHM,
        true);

    tableHM->addColumn(L"#");
    tableHM->addColumn(L"X");
    tableHM->setColumnWidth(1, 70);
    tableHM->addColumn(L"Y");
    tableHM->setColumnWidth(2, 70);
    tableHM->addColumn(L"Z");
    tableHM->setColumnWidth(3, 70);
    tableHM->addColumn(L"radius");
    tableHM->setColumnWidth(4, 30);

    window->setVisible(false);
}

MenuPageEditorStage::~MenuPageEditorStage()
{
    //window->remove();
    close();
}

bool MenuPageEditorStage::OnEvent(const irr::SEvent &event)
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
                    case MI_BUTTONREFRESH:
                        //dprintf(MY_DEBUG_NOTE, "editor::refreshbutton clicked\n");
                        refresh();
                        return true;
                        break;
                    case MI_BUTTONSAVE:
                        dprintf(MY_DEBUG_NOTE, "editor::stage::save\n");
                        WStringConverter::toString(editBoxLongName->getText(), RaceManager::getInstance()->editorStage->stageLongName);
                        WStringConverter::toString(editBoxShortDescription->getText(), RaceManager::getInstance()->editorStage->shortDescription);
                        WStringConverter::toUnsignedInt(editBoxStageTime->getText(), RaceManager::getInstance()->editorStage->stageTime);
                        WStringConverter::toString(editBoxImage->getText(), RaceManager::getInstance()->editorStage->imageName);
                        RaceManager::getInstance()->editorStage->write();
                        return true;
                        break;
                    case MI_BUTTONCREATEROAD:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::race::newRoad\n");
                        std::string roadName;
                        std::string roadFilename;
                        std::string roadDataFilename;
                        WStringConverter::toString(editBoxNewRoadFilename->getText(), roadFilename);
                        WStringConverter::toString(editBoxNewRoadName->getText(), roadName);
                        WStringConverter::toString(editBoxNewRoadDataFilename->getText(), roadDataFilename);
                        RoadManager::roadMap_t::const_iterator rit = RaceManager::getInstance()->editorStage->roadMap.find(roadName);
                        if (rit == RaceManager::getInstance()->editorStage->roadMap.end() &&
                            !roadName.empty() &&
                            RoadTypeManager::getInstance()->editorRoadType)
                        {
                            bool ret;
                            ret = ConfigDirectory::mkdir(STAGE_ROADS(RaceManager::getInstance()->editorRace->getName(),RaceManager::getInstance()->editorDay->getName(),RaceManager::getInstance()->editorStage->getName())) &&
                                ConfigDirectory::mkdir(STAGE_ROADS_DATA(RaceManager::getInstance()->editorRace->getName(),RaceManager::getInstance()->editorDay->getName(),RaceManager::getInstance()->editorStage->getName()));
                            if (ret)
                            {
                                RaceManager::getInstance()->editorStage->roadMap[roadName] =
                                    new Road(roadFilename, roadName, roadDataFilename, RoadTypeManager::getInstance()->editorRoadType, false);
                                refreshRoads();
                            }
                        }
                        return true;
                        break;
                    }
                    case MI_BUTTONHMSET:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::stage::hmset\n");
                        float height;
                        float radius;
                        WStringConverter::toFloat(editBoxHMHeight->getText(), height);
                        WStringConverter::toFloat(editBoxHMRadius->getText(), radius);
                        int w = tableHM->getSelected();
                        int i = 0;
                        for (heightModifierList_t::iterator it = RaceManager::getInstance()->editorStage->heightModifierList.begin();
                             it != RaceManager::getInstance()->editorStage->heightModifierList.end();
                             it++, i++)
                        {
                            if (i == w)
                            {
                                it->pos.Y = height;
                                it->radius = radius;
                                refreshHM();
                                break;
                            }
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
                    case MI_TABLEROADS:
                        RoadManager::getInstance()->editorRoad = (Road*)tableRoads->getCellData(tableRoads->getSelected(), 0);
                        if (RoadManager::getInstance()->editorRoad && !RoadManager::getInstance()->editorRoad->getRoadPointVector().empty())
                        {
                            RoadManager::getInstance()->editorColor = RoadManager::getInstance()->editorRoad->getRoadPointVector().back().color;
                            RoadManager::getInstance()->editorRadius = RoadManager::getInstance()->editorRoad->getRoadPointVector().back().radius;
                        }
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_TABLE_SELECTED_AGAIN:
            {
                switch (id)
                {
                    case MI_TABLEROADS:
                        RoadManager::getInstance()->editorRoad = (Road*)tableRoads->getCellData(tableRoads->getSelected(), 0);
                        if (RoadManager::getInstance()->editorRoad && !RoadManager::getInstance()->editorRoad->getRoadPointVector().empty())
                        {
                            RoadManager::getInstance()->editorColor = RoadManager::getInstance()->editorRoad->getRoadPointVector().back().color;
                            RoadManager::getInstance()->editorRadius = RoadManager::getInstance()->editorRoad->getRoadPointVector().back().radius;
                        }
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORROAD);
                        return true;
                        break;
                };
                break;
            }
            case irr::gui::EGET_EDITBOX_CHANGED:
            case irr::gui::EGET_EDITBOX_ENTER:
            {
                switch (id)
                {
                    case MI_EBNEWROADNAME:
                        refreshRoadEditBoxes(editBoxNewRoadName->getText());
                        break;
                    case MI_EBHMHEIGHT:
                        WStringConverter::toFloat(editBoxHMHeight->getText(), RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y);
                        dprintf(MY_DEBUG_NOTE, "editor::stage::hm, set to %f\n", RaceManager::getInstance()->editorStage->editorHeightModifier.pos.Y);
                        break;
                    case MI_EBHMRADIUS:
                        WStringConverter::toFloat(editBoxHMRadius->getText(), RaceManager::getInstance()->editorStage->editorHeightModifier.radius);
                        break;
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageEditorStage::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorStage::open()\n");
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageEditorStage::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorStage::close()\n");
    window->setVisible(false);
}

void MenuPageEditorStage::refresh()
{
    refreshGlobalObjects();
    refreshEditBoxes();
    refreshRoads();
    refreshItiner();
    refreshAI();
    refreshWP();
    refreshHM();
}

void MenuPageEditorStage::refreshGlobalObjects()
{
    // ----------------------------
    // GlobalObjects
    // ----------------------------
    tableGlobalObjects->clearRows();
    /*
    tableGlobalObjects->addColumn(L"#");
    tableGlobalObjects->addColumn(L"object");
    tableGlobalObjects->addColumn(L"X");
    tableGlobalObjects->addColumn(L"Y");
    tableGlobalObjects->addColumn(L"visible");
    */
    const RaceManager::globalObjectList_t& globalObjectList = RaceManager::getInstance()->editorStage->globalObjectList;
    unsigned int i = 0;
    for (RaceManager::globalObjectList_t::const_iterator goit = globalObjectList.begin();
         goit != globalObjectList.end();
         goit++, i++)
    {
        irr::core::stringw str;
        
        tableGlobalObjects->addRow(i);

        str += i;
        tableGlobalObjects->setCellText(i, 0, str.c_str());

        str = L"";
        str += (*goit)->getObjectPool()->getName().c_str();
        tableGlobalObjects->setCellText(i, 1, str.c_str());

        str = L"";
        str += (*goit)->getPos().X;
        tableGlobalObjects->setCellText(i, 2, str.c_str());

        str = L"";
        str += (*goit)->getPos().Z;
        tableGlobalObjects->setCellText(i, 3, str.c_str());

        str = L"false";
        if ((*goit)->getVisible()) str = L"true";
        tableGlobalObjects->setCellText(i, 4, str.c_str());

    }
}

void MenuPageEditorStage::refreshEditBoxes()
{
    irr::core::stringw str;

    str += RaceManager::getInstance()->editorStage->getLongName().c_str();
    editBoxLongName->setText(str.c_str());

    str = L"";
    str += RaceManager::getInstance()->editorStage->getShortDescription().c_str();
    editBoxShortDescription->setText(str.c_str());

    str = L"";
    str += RaceManager::getInstance()->editorStage->getStageTime();
    editBoxStageTime->setText(str.c_str());

    str = L"";
    str += RaceManager::getInstance()->editorStage->imageName.c_str();
    editBoxImage->setText(str.c_str());
}

void MenuPageEditorStage::refreshRoads()
{
    refreshRoadEditBoxes(editBoxNewRoadName->getText());

    tableRoads->clearRows();
    /*
    tableRoads->addColumn(L"#");
    tableRoads->addColumn(L"name");
    tableRoads->addColumn(L"type");
    tableRoads->addColumn(L"loaded");
    tableRoads->addColumn(L"filename");
    tableRoads->addColumn(L"data");
    */
    const RoadManager::roadMap_t& roadMap = RaceManager::getInstance()->editorStage->getRoadMap();
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

}

void MenuPageEditorStage::refreshRoadEditBoxes(const wchar_t* newRoadName)
{
    irr::core::stringw str;

    str = newRoadName;
    editBoxNewRoadName->setText(str.c_str());

    str = L"";
    str += (STAGE_ROADS(RaceManager::getInstance()->editorRace->getName(),
        RaceManager::getInstance()->editorDay->getName(),
        RaceManager::getInstance()->editorStage->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadFilename->setText(str.c_str());

    str = L"";
    str += (STAGE_ROADS_DATA(RaceManager::getInstance()->editorRace->getName(),
        RaceManager::getInstance()->editorDay->getName(),
        RaceManager::getInstance()->editorStage->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadDataFilename->setText(str.c_str());

}

void MenuPageEditorStage::refreshItiner()
{
    // ----------------------------
    // Itiner
    // ----------------------------
    tableItiner->clearRows();

    const ItinerManager::itinerPointList_t& itinerPointList = RaceManager::getInstance()->editorStage->itinerPointList;
    unsigned int i = 0;
    for (ItinerManager::itinerPointList_t::const_iterator ipit = itinerPointList.begin();
         ipit != itinerPointList.end();
         ipit++, i++)
    {
        irr::core::stringw str;
        
        tableItiner->addRow(i);

        str += i;
        tableItiner->setCellText(i, 0, str.c_str());

        str = L"";
        str += (*ipit)->getItinerImageName().c_str();
        tableItiner->setCellText(i, 1, str.c_str());

        str = L"";
        str += (*ipit)->getPos().X;
        tableItiner->setCellText(i, 2, str.c_str());

        str = L"";
        str += (*ipit)->getPos().Z;
        tableItiner->setCellText(i, 3, str.c_str());

        str = L"";
        str += (*ipit)->getGlobalDistance();
        tableItiner->setCellText(i, 4, str.c_str());

        str = L"";
        str += (*ipit)->getLocalDistance();
        tableItiner->setCellText(i, 5, str.c_str());

        str = L"";
        str += (*ipit)->getDescription().c_str();
        tableItiner->setCellText(i, 6, str.c_str());
    }
}

void MenuPageEditorStage::refreshAI()
{
    // ----------------------------
    // AI
    // ----------------------------
    tableAI->clearRows();

    const AIPoint::AIPointList_t& AIPointList = RaceManager::getInstance()->editorStage->AIPointList;
    unsigned int i = 0;
    for (AIPoint::AIPointList_t::const_iterator aiit = AIPointList.begin();
         aiit != AIPointList.end();
         aiit++, i++)
    {
        irr::core::stringw str;
        
        tableAI->addRow(i);

        str += i;
        tableAI->setCellText(i, 0, str.c_str());

        str = L"";
        str += (*aiit)->getPos().X;
        tableAI->setCellText(i, 1, str.c_str());

        str = L"";
        str += (*aiit)->getPos().Z;
        tableAI->setCellText(i, 2, str.c_str());

        str = L"";
        str += (*aiit)->getGlobalDistance();
        tableAI->setCellText(i, 3, str.c_str());

        str = L"";
        str += (*aiit)->getLocalDistance();
        tableAI->setCellText(i, 4, str.c_str());
    }
}

void MenuPageEditorStage::refreshWP()
{
    // ----------------------------
    // WP
    // ----------------------------
    tableWP->clearRows();

    const WayPointManager::wayPointList_t& wayPointList = RaceManager::getInstance()->editorStage->wayPointList;
    unsigned int i = 0;
    for (WayPointManager::wayPointList_t::const_iterator wpit = wayPointList.begin();
         wpit != wayPointList.end();
         wpit++, i++)
    {
        irr::core::stringw str;
        
        tableWP->addRow(i);

        str += i;
        tableWP->setCellText(i, 0, str.c_str());

        str = L"";
        str += (*wpit)->getPos().X;
        tableWP->setCellText(i, 1, str.c_str());

        str = L"";
        str += (*wpit)->getPos().Z;
        tableWP->setCellText(i, 2, str.c_str());
    }
}

void MenuPageEditorStage::refreshHM()
{
    // ----------------------------
    // HM
    // ----------------------------
    tableHM->clearRows();

    const heightModifierList_t& hmList = RaceManager::getInstance()->editorStage->heightModifierList;
    unsigned int i = 0;
    for (heightModifierList_t::const_iterator it = hmList.begin();
         it != hmList.end();
         it++, i++)
    {
        irr::core::stringw str;
        
        tableHM->addRow(i);

        str += i;
        tableHM->setCellText(i, 0, str.c_str());

        str = L"";
        str += it->pos.X;
        tableHM->setCellText(i, 1, str.c_str());

        str = L"";
        str += it->pos.Y;
        tableHM->setCellText(i, 2, str.c_str());

        str = L"";
        str += it->pos.Z;
        tableHM->setCellText(i, 3, str.c_str());

        str = L"";
        str += it->radius;
        tableHM->setCellText(i, 4, str.c_str());
    }
}
