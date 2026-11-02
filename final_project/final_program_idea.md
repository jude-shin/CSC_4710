# Maze Generation Runner

The idea is that there are preset objects that will act as walls

## Maze

- Buildings with different textures or heights
- They are loaded at random
- Have a fixed location

## Camera

You can toggle between views 1 and 2 using a key-bind.

- View 1 (FPS): you are walking through the maze using wasd for forward, left, 
back, and right respectively. The camera translates based on these movements.

- View 2 (Birds Eye): a top down view of the maze. wasd will move the player 
north, west, south, and east respectively. The camera does not translate.

## technicals

- just store a flag for the type of view you are on 
- store one hardcoded camera profile as the birds eye view.
- store the location (and orientation) of the person
    - based on this, the camera location and view can be derived.
    - when the user toggles from birds eye view to fps, the location of the 
    user is converted to a new camera location and orientation.

## Lighting 

- a user can have a light source illuminating from it's location
- there can be an overall light source for the whole scene (birds eye view)

## Solution

- If a user needs help with the solution, an illuminating line of blue dots 
could show the path to the solution
    - I am thinking of the "spirits" in brave.
    - They could also be like proximity, so only when you get within range of 
    one of "spirits" will the other one show up.
- Once you reach the end, all of the little blue dots could be rendered to show 
the overall path

## Extra Fun Stuff
- lanterns or spinning coins can be added?
    - lanterns will have their own "lights"
    - maybe you can have n number of lanterns at the start and there is no 
    overall light source.


