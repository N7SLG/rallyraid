# How to create new race with stages #

This page will show you how can you create a new race with their stages.
I will show you the steps of the creation on the race Dakar 2013.

## Content ##
  * [Prerequisites](#Prerequisites.md)
  * [Create race](#Create_race.md)
    * [Get earth data](#Get_earth_data.md)
    * [Open earth data texture](#Open_earth_data_texture.md)
    * [Steps inside the game](#Steps_inside_the_game.md)
    * [Navigation](#Navigation.md)
    * [Action](#Action.md)
  * [Continue race creation](#Continue_race_creation.md)
    * [Add race picture](#Add_race_picture.md)

## Prerequisites ##

You will need the following things to be able to create a new race:
  * installed game base, runtime (see: [Install Instructions](InstallInstructions.md))
  * some image maipulation program (prefer: [Gimp](http://www.gimp.org))

## Create race ##

Before start to create the race you must do few things.

### Get earth data ###

Earth data contains the terrain height data. That will deterimine the style of
the race. At this moment you can only download the data for Dakar 2012
([here](https://code.google.com/p/rallyraid/downloads/list)),
which is also good for 2013. Unfortunatelly the size of the data for the whole
earth is to large, but i have if you need any part of it (from America to the
Arabian peninsula).

### Open earth data texture ###

Start the image manipulation program. Open image: `<`game path`>`/data/earthdata/earth`_`data`_`texture.png.

### Steps inside the game ###

  * start the game
  * put the game into editor mode (left-bottom on the main screen)
  * save settings to make the editor mode permanent
  * click on _Start New Game_
  * if there was already at least one race, then click on _Start Game_ on the stage screen
  * press _F3_ if you are in the game, that will open the [editor window](EditorMainS.md)
  * click on _RaM_ tab (Race Manager)
  * type _race id_: dakar2013
  * click on create (new directory will be created under data/races)
  * double click on the new race in the list (race window will be opened)
  * fill necessarry datas
  * click on _Save_ (race.cfg will be created under data/races/dakar2013)
  * to create new day: type _Day id_: day01
  * click on _new day_ (day01 directory will be created under data/race/dakar2013)
  * double click on the new day in the list (day window will be opened)
  * fill necessary datas
  * click on _Save_ (day.cfg will be created under data/races/dakar2013/day01)
  * to create new stage: type _Stage id_: stage1
  * click on _new stage_ (stage1 directory will be created under data/race/dakar2013/day01)
  * double click on the new stage in the list (stage window will be opened)
  * fill necessary datas
  * click on _Save_ (stage.cfg will be created under data/races/dakar2013/day01/stage1)
  * click on _activate race_ to make the stage active, so next time if you place an object it will be visible

Now you have a race with an empty stage. You will need to edit the stage. Close stage, day and race window.

### Navigation ###

  * switch to the image manupulation program and select the pixel where you want to go: 1819, 2730
  * type these coordinates into _tileX_ and _tileY_
  * click on _reset_ (game will place you at that coordinate)
  * type _F_ to switch free flight mode, where you can navigate with the cursor and mouse
  * you can increse the speed of the camera by _O_, decrease by _L_
  * you can see some status in the editor infobar at the top-left corner
[![](http://rallyraid.googlecode.com/svn/images/wiki/editor_infobar.png)](http://code.google.com/p/rallyraid/wiki/EditorInfobar)

### Action ###

  * select a place where the stage should be
  * press _F3_ again to open editor window
  * select _add AI point_ in the action list
  * click on _action_
  * you can now navigate again
  * click with the left mouse button to place an AI point (next time you select the stage, you will be place there)
  * press _F4_ to exit from the _ACTION_ mode
  * double click on the stage line in the list (stage window will be opened)
  * click on _save_

## Continue race creation ##

You can any time stop editing the stage and continue later. Be careful to save all data (roads, stages, days, race).

### Add race picture ###

After restart the game, you can see a strange picture at the race picture. You can change it any time.

  * quit the game
  * find a suitable picture
  * copy it to the race directory, e.g.: data/races/dakar2013
  * you can edit the name of the picture later in the [race editor window](EditorRaceDays.md)