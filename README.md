# Metaball Rendering Project Using Marching Cubes and Skybox

## Overview
This project demonstrates the rendering of metaballs using the marching cubes algorithm, combined with a skybox environment. The rendered metaballs exhibit reflections, adding to the visual realism. This guide provides an overview of the project structure, setup, and usage.

## Demo
<img src="./media/demo.gif" width="400px" alt="Demo">

## Prerequisites
- OpenGL 4.1 or higher
- SDL2 library
- GLAD OpenGL loader
- glm (OpenGL Mathematics)

## Installation
1. Ensure all prerequisites are installed on your system.
2. Clone the repository or download the source code.
3. Compile the source code using a compatible C++ compiler (e.g., g++ or clang).
4. Run the executable generated after compilation.

## Usage
- Launch the program with command-line arguments specifying the number of metaballs and the size of the cube for the marching algorithm.
- Example command: `./program 3 0.15`.

## Features
- **Marching Cubes Algorithm**: Generates a mesh for metaballs based on an isosurface.
- **Skybox Rendering**: Implements a skybox that encompasses the entire scene, adding depth and context to the rendering.
- **Metaball Reflections**: Metaballs reflect the skybox, enhancing visual realism.
- **Camera Control**: Navigate through the scene using keyboard and mouse inputs.
- **Dynamic Metaball Updates**: Metaballs are updated in real-time, demonstrating dynamic behavior.
- **Custom Shader Support**: Utilizes vertex and fragment shaders for rendering the skybox and metaballs.
- **Error Handling**: Checks for errors during the setup phase and provides informative messages.
- **Performance Optimization**: Implements frame rate control and efficient OpenGL practices.

## Controls
- **Arrow Keys**: Move the camera left, right, up, or down.
- **W Key**: Toggle wireframe mode.
- **Mouse Movement**: Adjust the camera's orientation.
- **Esc or Q Key**: Exit the program.

## Code Structure
- `SDLGraphicsProgram.hpp`: Main application class for initializing and running the graphics program.
- `Camera.hpp`: Camera class for 3D navigation.
- `Shader.hpp`: Shader class for handling vertex and fragment shaders.
- `MarchingCubes.hpp`: Implementation of the marching cubes algorithm for metaball mesh generation.
- `skybox.hpp`: Skybox class for creating and rendering a skybox.
