# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

WeirdMath is a small Qt 6 Widgets (C++17) desktop app that draws Sierpinski-style fractals with the "chaos game". It is a port of an older C++Builder/VCL program, and much of the code keeps that program's behaviour on purpose. Comments that mention VCL (`clRed`, `Canvas->Ellipse`, the "default:" branch of the original switch) describe the original, and those details should stay unless a change is asked for.

## Build & run

The project is normally built from Qt Creator. The existing build directory is `build/Desktop_Qt_6_12_0_Debug` (Unix Makefiles, Qt at `/home/wiesiek/Qt/6.12.0/gcc_64`).

```bash
# Configure (only needed for a fresh build dir)
cmake -S . -B build/Desktop_Qt_6_12_0_Debug -DCMAKE_PREFIX_PATH=/home/wiesiek/Qt/6.12.0/gcc_64 -DCMAKE_BUILD_TYPE=Debug
# Build
cmake --build build/Desktop_Qt_6_12_0_Debug
# Run
./build/Desktop_Qt_6_12_0_Debug/WeirdMath
```

There are no tests and no lint configuration.

## Architecture

- `ChaosCanvas` (QWidget) owns all the drawing. It renders into an off-screen `QImage` (`m_image`), so the picture survives repaints, and `paintEvent` only blits that image. `resizeEvent` copies the old image into a new black one of the new size. `runChaosGame(vertices, colors, iterations)` draws vertex markers, then repeatedly jumps to the integer midpoint between the current point and a random vertex. Each point gets the colour of the vertex it jumped towards (`colors[i]` belongs to `vertices[i]`). Left-clicks collect up to `MaxUserVertices` (11) user vertices and emit `userVertexAdded` or `userVertexRejected`.
- `MainWindow` (QWidget, not QMainWindow) builds the layout in code (there are no `.ui` files): the canvas, a "Your Figure" button next to a 3-line read-only log, and a row of buttons for the preset figures. It holds the colour palette (`vclColors()`, the VCL colour order) and the "Points" spin box (`m_iterations`, default 3M), which sets the iteration count for every figure. For preset figures, `figureColors(n)` uses the first n-1 palette colours and always makes the last vertex blue.
- Resources: `resources/weirdmath.ico` is embedded through `qt_add_resources` (`:/resources/weirdmath.ico`) for the window icon. `weirdmath.rc` is added only on Windows, for the executable icon.
- When adding source files, list them in `qt_add_executable` in `CMakeLists.txt`.
