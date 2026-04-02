# Lorenz Attractor Simulation with Raylib

A real-time 3D visualization of the Lorenz attractor, a classic chaotic system, built with Raylib. This simulation features 100 colorful particles following chaotic trajectories with smooth trails and a fully interactive 3D freelook camera system.

## Features

- **Real-time Lorenz Attractor Simulation**: Visualizes the classic chaotic system with accurate differential equations
- **100 Colorful Particles**: Each particle follows a unique chaotic trajectory with smooth color-coded trails
- **Interactive 3D Freelook Camera**: Full FPS-style camera controls for exploring the 3D space
- **Multiple Control Schemes**: Keyboard and mouse controls for intuitive navigation
- **Cross-platform**: Build for desktop (Windows/Linux) and web (WebAssembly)
- **Dynamic Trail System**: Particles leave fading trails that show the path of chaotic motion
- **Real-time UI**: On-screen display of simulation time, particle count, and control instructions

## Controls

### Camera Movement
- **W/S or Arrow Up/Down**: Move forward/backward
- **A/D or Arrow Left/Right**: Move left/right
- **SPACE/C or Page Up/Down**: Move up/down
- **Mouse Wheel**: Zoom in/out
- **Hold Right Mouse Button**: Enable freelook mode (FPS-style rotation)
- **R**: Reset camera to default position
- **SHIFT**: Hold to increase movement speed

### Simulation Controls
- **R** (while freelook is not active): Reset all particles to new random starting positions

## Building

### Desktop (CMake)

From the project root directory:

```bash
# Configure and build
cmake -S . -B build
cmake --build build

# Run the executable (Windows)
./build/raylib_lorenz.exe

# On Linux/Mac
./build/raylib_lorenz
```

The build system automatically:
- Creates a statically linked executable
- Packages data files (fonts, audio) with the executable
- Generates a zip file (`raylib_lorenz.zip`) in the build directory for distribution

### Web (Emscripten)

Prerequisites: Install [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) and ensure Raylib is built for WebAssembly.

```bash
# Windows
./build_web.sh

# Or manually with emcc
emcc src/*.cpp -o web-build/index.html \
  -I/path/to/raylib/src \
  libraylib.web.a \
  -DPLATFORM_WEB \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s TOTAL_MEMORY=16777216 \
  -s FORCE_FILESYSTEM=1 \
  --preload-file data@/data \
  --shell-file custom_shell.html
```

The web build will create a `web-build` directory with the HTML/JavaScript/WASM files, and start a local HTTP server on port 8000.

## Project Structure

```
raylib_lorenz/
├── src/
│   └── main.cpp          # Main simulation code (Lorenz equations, camera, rendering)
├── data/
│   ├── PressStart2P-Regular.ttf  # UI font
│   ├── music.mp3         # Background music
│   └── action.mp3        # Sound effects
├── lib/
│   ├── libgcc_s_dw2-1.dll # MinGW runtime libraries
│   └── libstdc++-6.dll   # C++ standard library
├── CMakeLists.txt        # Desktop build configuration
├── build_web.sh          # Web build script (Emscripten)
├── run_web.bat           # Windows web build helper
├── custom_shell.html     # Custom HTML shell for web build
└── libraylib.web.a       # Raylib WebAssembly library
```

## Technical Details

### Lorenz Equations
The simulation solves the classic Lorenz system of differential equations:
```
dx/dt = σ(y - x)
dy/dt = x(ρ - z) - y
dz/dt = xy - βz
```
Where σ = 10.0, ρ = 28.0, β = 8/3 (standard chaotic parameters).

### Camera System
The 3D camera implements:
- Quaternion-free yaw/pitch rotation for freelook mode
- Smooth interpolation between camera modes
- Collision-free movement in 3D space
- Proper handling of gimbal lock prevention

### Rendering Features
- 3D particle spheres with 2D screen-space overlays for visibility
- Alpha-blended trail lines that fade with age
- Coordinate axes visualization (optional)
- Custom UI with system information

## Notes

- The Lorenz attractor is a classic example of deterministic chaos - small changes in initial conditions lead to dramatically different trajectories over time.
- Each particle starts with slightly different initial positions, creating a beautiful visualization of the butterfly effect.
- The simulation uses Euler integration for simplicity; more accurate integrators (like Runge-Kutta) could be implemented for higher precision.
- The web version requires a modern browser with WebAssembly and WebGL 2.0 support.

## License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

Copyright (c) 2026 Adrian Mirea

## Acknowledgments

- [Raylib](https://www.raylib.com/) for the excellent graphics library
- The Lorenz attractor mathematical model developed by Edward Lorenz
- Emscripten for making C++ web compilation possible