# Content #

  * [Introduction](#Introduction.md)
  * [Description](#Description.md)
    * [Road Stage Table](#Road_Stage_Table.md)
    * [Visual Road Table](#Visual_Road_Table.md)
    * [Road Types Table](#Road_Types_Table.md)

# Introduction #

Race manager contains information about road types, and currently visible roads.

![http://rallyraid.googlecode.com/svn/images/wiki/editor_main_rom.png](http://rallyraid.googlecode.com/svn/images/wiki/editor_main_rom.png)

# Description #

## Road Stage Table ##
N/A

## Visual Road Table ##

Displaying all part of a long road make a big performance hit. So i only display the parts of the roads, which are next to the player. How? As i chopped the [Earth](Terminology#Earth.md) into [tiles](Terminology#Earth_Tile.md), i also chopping the roads at stage loading along these [tiles](Terminology#Earth_Tile.md). I call the part of the road which is on a tile _road chunk_. This table contains information about the _road chunks_:
  * road name
  * the number of the start point of the chunk
  * the number of the end point of the chunk

## Road Types Table ##
The list of the available road types. If you select a road type from the list, then its texture will be displayed in the image above this table.

There is a special type _null`_`road_, which is used in case where you don't need the visible stuf only the property of the roads:
  * makes object density zero, so there won't be placed objects along the road
  * makes ground texture a bit different

More information can be found on the [road tab page](EditorStageRoads.md).