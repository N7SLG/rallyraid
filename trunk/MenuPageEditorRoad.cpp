
#include "MenuPageEditorRoad.h"
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

MenuPageEditorRoad* MenuPageEditorRoad::menuPageEditorRoad = 0;

MenuPageEditorRoad::MenuPageEditorRoad()
    : window(0),
      tablePoints(0),
      editBoxRadius(0),
      editBoxRed(0),
      editBoxGreen(0),
      editBoxBlue(0)
{
    menuPageEditorRoad = this;
    window = TheGame::getInstance()->getEnv()->addWindow(
        irr::core::recti(TheGame::getInstance()->getScreenSize().Width-350, 50, TheGame::getInstance()->getScreenSize().Width-10, TheGame::getInstance()->getScreenSize().Height-150),
        false,
        L"Editor - Road",
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

    irr::gui::IGUITabControl* tc = TheGame::getInstance()->getEnv()->addTabControl(
        irr::core::recti(irr::core::position2di(2, 44), irr::core::dimension2di(window->getRelativePosition().getSize().Width - 4, window->getRelativePosition().getSize().Height - 46)),
        window,
        true,
        true,
        MI_TABCONTROL);

    // ----------------------------
    // Points tab
    // ----------------------------
    irr::gui::IGUITab* tabPoints = tc->addTab(L"Points", MI_TABPOINTS);

    TheGame::getInstance()->getEnv()->addStaticText(L"Road point radius",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabPoints)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxRadius = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 0), irr::core::dimension2di(tabPoints->getRelativePosition().getSize().Width-EXP_TEXT_WIDTH, 20)),
        true,
        tabPoints,
        MI_EBRADIUS);

    TheGame::getInstance()->getEnv()->addStaticText(L"Color modifier (RGB)",
        irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2di(EXP_TEXT_WIDTH, 20)),
        false,
        false,
        tabPoints)->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
    editBoxRed = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH, 22), irr::core::dimension2di(40, 20)),
        true,
        tabPoints,
        MI_EBRED);

    editBoxGreen = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH+42, 22), irr::core::dimension2di(40, 20)),
        true,
        tabPoints,
        MI_EBGREEN);

    editBoxBlue = TheGame::getInstance()->getEnv()->addEditBox(L"0",
        irr::core::recti(irr::core::position2di(EXP_TEXT_WIDTH+84, 22), irr::core::dimension2di(40, 20)),
        true,
        tabPoints,
        MI_EBBLUE);

    tablePoints = TheGame::getInstance()->getEnv()->addTable(
        irr::core::recti(irr::core::position2di(0, 44), irr::core::dimension2di(tabPoints->getRelativePosition().getSize().Width, tabPoints->getRelativePosition().getSize().Height-44)),
        tabPoints,
        MI_TABLEPOINTS,
        true);

    tablePoints->addColumn(L"#");
    tablePoints->addColumn(L"X");
    tablePoints->addColumn(L"Y");
    tablePoints->addColumn(L"Z");
    tablePoints->addColumn(L"Ra");
    tablePoints->addColumn(L"R");
    tablePoints->addColumn(L"G");
    tablePoints->addColumn(L"B");

    window->setVisible(false);
}

MenuPageEditorRoad::~MenuPageEditorRoad()
{
    //window->remove();
    menuPageEditorRoad = 0;
    close();
}

