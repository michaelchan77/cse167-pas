# HW 2 Submission 11/1/23
2_1 Bonus: Triangle Clipping
1. Clipping implemented in lines 50-115 of "src/hw2.cpp"
2. Output images for 3 test scenes in "hw_2_outputs/hw_2_1_clipping*.png"

2_4 Bonus: Teapot Animation
1. Json files in "scenes/hw2/teapot_f*.json"
2. Animation in "hw_2_outputs/hw_2_4_animated_teapot.gif"

2_5 Design: Failed Person Scene
1. Json and blender-exported ply in "scenes/hw2/failed_person*"
2. Output image in "hw_2_outputs/hw_2_5_failed_person.png"

---
# balboa
UCSD CSE 167 codebase
https://cseweb.ucsd.edu/~tzli/cse167/

# Build
All the dependencies are included. Use CMake to build.
```
git clone --recurse-submodules https://github.com/BachiLi/balboa_public mkdir build
cd build
cmake ..
make -j
```
It requires compilers that support C++17 (gcc version >= 8, clang version >= 7, Apple Clang version >= 11.0, MSVC version >= 19.14).

---
### For Windows users:
1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) and [Visual Studio Code](https://code.visualstudio.com/). Choose "Desktop development with C++" and check "C++ CMake tools for Windows" on the recipe. 
2. `git clone --recurse-submodules https://github.com/BachiLi/balboa_public`
3. `cd balboa_public; mkdir build; cd build`
4. `cmake ..`
5. At this point, you can either open up the `balboa.sln` in `build` folder and use Visual Studio as your IDE, or work with Visual Studio Code. 

(optional) To build, run, and debug in VS Code: 

1. In the bottom bar, configure CMake as follows (from left to right): 
    - Build variant: `RelWithDebInfo`. 
    - Active kit: `Visual Studio ... x64`, choose the appropriate one depending on your architecture. 
    - Default build target: `balboa`. 
2. Create a `launch.json` in `.vscode` folder: 
    ```
    {
        "version": "0.2.0",
        "configurations": [
            {
                "name": "hw1_1", 
                "type": "cppvsdbg", 
                "request": "launch", 
                "program": "${workspaceFolder}/build/${command:cmake.buildType}/balboa.exe",
                "cwd": "${workspaceFolder}/build/${command:cmake.buildType}", 
                "args": ["-hw", "1_1"]
            }
        ]
    }
    ```
    You can pass arguments using `args`. 
3. Press `Ctrl+Shift+D` to start the debugger. You can set breakpoints, inspect variables, and trace callstack, etc. 

# Run
Try 
```
cd build
./balboa -hw 1_1
```
This will generate an image "hw1_1.png".

# Acknowledgement
stb\_image, stb\_image\_write, json, tinyply, GLFW, and glad

We use [stb_image](https://github.com/nothings/stb) and [stb_image_write](https://github.com/nothings/stb) for reading & writing images.

We use [json](https://github.com/nlohmann/json) to parse JSON files.

We use [tinyply](https://github.com/ddiakopoulos/tinyply) for parsing PLY files.

We use [glfw](https://www.glfw.org/) and [glad](https://glad.dav1d.de/) for the OpenGL homeworks.

3D models and textures: [Stanford 3D Scanning Repository](https://graphics.stanford.edu/data/3Dscanrep/), [KickAir_8p](https://blenderartists.org/t/uv-unwrapped-stanford-bunny-happy-spring-equinox/1101297), and [Texturemontage](http://kunzhou.net/tex-models.htm).
