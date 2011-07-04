
#include "MenuPageEditorDay.h"
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

MenuPageEditorDay::MenuPageEditorDay()
    : window(0),
      tableStages(0),
      tableGlobalObjects(0),
      tableRoads(0),
      editBoxLongName(0),
      editBoxShortDescription(0),
      editBoxNewStage(0),
      editBoxNewRoadFilename(0),
      editBoxNewRoadName(0),
      editBoxNewRoadDataFilename(0)
{
    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width-350, 50, TheGame::getInstance()->getScreenSize().Width-10, TheGame::getInstance()->getScreenSize().Height-150),
        false,
        L"Editor - Day",
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
        MI_BUTTONCREATESTAGE,
        L"new stage");

    TheGame::getInstance()->getEnv()->addButton(
        irr::core::recti(128,22,168,42),
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

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 88), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 90)),
        window,
        true,
        true,
        MI_TABCONTROL);

    // ----------------------------
    // Stages tab
    // ----------------------------
    irr::gui::IGUITab* tabStages = tc->addTab(L"Stages", MI_TABSTAGES);

    editBoxNewStage = TheGame::getInstance()->getEnv()->addEditBox(L"new stage name",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(tabStages->getRelativePosition().getSize().Width, 20)),
        true,
        tabStages,
        MI_EBNEWSTAGENAME);


    tableStages = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 22), irr::core::dimension2di(tabStages->getRelativePosition().getSize().Width, tabStages->getRelativePosition().getSize().Height-22)),
        tabStages,
        MI_TABLESTAGES,
        true);

    tableStages->addColumn(L"#");
    tableStages->addColumn(L"name");
    tableStages->setColumnWidth(1, 80);
    tableStages->addColumn(L"long name");
    tableStages->setColumnWidth(2, 180);

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

    window->setVisible(false);
}

MenuPageEditorDay::~MenuPageEditorDay()
{
    //window->remove();
    close();
}

