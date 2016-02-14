# Introduction #

As Rally Raid Game can handle several races there are some steps of the install:
  * [install the game](#Install_the_game.md)
    * [runtime](#Game_runtime.md)
    * [base](#Game_base.md)
  * [install race](#Install_a_race.md)
    * [base](#Race_base.md)
    * [stages](#Race_stages.md)
  * [install MS libraries if needed](#Install_MS_libraries.md)

You can use the game alone, or with many of the races.

# Install the game #

The game has two part, that you can download independently:
  * [runtime](#Game_runtime.md)
  * [base](#Game_base.md)

**The game only works if both parts are installed!** But after once installed both parts, it is possible to update only one of them.

## Game runtime ##

The runtime contains the executables, libraries and configuration files. The runtime often changes.

### File name format ###

**RallyRaid`_`runtime`_`v**`<`major`_`version`>`**`_`**`<`minor`_`version`>`**`_`b**`<`build`_`number`>`**.zip**

For example: `RallyRaid_runtime_v1_0_b162.zip`

The game runtime can only handle races with the same major number.
The game runtime is only compatible with the game base that has the same major and minor number. Use the most fresh base for a runtime that has not higher build number than the runtime.

## Game base ##

The base contains any other part, that need the game to work, and to create races from scratch:
  * backgrounds
  * earth texture
  * fonts
  * hud textures
  * roadbook textures
  * shader programs
  * menu textures
  * object meshes and textures
  * road types with textures
  * skies
  * smoke textures
  * sounds
  * vehicle datas and textures

The base part of the game changes rarely, that's why it is separated from the runtime.

### File name format ###

**RallyRaid`_`base`_`v**`<`major`_`version`>`**`_`**`<`minor`_`version`>`**`_`b**`<`build`_`number`>`**.zip**

For example: `RallyRaid_base_v1_0_b161.zip`

# Install a race #

Installing the race can depend on the race itself. For example the Dakar 2012 race has the following installation instruction:

The Dakar 2012 Race has two part:
  * [base](#Race_base.md)
  * [stages](#Race_stages.md)

## Race base ##

The base contains part of the race that changes rarely. In case of Dakar 2012 it contains only the height information of the Earth where the race is going. Anyway it is quite large, so it has the benefit, if an update need for the race the user don't need to download the whole large data again.

### File name format ###

**RallyRaid`_`**`<`name`_`of`_`the`_`race`>`**`_`base`_`v**`<`major`_`version`_`of`_`the`_`compatible`_`game`>`**`_`s**`<`version`_`of`_`this`_`file`>`**.zip**

For example: `RallyRaid_dakar2012_base_v1_s4.zip`

## Race stages ##

The stages describes the route and the object for all of the stages. It is quite small, so an update is easy to download.

### File name format ###

**RallyRaid`_`**`<`name`_`of`_`the`_`race`>`**`_`stages`_`v**`<`major`_`version`_`of`_`the`_`compatible`_`game`>`**`_`s**`<`version`_`of`_`this`_`file`>`**.zip**

For example: `RallyRaid_dakar2012_stages_v1_s3.zip`

**Both race base and stages are only compatible with the game (runtime and base) that has the same major version!**

# Install MS libraries #

[Microsoft Visual C++ 2008 Redistributable Package (x86)](http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=29)

[Additional msvc\*.dll files](http://rallyraid.googlecode.com/files/RallyRaid_msvc_dlls.zip)