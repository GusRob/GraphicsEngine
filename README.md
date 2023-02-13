# GraphicsEngine
Creating a 3D rendering graphics engine from scratch. C++ with SDL for window generation and control of individual pixel colours. All additional drawing functions will be built by me.

coursework-result.mp4 is a file containing the resulting video for my coursework submitted to a similarly names unit in my first year at university. Due to time constraints there are several issues with it. The aim of this project is ultimately to refactor the code to fix these issues and extend the project.

Old project had poor file structures, large unreadable method and variable names - current system uses main.cpp as demonstration of the library I will create. Data structures are located in libs/sdw/, this includes scene content structures and mathematical models as well as drawing window instantiation and pixel control functions. Actual abstract rendering functions are located in libs/rend. This is where 2D, 3D rasterized, ray traced and shader functions will be located.

Features included in the old coursework to replicate as a base:

line rasterising
2d triangle rasterising

3d points projected onto 2d screen
triangles in 3d space rasterised
depth correction for rasterised objects
texture mapping onto rasterised 3d triangles

ray tracing for 3d objects
texture mapping for ray traced 3d objects
diffuse ambient and spectral lighting
gouraud and phong shading
reflection and refraction (including fresnel effect)
bump maps

Extension features not included in last project:
photon mapping
...?


**Note** the mtl obj and mtl files were handwritten by me, they don't necessarily follow the proper file specifications and syntax, but functionally act the same