bool MenuPageEditorDay::OnEvent(const irr::SEvent &event)
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
                        dprintf(MY_DEBUG_NOTE, "editor::day::save\n");
                        WStringConverter::toString(editBoxLongName->getText(), RaceManager::getInstance()->editorDay->dayLongName);
                        WStringConverter::toString(editBoxShortDescription->getText(), RaceManager::getInstance()->editorDay->shortDescription);
                        RaceManager::getInstance()->editorDay->write();
                        return true;
                        break;
                    case MI_BUTTONCREATESTAGE:
                    {
                        dprintf(MY_DEBUG_NOTE, "editor::day::newStage\n");
                        std::string stage;
                        WStringConverter::toString(editBoxNewStage->getText(), stage);
                        Day::stageMap_t::const_iterator sit = RaceManager::getInstance()->editorDay->stageMap.find(stage);
                        if (sit == RaceManager::getInstance()->editorDay->stageMap.end())
                        {
                            bool ret;
                            ret = ConfigDirectory::mkdir(STAGE_DIR(RaceManager::getInstance()->editorRace->getName(), RaceManager::getInstance()->editorDay->getName(), stage));
                            if (ret)
                            {
                                RaceManager::getInstance()->editorDay->stageMap[stage] = new Stage(RaceManager::getInstance()->editorDay, RaceManager::getInstance()->editorRace->getName(), RaceManager::getInstance()->editorDay->getName(), stage, ret);
                                refreshStages();
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
                        RoadManager::roadMap_t::const_iterator rit = RaceManager::getInstance()->editorDay->roadMap.find(roadName);
                        if (rit == RaceManager::getInstance()->editorDay->roadMap.end() &&
                            !roadName.empty() &&
                            RoadTypeManager::getInstance()->editorRoadType)
                        {
                            bool ret;
                            ret = ConfigDirectory::mkdir(DAY_ROADS(RaceManager::getInstance()->editorRace->getName(),RaceManager::getInstance()->editorDay->getName())) &&
                                ConfigDirectory::mkdir(DAY_ROADS_DATA(RaceManager::getInstance()->editorRace->getName(),RaceManager::getInstance()->editorDay->getName()));
                            if (ret)
                            {
                                RaceManager::getInstance()->editorDay->roadMap[roadName] =
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
                    case MI_TABLESTAGES:
                        RaceManager::getInstance()->editorStage = (Stage*)tableStages->getCellData(tableStages->getSelected(), 0);
                        ItinerManager::getInstance()->editorGlobalDistance = 0.f;
                        if (!RaceManager::getInstance()->editorStage->itinerPointList.empty())
                        {
                            ItinerManager::getInstance()->editorGlobalDistance = RaceManager::getInstance()->editorStage->itinerPointList.back()->getGlobalDistance();
                        }
                        MenuPageEditor::menuPageEditor->refreshSelected();
                        MenuPageEditor::menuPageEditor->refreshItinerGD();
                        return true;
                        break;
                    case MI_TABLEROADS:
                        RoadManager::getInstance()->editorRoad = (Road*)tableRoads->getCellData(tableRoads->getSelected(), 0);
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
                    case MI_TABLESTAGES:
                        RaceManager::getInstance()->editorStage = (Stage*)tableStages->getCellData(tableStages->getSelected(), 0);
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORSTAGE);
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

void MenuPageEditorDay::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorDay::open()\n");
    refresh();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageEditorDay::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorDay::close()\n");
    window->setVisible(false);
}

void MenuPageEditorDay::refresh()
{
    refreshStages();
    refreshGlobalObjects();
    refreshEditBoxes();
    refreshRoads();
}

void MenuPageEditorDay::refreshStages()
{
    // ----------------------------
    // Stages
    // ----------------------------
    tableStages->clearRows();

    const Day::stageMap_t& stageMap = RaceManager::getInstance()->editorDay->getStageMap();
    unsigned int i = 0;
    for (Day::stageMap_t::const_iterator sit = stageMap.begin();
         sit != stageMap.end();
         sit++, i++)
    {
        irr::core::stringw str;
        
        tableStages->addRow(i);

        str += i;
        tableStages->setCellText(i, 0, str.c_str());
        tableStages->setCellData(i, 0, (void*)sit->second);

        str = L"";
        str += sit->first.c_str();
        tableStages->setCellText(i, 1, str.c_str());

        str = L"";
        str += sit->second->getLongName().c_str();
        tableStages->setCellText(i, 2, str.c_str());
    }
}

void MenuPageEditorDay::refreshGlobalObjects()
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
    const RaceManager::globalObjectList_t& globalObjectList = RaceManager::getInstance()->editorDay->globalObjectList;
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

void MenuPageEditorDay::refreshEditBoxes()
{
    irr::core::stringw str;

    str += RaceManager::getInstance()->editorDay->getLongName().c_str();
    editBoxLongName->setText(str.c_str());

    str = L"";
    str += RaceManager::getInstance()->editorDay->getShortDescription().c_str();
    editBoxShortDescription->setText(str.c_str());

    str = L"";
    editBoxNewStage->setText(str.c_str());
}

void MenuPageEditorDay::refreshRoads()
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
    const RoadManager::roadMap_t& roadMap = RaceManager::getInstance()->editorDay->getRoadMap();
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

void MenuPageEditorDay::refreshRoadEditBoxes(const wchar_t* newRoadName)
{
    irr::core::stringw str;

    str = newRoadName;
    editBoxNewRoadName->setText(str.c_str());

    str = L"";
    str += (DAY_ROADS(RaceManager::getInstance()->editorRace->getName(),RaceManager::getInstance()->editorDay->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadFilename->setText(str.c_str());

    str = L"";
    str += (DAY_ROADS_DATA(RaceManager::getInstance()->editorRace->getName(), RaceManager::getInstance()->editorDay->getName())+std::string("/")).c_str();
    str += newRoadName;
    editBoxNewRoadDataFilename->setText(str.c_str());

}
