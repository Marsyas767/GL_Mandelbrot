# GL_Mandelbrot
Simple Mandelbrot set implementation in OpenGL and C++
# Requirements
- GLFW
- gl3w
# Controls
WASD to move  
Z to zoom in  
X to zoom out  
Up Arrow to increase resolution  
Down Arrow to decrease resolution  
# Performance
If it is not running well, replace all the double variables with floats. It will mean that you can't zoom in as far but it should bump up the FPS.  
If you are running on older hardware that doesn't support OpenGL 4.0+, do the same thing but also change the version string and window hints.
