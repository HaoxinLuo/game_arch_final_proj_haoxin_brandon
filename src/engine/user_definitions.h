#pragma once

/**********************************************************************************
* Keyboard controls shared across any mode and agent (entity)
*   WASD : for standard movement of agent (will move every one at the same time).
*            this will also clear any path the agent was following
*		x    : tells every agent to pick a random location from a list of predetermined 
*            spots to path find to
*   left click : when there are multiple agents, this will tell them to each move 
*                  towards one other agent selected at random
*   q    : scales the agent models up in size to help see them in large maps
*   e    : scales the agent models down in size to help see them in small maps
*   IJKL : controls the camera the same way it was originally coded in the game 
*            engine
*   arrow keys : still performs the same camera functions as in the original engine
**********************************************************************************/


/*********************************************************************************
*	Simulation modes that activate preset debug settings
*		0 : runs the simulation with one agent on a medium map (the one we demoed). 
*         Path smoothing is enabled and the underlying mesh polygons are 
* 				displayed.
*   1 : runs the simulation with the same parameters as mode 0, but there are now 
*         two agents on the map.
*   2 : same as mode 0, but path smoothing is turned off so you can see the agent
*         zigzag its way to the target point. This is an undesirable artifact of 
*         A* path finding that is particularly sensitive to the underlying mesh
*         shape.
*   3 : runs the simulation with one agent on the 100x100 map without displaying
*         the underlying mesh layout to increase performance. The floor is rendered
*         from a dynamic_drawcall, which is why we believe it gets very slow at high
*         polygon counts. Path smoothing is still enabled. Mesh simplification is
*         limited to 500 edge collapses to make sure the process terminates within
*         within reasonable time.
*   4 : runs the simulation with the same parameters as mode 3, but turns off path
*         smooth because there are still some edge cases that causes it to stall
*         for seemingly forever.
*   5 : runs the simulation with the same parameters as mode 3, but with the big
*         mesh. This is not tested to terminate or perform correctly once it does.
*         see mode 3 for details on why.
*   6 : runs the simulation with the same parameters as mode 5, but with the path
*         smoothing turned off. As for why, see mode 4.
*********************************************************************************/
#define SIMULATION_MODE 0

#if SIMULATION_MODE == 0

#define SMOOTH_PATH true
#define SHOW_TRIANGLE_FOR_MESH true
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/small_square.png"


#elif SIMULATION_MODE == 1

#define SMOOTH_PATH true
#define SHOW_TRIANGLE_FOR_MESH true
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT true
#define INPUT_MAP_FILE "data/maps/small_square.png"


#elif SIMULATION_MODE == 2

#define SMOOTH_PATH false
#define SHOW_TRIANGLE_FOR_MESH true
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/small_square.png"


#elif SIMULATION_MODE == 3

#define SMOOTH_PATH true
#define SHOW_TRIANGLE_FOR_MESH false
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/center_box_big.png"


#elif SIMULATION_MODE == 4

#define SMOOTH_PATH false
#define SHOW_TRIANGLE_FOR_MESH false
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/center_box_big.png"


#elif SIMULATION_MODE == 5

#define SMOOTH_PATH true
#define SHOW_TRIANGLE_FOR_MESH false
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/nested_cube.bmp"


#elif SIMULATION_MODE == 6

#define SMOOTH_PATH false
#define SHOW_TRIANGLE_FOR_MESH false
#define EDGE_DEBUG true
#define MAX_TRIANGLES_SIMPLIFIED 500
#define TEST_MULTI_AGENT false
#define INPUT_MAP_FILE "data/maps/nested_cube.bmp";
#endif
