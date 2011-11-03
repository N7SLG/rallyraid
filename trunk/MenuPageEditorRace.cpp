
#include "MenuPageEditorRace.h"
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

MenuPageEditorRace::MenuPageEditorRace()
    : window(0),
      tableDays(0),
      tableCompetitors(0),
      tableGlobalObjects(0),
      tableRoads(0),
      editBoxLongName(0),
      editBoxShortDescription(0),
      editBoxNewDay(0),
      editBoxNewRoadFilename(0),
      editBoxNewRoadName(0),
      editBoxNewRoadDataFilename(0)
{
    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width-350, 50, TheGame::getInstance()->getScreenSize().Width-10, TheGame::getInstance()->getScreenSize().Height-150),
        false,
        L"Editor - Race",
        0,
        MI_WINDOW);

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(2,22,42,42),
        window,
        MI_BUTTONREFRESH,
        L"Refresh", L"Refresh the content of this window.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(44,22,84,42),
        window,
        MI_BUTTONSAVE,
        L"Save", L"Save the changes.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(86,22,126,42),
        window,
        MI_BUTTONCREATEDAY,
        L"new day", L"Create new day.");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(128,22,168,42),
        window,
        MI_BUTTONCREATEROAD,
        L"new road", L"Create new road.");

    TheGame::getInstance()->getEnv()->addStaticText(L"Long name",
        irr::core::recti(irr::core::position2di(2, 44), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        window)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxLongName = TheGame::getInstance()->getEnv()->addEditBox(L"Name of the Race",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH+2, 44), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4 - EXP_TEXT_WIDTH, 20)),
        true,
        window,
        MI_EBLONGNAME);

    TheGame::getInstance()->getEnv()->addStaticText(L"Short desc.",
        irr::core::recti(irr::core::position2di(2, 66), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        window)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxShortDescription = TheGame::getInstance()->getEnv()->addEditBox(L"",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH+2, 66), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4 - EXP_TEXT_WIDTH, 20)),
        true,
        window,
        MI_EBSHORTDESCRIPTION);

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 88), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 90)),
        window,
        true,
        true,
        MI_TABCONTROL);

    // ----------------------------
    // Days tab
    // ----------------------------
    irr::gui::IGUITab* tabDays = tc->addTab(L"Days", MI_TABDAYS);

    TheGame::getInstance()->getEnv()->addStaticText(L"Day id",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabDays)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxNewDay = TheGame::getInstance()->getEnv()->addEditBox(L"dayxxx",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 0), irr::core::dimension2di(tabDays->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabDays,
        MI_EBNEWDAYNAME);


    tableDays = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(tabDays->getRelativePosition().getSize().Width, tabDays->getRelativePosition().getSize().Height-22)),
        tabDays,
        MI_TABLEDAYS,
        true);

    tableDays->addColumn(L"#");
    tableDays->addColumn(L"name");
    tableDays->setColumnWidth(1, 80);
    tableDays->addColumn(L"long name");
    tableDays->setColumnWidth(2, 180);

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
    // Competitors tab
    // ----------------------------
    irr::gui::IGUITab* tabCompetitors = tc->addTab(L"Comp", MI_TABCOMPETITORS);

    tableCompetitors = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 0), tabGlobalObjects->getRelativePosition().getSize()),
        tabCompetitors,
        MI_TABLECOMPETITORS,
        true);

    tableCompetitors->addColumn(L"#");
    tableCompetitors->addColumn(L"num");
    tableCompetitors->addColumn(L"name");
    tableCompetitors->addColumn(L"co-name");
    tableCompetitors->addColumn(L"team");
    tableCompetitors->addColumn(L"vehicle");
    tableCompetitors->addColumn(L"strength");

    // ----------------------------
    // Roads tab
    // ----------------------------
    irr::gui::IGUITab* tabRoads = tc->addTab(L"Roads", MI_TABROADS);

    TheGame::getInstance()->getEnv()->addStaticText(L"Road id",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabRoads)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxNewRoadName = TheGame::getInstance()->getEnv()->addEditBox(L"road_x",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 0), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabRoads,
        MI_EBNEWROADNAME);

    TheGame::getInstance()->getEnv()->addStaticText(L"Road file",
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabRoads)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxNewRoadFilename = TheGame::getInstance()->getEnv()->addEditBox(L"new road filename",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 22), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabRoads,
        MI_EBNEWROADFILENAME);

    TheGame::getInstance()->getEnv()->addStaticText(L"Road data file",
        irr::core::recti(irr::core::position2di(0, 2*22), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabRoads)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxNewRoadDataFilename = TheGame::getInstance()->getEnv()->addEditBox(L"new road data filename",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 2*22), irr::core::dimension2di(tabRoads->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
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

    window->setVisible(false);
}

MenuPageEditorRace::~MenuPageEditorRace()
{
    //window->remove();
    close();
}

bool MenuPageEditorRace::OnEvent(const irr::SEvent &event)
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
                        dprintf(MY_DEBUG_NOTE, "editor::race::save\n");
                        WStringConverter::toString(editBoxLongName->getText(), RaceManager::getInstance()->editorRace->raceLongName);
                        WStringConverter::toString(editBoxShortDescription->getText(), RaceManager::getInstance()->editorRace->shortDescription);
                        RaceManager::getInstance()->editorRace->write();
                        return true;
                        break;
                    case MI_BUTTONCREATEDAY:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::race::newDay\n");
                        std::string day;
                        WStringConverter::toString(editBoxNewDay->getText(), day);
                        Race::dayMap_t::const_iterator dit = RaceManager::getInstance()->editorRace->dayMap.find(day);
                        if (dit == RaceManager::getInstance()->editorRace->dayMap.end())
                        {
                            bool ret;
                            ret = ConfigDirectory::mkdir(DAY_DIR(RaceManager::getInstance()->editorRace->getName(), day));
                            if (ret)
                            {
                                RaceManager::getInstance()->editorRace->dayMap[day] = new Day(RaceManager::getInstance()->editorRace, RaceManager::getInstance()->editorRace->getName(), day, ret);
                                refreshDays();
                            }
                        }
                        return true;
                        break;
                    }
                    case MI_BUTTONCREATEROAD:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::race::newRoad\n");
                        std::string roadName;
                        std::string roadFilename;
                        std::string roadDataFilename;
                        WStringConverter::toString(editBoxNewRoadFilename->getText(), roadFilename);
                        WStringConverter::toString(editBoxNewRoadName->getText(), roadName);
                        WStringConverter::toString(editBoxNewRoadDataFilename->getText(), roadDataFilename);
                        RoadManager::roadMap_t::const_iterator rit = RaceManager::getInstance()->editorRace->roadMap.find(roadName);
                        dprintf(MY_DEBUG_NOTE, "\t%s\n\t%s\n\t%s\n\t%s\n",
                            roadName.c_str(), roadFilename.c_str(), roadDataFilename.c_str(),
                            RoadTypeManager::getInstance()->editorRoadType?RoadTypeManager::getInstance()->editorRoadType->getName().c_str():"ERROR - no road type selected");
                        if (rit == RaceManager::getInstance()->editorRace->roadMap.end() &&
                            !roadName.empty() &&
                            RoadTypeManager::getInstance()->editorRoadType)
                        {
                            bool ret;
                            ret = ConfigDirectory::mkdir(RACE_ROADS(RaceManager::getInstance()->editorRace->getName())) &&
                                ConfigDirectory::mkdir(RACE_ROADS_DATA(RaceManager::getInstance()->editorRace->getName()));
                            if (ret)
                            {
                                RaceManager::getInstance()->editorRace->roadMap[roadName] =
                                    new Road(roadFilename, roadName, roadDataFilename, RoadTypeManager::getInstance()->editorRoadType, false);
                                refreshRoads();
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
                    case MI_TABLEDAYS:
                        RaceManager::getInstance()->editorDay = (Day*)tableDays->getCellData(tableDays->getSelected(), 0);
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        return true;
                        break;
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
                    case MI_TABLEDAYS:
                        RaceManager::getInstance()->editorDay = (Day*)tableDays->getCellData(tableDays->getSelected(), 0);
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORDAY);
                        return true;
                        break;
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
            {
                switch (id)
                {
                    case MI_EBNEWROADNAME:
                        refreshRoadEditBoxes(editBoxNewRoadName->getText());
                        break;
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageEditorRace::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorRace::open()\n");
    refreshDays();
    refreshGlobalObjects();
    refreshEditBoxes();
    refreshRoads();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageEditorRace::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorRace::close()\n");
    window->setVisible(false);
}

void MenuPageEditorRace::refresh()
{
    refreshDays();
    refreshGlobalObjects();
    refreshCompetitors();
    refreshEditBoxes();
    refreshRoads();
}

void MenuPageEditorRace::refreshDays()
{
    // ----------------------------
    // Days
    // ----------------------------
    tableDays->clearRows();

    const Race::dayMap_t& dayMap = RaceManager::getInstance()->editorRace->getDayMap();
    unsigned int i = 0;
    for (Race::dayMap_t::const_iterator dit = dayMap.begin();
         dit != dayMap.end();
         dit++, i++)
    {
        irr::core::stringw str;
        
        tableDays->addRow(i);

        str += i;
        tableDays->setCellText(i, 0, str.c_str());
        tableDays->setCellData(i, 0, (void*)dit->second);

        str = L"";
        str += dit->first.c_str();
        tableDays->setCellText(i, 1, str.c_str());

        str = L"";
        str += dit->second->getLongName().c_str();
        tableDays->setCellText(i, 2, str.c_str());
    }
}

void MenuPageEditorRace::refreshGlobalObjects()
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
    const RaceManager::globalObjectList_t& globalObjectList = RaceManager::getInstance()->editorRace->globalObjectList;
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

void MenuPageEditorRace::refreshCompetitors()
{
    // ----------------------------
    // Competitors
    // ----------------------------
    tableCompetitors->clearRows();
    /*
    tableCompetitors->addColumn(L"#");
    tableCompetitors->addColumn(L"num");
    tableCompetitors->addColumn(L"name");
    tableCompetitors->addColumn(L"co-name");
    tableCompetitors->addColumn(L"team");
    tableCompetitors->addColumn(L"vehicle");
    tableCompetitors->addColumn(L"strength");
    */
    const Race::competitorMap_t& competitorMap = RaceManager::getInstance()->editorRace->getCompetitorMap();
    unsigned int i = 0;
    for (Race::competitorMap_t::const_iterator cit = competitorMap.begin();
         cit != competitorMap.end();
         cit++, i++)
    {
        irr::core::stringw str;
        
        tableCompetitors->addRow(i);

        str += i;
        tableCompetitors->setCellText(i, 0, str.c_str());

        str = L"";
        str += cit->second->getNum();
        tableCompetitors->setCellText(i, 1, str.c_str());

        str = L"";
        str += cit->second->getName().c_str();
        tableCompetitors->setCellText(i, 2, str.c_str());

        str = L"";
        str += cit->second->getCoName().c_str();
        tableCompetitors->setCellText(i, 3, str.c_str());

        str = L"";
        str += cit->second->getTeamName().c_str();
        tableCompetitors->setCellText(i, 4, str.c_str());

        str = L"";
        str += cit->second->getVehicleTypeName().c_str();
        tableCompetitors->setCellText(i, 5, str.c_str());

        str = L"";
        str += cit->second->getStrength();
        tableCompetitors->setCellText(i, 6, str.c_str());
    }
}

void MenuPageEditorRace::refreshEditBoxes()
{
    irr::core::stringw str;

    str += RaceManager::getInstance()->editorRace->getLongName().c_str();
    editBoxLongName->setText(str.c_str());

    str = L"";
    str += RaceManager::getInstance()->editorRace->getShortDescription().c_str();
    editBoxShortDescription->setText(str.c_str());

    str = L"";
    editBoxNewDay->setText(str.c_str());
}

void MenuPageEditorRace::refreshRoads()
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
    const RoadManager::roadMap_t& roadMap = RaceManager::getInstance()->editorRace->getRoadMap();
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

void MenuPageEditorRace::refreshRoadEditBoxes(const wchar_t* newRoadName)
{
    irr::core::stringw str;

    str = newRoadName;
    editBoxNewRoadName->setText(str.c_str());

    str = L"";
    str += (RACE_ROADS(RaceManager::getInstance()->editorRace->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadFilename->setText(str.c_str());

    str = L"";
    str += (RACE_ROADS_DATA(RaceManager::getInstance()->editorRace->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadDataFilename->setText(str.c_str());

}