bool MenuPageEditorRoad::OnEvent(const irr::SEvent &event)
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
                        dprintf(MY_DEBUG_NOTE, "editor::road::save\n");
                        //WStringConverter::toString(editBoxLongName->getText(), RaceManager::getInstance()->editorRace->raceLongName);
                        //WStringConverter::toString(editBoxShortDescription->getText(), RaceManager::getInstance()->editorRace->shortDescription);
                        RoadManager::getInstance()->editorRoad->writeHeader();
                        RoadManager::getInstance()->editorRoad->writeData();
                        return true;
                        break;
                };
                break;
            }
            /*
            case irr::gui::EGET_TABLE_CHANGED:
            {
                switch (id)
                {
                    case MI_TABLEPOINTS:
                        RaceManager::getInstance()->editorDay = (Day*)tableDays->getCellData(tableDays->getSelected(), 0);
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
                        MenuManager::getInstance()->open(MenuManager::MP_EDITORDAY);
                        return true;
                        break;
                };
                break;
            }
            */
            case irr::gui::EGET_EDITBOX_CHANGED:
            case irr::gui::EGET_EDITBOX_ENTER:
            {
                switch (id)
                {
                    case MI_EBRADIUS:
                        WStringConverter::toInt(editBoxRadius->getText(), RoadManager::getInstance()->editorRadius);
                        break;
                    case MI_EBRED:
                    {
                        int ret = 0;
                        WStringConverter::toInt(editBoxRed->getText(), ret);
                        RoadManager::getInstance()->editorColor.setRed(ret);
                        break;
                    }
                    case MI_EBGREEN:
                    {
                        int ret = 0;
                        WStringConverter::toInt(editBoxGreen->getText(), ret);
                        RoadManager::getInstance()->editorColor.setGreen(ret);
                        break;
                    }
                    case MI_EBBLUE:
                    {
                        int ret = 0;
                        WStringConverter::toInt(editBoxBlue->getText(), ret);
                        RoadManager::getInstance()->editorColor.setBlue(ret);
                        break;
                    }
                }
                break;
            }
        };
    }
    return false;
}

void MenuPageEditorRoad::open()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorRoad::open()\n");
    refreshColor();
    window->setVisible(true);
    TheGame::getInstance()->getEnv()->setFocus(window);
}

void MenuPageEditorRoad::close()
{
    dprintf(MY_DEBUG_NOTE, "MenuPageEditorRoad::close()\n");
    window->setVisible(false);
}

void MenuPageEditorRoad::refresh()
{
    refreshPoints();
}

void MenuPageEditorRoad::refreshPoints()
{
    // ----------------------------
    // Points
    // ----------------------------
    tablePoints->clearRows();

    const Road::roadPointVector_t& roadPointVector = RoadManager::getInstance()->editorRoad->roadPointVector;
    unsigned int i = 0;
    for (Road::roadPointVector_t::const_iterator rpit = roadPointVector.begin();
         rpit != roadPointVector.end();
         rpit++, i++)
    {
        irr::core::stringw str;
        
        tablePoints->addRow(i);

        str += i;
        tablePoints->setCellText(i, 0, str.c_str());
        //tablePoints->setCellData(i, 0, (void*)rpit->second);

        str = L"";
        str += rpit->p.X;
        tablePoints->setCellText(i, 1, str.c_str());

        str = L"";
        str += rpit->p.Y;
        tablePoints->setCellText(i, 2, str.c_str());

        str = L"";
        str += rpit->p.Z;
        tablePoints->setCellText(i, 3, str.c_str());

        str = L"";
        str += rpit->radius;
        tablePoints->setCellText(i, 4, str.c_str());

        str = L"";
        str += rpit->color.getRed();
        tablePoints->setCellText(i, 5, str.c_str());

        str = L"";
        str += rpit->color.getGreen();
        tablePoints->setCellText(i, 6, str.c_str());

        str = L"";
        str += rpit->color.getBlue();
        tablePoints->setCellText(i, 7, str.c_str());
    }
}

void MenuPageEditorRoad::refreshColor()
{
    irr::core::stringw str;

    str += RoadManager::getInstance()->editorColor.getRed();
    editBoxRed->setText(str.c_str());

    str = L"";
    str += RoadManager::getInstance()->editorColor.getGreen();
    editBoxGreen->setText(str.c_str());

    str = L"";
    str += RoadManager::getInstance()->editorColor.getBlue();
    editBoxBlue->setText(str.c_str());

    str = L"";
    str += RoadManager::getInstance()->editorRadius;
    editBoxRadius->setText(str.c_str());
}
