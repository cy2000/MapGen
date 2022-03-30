# MapGen

MapGen v0.1.1
************************************************************************
Setup
************************************************************************
Please set "MapGenerator" as startup project.
The project builds under SDL_Debug or SDL_Release.
Please run it using SDL_Release.
************************************************************************
Release mode Control:

On the startup menu:

	- you can click "Random" button to get a new seed, 
	or input a seed directly. 

	- click "Generate" generates the world map base on the seed.
	
	- click "Quit" to quit the program.

On the World Map:
	-click any place to get in the local map.
	-press R to regenerate a new map.
	
On the Local Map:
	-press Esc to go back to world map.

************************************************************************
Debug mode Control:

On the World Map:

	- press q/w to change octaves

	- press a/s to change persistance
	
	- press z/x to change zooming levels
  
  - press space to split the world
  
  - press 1 to see the split lines

  - press 2 to see noise
  
  - press enter to generate the final map

  -click any place to get in the local map.
  
	-press R to start over.
  
On the Local Map:
	-press Esc to go back to world map.
  
  -press 4/5 to see the elevation visualized.

************************************************************************
ChangeLog v 0.1.1:
	-Better route method to decide how cities connects each other.
	-On the local map, connects the buildings with local roads (build corridors).

