# Content #

  * [Introduction](#Introduction.md)
  * [Description](#Description.md)
    * [Road Properties](#Road_Properties.md)
      * [Fix Property](#Fix_Property.md)

# Introduction #

The roads tab are the same in the race, day, and stage window. So i only describe it here, because this is the most used road type.

![http://rallyraid.googlecode.com/svn/images/wiki/editor_stage_roads_cut.png](http://rallyraid.googlecode.com/svn/images/wiki/editor_stage_roads_cut.png)

# Description #
Road tab contains the list of the roads that are already created. And the inputs for the properties of the newly created roads (except the [road type](EditorMainRom.md)).

## Road Properties ##
| **Road id** / **name** | the name/id of the road. This must be unique in each stage. |
|:-----------------------|:------------------------------------------------------------|
| **Road file**          | the path to the road descriptor file, it is filled automatically when you type the road id. So you don't need to touch this property. |
| **Road data file**     | the path to the road data file, this file will contain the coordinates of the road points. It is filled automatically when you type the road id. So you don't need to touch this property. |
| **Type**               | the [type](EditorMainRom.md) of the road. You cannot create a road without a type. |
| **Road HM radius (HMR)** | (Integer value) Height modifier radius for this road: if you give a HM for the road, then the radius describe distance, how far from the road the game will modify the height of the earth. The measure is in detail size (16 meters), see [Detail pos](EditorInfobar.md). |
| **Road height (HM)**   | (float value) the height modifier of the road. You can change the earth height along the road, can be positive or negative value. |
| **Fix**                | the height of the road sides will be the same. See [below](#Fix_Property.md) |

### Fix Property ###

Non-Fix cross-sectional view:

```
\
 \ <--- ground
  \
   \\
    \\  <--- road
     \\
      \
       \
        \
```

Fix cross-sectional view:

```
\
 \ <--- ground
  |
  | 
  |____   <--- road
       |
       |
        \
         \
```