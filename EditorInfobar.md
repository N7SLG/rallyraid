# Introduction #

The info bar is placed on the left-top of the game window and gives you information about many things in editor mode:

![http://rallyraid.googlecode.com/svn/images/wiki/editor_infobar.png](http://rallyraid.googlecode.com/svn/images/wiki/editor_infobar.png)

## Description ##

| **Secs** | how many seconds have you started the game. |
|:---------|:--------------------------------------------|
| **Pos**  | This is your real position inside the _render world_, that is not your position in the _real world_ (except the _Y_ coordinate) |
| **Offset** | _Pos_ + _Offset_ gives your position in the real world (the _Z_ coordinate is negative) |
| **Tile pos** | a tile is a 2 km x 2 km square. Every pixel of `<`game path`>`/data/earthdata/earth`_`data`_`texture.png is a tile. _Tile pos_ is the current pixel on this map. |
| **Polygon count** | the number of the currently draw polygons   |
| **Detail pos** | Every [tile](Terminology#Earth_Tile.md) has 32 x 32 height data. The distance between two height point is 64 meters (32 x 64 = 2048 meters = ~2 km = _tile size_). 64 meters is to rare so the game interpolates the height (_detail point_) points between two height data points. There are 3 _detail point_ between the height points, so the distance of the _detail points_ are 64 / 4 = 16 meters (_detail scale_). The _detail pos_ is the position / _detail scale_  |
| **Distance to the last itinerary point** | Distance to the last itinerary point.       |
| **FPS speed** | is the camera speed in FPS mode (_F_ key by default), you can increase the camera speed to 125% by _O_ key or decrease to 80% by _L_ key of the current speed |