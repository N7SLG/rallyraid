# Content #
  * [Introduction](#Introduction.md)
  * [Description](#Description.md)
    * [Buttons](#Buttons.md)
    * [Tabs](#Tabs.md)
    * [Tab Content](#Tab_Content.md)
      * [Render Helpers](#Render_Helpers.md)
      * [Render All Roads](#Render_All_Roads.md)
      * [Tables](#Tables.md)
        * [Selection Table](#Selection_Table.md)
        * [Action Table](#Action_Table.md)

# Introduction #

This picture will show if you press _F3_ key to open editor window:

![http://rallyraid.googlecode.com/svn/images/wiki/editor_main_s2.png](http://rallyraid.googlecode.com/svn/images/wiki/editor_main_s2.png)

# Description #

The S (selected) tab of the main window is the central point of the editor. You can start the actions here (put objects, add road point). Place yourself anywhere you want. You will you the quite often during the stage editing.

## Buttons ##

| **Refresh** | refresh the content of the main window (every tab), you don't need to use it |
|:------------|:-----------------------------------------------------------------------------|
| **action** / **ACTION** | you can switch to action mode, if you click the _action_ button the button title will change to _ACTION_, that means you are in [action mode](Terminology#Action_Mode.md). **ATTENTION**: if the button title is _ACTION_ you cannot use the editor windows, and the mouse click will do the action! |
| **activate race** | activate the selected [race](Terminology#Race.md) and [stage](Terminology#Stage.md), which means deactivate the old ones, remove _objects_/_roads_ everything that belongs to the old [stage](Terminology#Stage.md) and add _objects_/_roads_/_itinerary points_ to the scene from the new stage. |
| **reset**   | put you and your car to the [tile](Terminology#Earth_Tile.md) that you wrote into the _tile inputs_ on the _S_ tab on on the main window |
| **reload**  | reload , but you will be left at the position where you were.                |
| **jump end** | jump to the last itinerary point. Useful when you have to close the program and leave during the editing and you need to continue later. |


## Tabs ##

| **S** | see this page :) |
|:------|:-----------------|
| **Tls** | gives you some information about the currently used [tiles](Terminology#Earth_Tile.md) |
| **OW\_G** | gives you some information about global object in the _object wire_. _Object wire_ is used to display the objects that are near to you. |
| **OW\_T** | _Object wire_ use _tiles_, a _tile_ is an object batch, easy to handle. This tab contains information about the _object wire tiles_. |
| **OPool** | [Object Pool](EditorMainOpool.md) |
| **RaM** | [Race Manager settings](EditorMainRam.md) |
| **RoM** | [Road Manager settings](EditorMainRom.md) |
| **Iti** | [Itinerary settings](EditorMainIti.md) |

## Tab content ##

### Render Helpers ###
If it is checked, then it is use special draw elements to indicate the stage elements (object, road points, itinerary points, way points).

### Render All Roads ###
If it is checked render helpers of all road points of all [roads](Terminology#Road.md) will be rendered. Otherwise only the currently selected will be rendered.

### Tables ###

There are two tables on the selection tab:
  * selection table
  * action table

#### Selection Table ####
Selection table contains the currently selected:
  * object type
  * [race](Terminology#Race.md)
  * [day](Terminology#Day.md)
  * [stage](Terminology#Stage.md)
  * [road](Terminology#road.md)
  * [road type](EditorMainRom.md)
  * [itinerary images](EditorMainIti.md)
  * [way point type](EditorStageWp.md)
  * stage length

Many of the list element like race, day, stage is a shortcut to reach the race-, day-, stage window. So you don't need to go the [stage window](EditorStageGo.md) through the [race manager tab](EditorMainRam.md) -> race -> day -> stage.
_Stage length_ is calculated from the distances of the AI points.

#### Action Table ####
Action table contains the actions, that you can do during the stage editing: add/remove stuffs.