# Content #

  * [Introduction](#Introduction.md)
  * [Description](#Description.md)
    * [Help For Calculation The Local Distance](#Help_For_Calculation_The_Local_Distance.md)

# Introduction #

The most important part of the navigation system is the road book. You can place road book elements (called itinerary point) along the stage.

![http://rallyraid.googlecode.com/svn/images/wiki/editor_main_iti.png](http://rallyraid.googlecode.com/svn/images/wiki/editor_main_iti.png)

# Description #

| **total distance** | (at the top of the tab) total distance that is determined by the summary of the local distances of each element. It is in meters. |
|:-------------------|:----------------------------------------------------------------------------------------------------------------------------------|
| **Local dist.**    | the distance from the previous itinerary point, it is very important, be punctual. It is in meters.                               |
| **Description**    | you can add some extra information to each road book entry: CAP, dangerous level.                                                 |
| **Itiner image**   | this will be the _tulip_ image. Mostly describes the direction.                                                                   |
| **Itiner image 2** | will be the part of the description. Some extra visual information.                                                               |

## Help For Calculation The Local Distance ##
If you edit the stage step by step: go along the stage and place AI and itinerary points at a time then the editor will help in the calculation of the calculation.

**How?**

For example, at the start:
  * place AI point at the begin, where the race should start.
  * place the start itinerary point (_local distance_: 0, _description_: START, _itiner image_: track`_`ahead.png, _itiner image 2_: start`_`with`_`clock.png)
  * AI-iti: 0 will be appear in the stage length info on the [editor - main window - selection tab - selection table](EditorMainS#Selection_Table.md)
  * place AI points along the stage to the next action point which you should add to the road book. (the _AI-iti_ number will increase, and tell the distance from the last itinerary point in meters)
  * go the _Local dist._ input if the _itinerary tab_ and press _Enter_. The _AI-iti_ number will be writen into the local distance.