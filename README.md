# fViz2D
**Simple 2D data renderer using Dear imGui with docking and GLFW3, for Windows and Linux.**

WIP: the goal is to be able to receive data and texture quads with it in real-time, with an interface to arrange, show/hide, scale, rotate and flip the layers, etc, as well as a simplified API to add hooks to develop applications capable of controlling whatever is creating said data. Should also have support to serialize, save and load its state.

Current state: 
- Renders a single data field (with support for different kinds of fields). 
- Can translate scalars to simple color schemes (for now, on the CPU - shader support will come later).
- Simple image saving (via GUI and as an utility function, no gif support yet).
- Custumizable GUI menus (pre-compilation, support for runtime hook management in future version).
- All in all, a minimal viable version of the system.
- Next updates will focus on maintenance, tests and docummentation.

## Requirements:

x86_64 platform, running **64 bit Windows or Linux** (mainly tested on Fedora 38 with KDE Plasma and on Windows 7 Professional, but should work on others).

Needs **OpenGL 3.3** support and dev package. Other dependencies are included (see notes bellow). Using clang is prefferable.

## Compilation notes:

Run one of the included batch or bash scripts or run premake5 on the solutions's root folder  to create a makefile or a VS solution (or something else, but these are the tested options).
- Eg: run "./premake5 gmake" on linux or run generateProjects_VS2019.bat on windows.

Premake is set, at the workspace level, to use **clang** - changing it to use something else should be ok, but is not actively tested.

In order to compile, your compiler needs a copy of **gl.h** it can see. On Windows, just having the necessary drivers should be enough. On Linux, you might need to look for your distribution's OpenGL dev package (mesa or libglvnd should do, possibly "-devel" versions).

The compiler also needs access to **64 bit GLFW 3 library** and header files. For windows, the needed files are already in the depend/glfw3_x64/GLFW/lib/ folder and set in the build system. For Linux, you may need to install something like **glfw-devel** through your package manager.

For Linux, premake is set to send shared libraries to **usr/local/lib64**. You can change that by changing  the **linSharedLibDest_x86_64 variable in the premake file**. Since this may copy into a protected area, you may need to run make with **sudo**.
