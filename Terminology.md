# Terminology #

This page contains the terminology that is used by the Rally Raid Game.


## Content ##
  * [Action Mode](#Action_Mode.md)
  * [AI Point](#AI_Point.md)
  * [Day](#Day.md)
  * [Earth](#Earth.md)
  * [Earth Tile](#Earth_Tile.md)
  * [Height Modifier](#Height_Modifier.md)
  * [Itinerary Point](#Itinerary_Point.md)
  * [Object](#Object.md)
  * [Race](#Race.md)
  * [Road](#Road.md)
  * [Stage](#Stage.md)
  * [Way Point](#Way_Point.md)


### Action Mode ###

There is two mode inside the editor when the editor window is open:
  * normal mode
  * action mode

In normal mode you can use the editor windows and their content as usual: you can click on the buttons, type in the editor boxes. But in _action mode_ the inputs is not forwarded to the window handler, so you cannot click on the buttons. The click in editor mode make an action:
  * put/remove an object
  * add/remove road point
  * etc.

**Turn on** - click on _action_ button in the [editor main window](EditorMainS.md).

**Turn off** - press _F4_

### AI Point ###

The route that the AI competitors will follow is deterimed by the AI points.
If the AI competitor reach an AI point it will start to follow the next.
First AI point is the start point, the player will be dropped there at stage
start. Last AI point is the finish, where the player can finish the stahe (stop
the clock).


### Day ###

The day is a container, that is under the race. This means a day can have
special objects and roads. So if you attach an object for a day, that won't
visible automatically on the next day. In Dakar rally there is only one
stage at each days. So there is no special meaning in this case, but in other
rallies there can be more stages at one day.


### Earth ###

The speciality of the game that you can reach almost every place of the whole
Earth at one load. So once you load a stage, than you can go everywhere you
want to. It only depends on if you have the height data.
The height data of the whole earth is quite large (10 GB). So I chopped it
into [tiles](#Earth_Tile.md). The every 100 x 100 [tiles](#Earth_Tile.md) are grouped into _tile category_. The _tile categories_ are stored in separate zip files under data/earthdata/tiles. So you can deliver a race only with the required _tile categories_ (the
Dakar 2012 Game contained ~480 MB of it which is 52 _tile categories_).
The dismemberment can be found in the game base (see downloads) in the following
files:
  * data/earthdata/earth`_`data`_`hasDetail`_`cat`_`num.png
  * data/earthdata/earth`_`data`_`texture`_`cat.png
These files are standard PNG images.


### Earth Tile ###
Every tile has 32 x 32 height data. The distance between two height point is 64 meters (32 x 64 = 2048 meters = ~2 km = tile size).


### Height Modifier ###

The height modifier is a special point of the stage, where you can modify the
height of the terrain. So you can make small hills, dips and bumps.


### Itinerary Point ###

The itinerary (roadbook) is the base of the navigation. It shows the route of
the race, where the driver should go to be able to finish the race. An entry
in the roadbook is the itinerary point. This point contains:
  * number
  * distance information (total, partial)
  * tulip image
  * description image
  * description text (including CAP)

This information for a stage is stored in data/races/`<`raceid`>`/`<`dayid`>`/`<`stageid`>`/itiner.cfg

### Object ###

In the game there are two kind of objects:
  * random objects
  * global objects

The random objects are put in random way where the player goes to. The global
ones is determined by the stage/day/race, how it was placed in the editor.

The random objects placement is determined by the density map:
data/earthdata/earth`_`data`_`density.png in the game base. There is category of each
object, which is presented by a color in the density map:
  1. Red
  1. Green
  1. Blue

Categories:
  1. normal
  1. tropical
  1. desert

The darker color means less dense, the ligher means higher dense. So black means
there is no random objects at all.

The available objects are stored in the game base in data/objects/objects.cfg.
Let see an example:

```
[my_tree6]
mesh=data/objects/meshes/my_tree6.mso
texture=data/objects/textures/OakBark.png
texture2=data/objects/textures/my_tree6.png
num=2
material=normal
material2=normal_t
mass=4.0
category=1
physics=yes
type=my_tree
```

| **`[`my`_`tree6`]`** | The id (name) of the object. This will be shown in the editor, and store in the objects.cfg file of the stages, days and races. |
|:---------------------|:--------------------------------------------------------------------------------------------------------------------------------|
| **mesh**             | the object mesh file (mostly I use Milkshape 3D object .ms3d, but severaly 3D format can be used). Type _tree_  and _grass_ has no mesh, they generates runtime the mesh. |
| **texture**          | the main texture of the object                                                                                                  |
| **texture2**         | the secondary texture of the object (optional). In the _my_`_`_tree_ object type the bark and the leaf has different textures   |
| **num**              | how many object should be generate in an object wire tile (250 x 250 squaremeters)                                              |
| **material**         | the main material of the object, this is a shader type id from data/materials/sm20.cfg and data/materials/base\_materials.cfg   |
| **material2**        | the secondary material type id (optional, like at _texture2_)                                                                   |
| **mass**             | if the _type_ is _vehicle_ then it is the mass of the object in Kg. If the _type_ is not _vehicle_ then it is a scale factor if the mass type is .mso |
| **category**         | 0 - no category (not used as a random object: vehicles, houses), 1 - cat 1 (normal), 2 - cat 2 (tropical), 4 - cat 3 (desert). You can use mixture of the categories: 6 means mixture of 2 and 4 (logical or), means it is also present at tropical and desert. |
| **physics**          | add it to the physical engine or not, can you collide to it (you cannot collide to grass). Default: no                          |
| **type**             | optional, can be: _standard_ (default), _vehicle_, _grass_, _tree_, _my_`_`_tree_. The type is used for many things.            |



### Race ###

The main container of the game. A race (e.g.: Dakar 2012) contains: days,
race objects, race roads. So in editor mode you can attach objects/roads
for the race that will be available during the race at all stages.
Attach stages to a race is only possible through the days.
I used to attach roads to race in case of main roads (e.g.: Road 5) and
objects when it is like a town, so you may need it at several stages.


### Road ###

A road is a visible curlicued stuff. At this moment it does not have physical
behaviour (because of performance issues). It is the most important part of
the navigation.
The road types are defined: data/roads/types/road\_types.txt


### Stage ###

This is the base container of the game. Responsible for many things:
  * objects
  * roads
  * AI points
  * way points
  * height modifiers
  * itinerary


### Way Point (WP) ###

The way points are special part of the route. It can be some dangerous place
or some hidden place that you should find. During the stage editing, take
care of all way points must be show by the roadbook. Now there is two kind of
way points:
  * WPS - safety way point (i used it mostly at dangerous places)
  * WPM - hidden way point (i used it mostly at some objects or intersection)

During the race, the player must touch all of these points, if not the player
will get penalty. So you should place as many WPs as you can, to force the
player to follow the route.