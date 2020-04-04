Game uses GOOP, (Shawns framework)
For fullScreen go into modules->FLORP->src->florp->app->Window.cpp
Replace the line 75 (myWindow = glfwCreateWindow(myWidth, myHeight, myTitle.c_str(), nullptr, nullptr);)
with 
myWindow = glfwCreateWindow(myWidth, myHeight, myTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);

Since it's a modification in florp it will not be saved when sent as such for full screen
this step is required.

Controls are wasd to move and arrow keys to look around.
E let's you interact with objects.

Aligned controls:

A* anywhere in the safe room by pressing r (a light will move to the position that the
						camera was at)
pressing 1,2,3,4 will enable or disable lights, color correction grey, color correction Sepia
		and 4 is bloom
 
