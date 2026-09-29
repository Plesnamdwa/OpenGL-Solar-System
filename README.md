# 3D Solar System — OpenGL & GLFW

An interactive 3D simulation of the Solar System written in C++ with **OpenGL (fixed-function pipeline)** and **GLFW**. The Sun sits at the center and lights eight orbiting planets (Mercury to Neptune), each with its own procedurally generated surface. Click any planet to zoom in, follow it, and read a glassmorphism-style info panel with key facts. Every UI interaction is accompanied by a sound effect synthesized at runtime, so the project needs **no external image or audio assets**.

> **Platform:** Windows only (audio uses `PlaySoundA` from the Win32 API).

---

## Features

### 3D Graphics
- **Hierarchical transformations** — built with `glPushMatrix` / `glPopMatrix`:
  - *Revolution*: each planet orbits the Sun (`glRotatef` around the Y axis, then `glTranslatef` to its orbit distance).
  - *Rotation*: a nested matrix applies **axial tilt** (e.g. Uranus at 97.8°, Venus at 177.3° with retrograde spin) and then self-rotation.
  - *Moon*: Earth's Moon is a child of Earth's transform, orbiting on a slightly inclined plane.
- **Lighting** — a point light (`GL_LIGHT0`) placed at the Sun's position with ambient, diffuse, and specular components. Uses `GL_COLOR_MATERIAL` and per-vertex normals. The Sun uses a **material emission** term so it appears self-illuminated.
- **Procedural surface details** — instead of image textures, a per-vertex color function (`getSurfaceColor`) generates each surface from latitude/longitude math:
  - Mercury: craters and dark maria
  - Venus: swirling cloud layers
  - Earth: oceans, continents, deserts, polar ice, and animated clouds
  - Mars: canyons and polar caps
  - Jupiter: cloud bands and the Great Red Spot
  - Saturn: banded atmosphere
  - Uranus / Neptune: icy bands, plus Neptune's Great Dark Spot and cirrus clouds
  - Sun: animated solar flares and granulation
- **Planetary rings** — multi-band ring for Saturn (`GL_QUAD_STRIP`) and a thin ring for Uranus (`GL_LINE_LOOP`).
- **Scene extras** — orbit path lines and a background starfield of 800 randomly distributed stars.
- **Smooth camera system** — orbit camera (yaw/pitch) with exponential interpolation (lerp) for focus transitions and zoom.
- **Delta-time simulation** — adjustable simulation speed and pause, independent of frame rate.

### Interaction & UI
- **3D object picking** — planets are selected by projecting their 3D positions onto the screen (manual model-view/projection matrix math).
- **2D overlay HUD** rendered with an orthographic projection and alpha blending:
  - Glassmorphism info panel (diameter, distance, orbit period, rotation, moons, average temperature, key facts)
  - Floating name badges on hover / focus
  - Clickable "PRESS ESC TO STOP FOLLOWING" button
  - Custom **vector stroke font** (letters and digits drawn with `GL_LINES`, no font library required)

### Procedural Audio
- Seven sci-fi sound effects (hover, click, back, speed up, slow down, pause, resume) are synthesized from sine waves, envelopes, and noise, packed into in-memory WAV buffers, and played asynchronously via `PlaySoundA`.

---

## Dependencies

| Dependency | Purpose |
|---|---|
| **C++11** (or newer) compiler | Language features (`auto`, range-based `for`, etc.) |
| **[GLFW 3](https://www.glfw.org/)** | Window creation, OpenGL context, input callbacks |
| **OpenGL** (`opengl32`) | Fixed-function 3D rendering (ships with Windows) |
| **Windows API** (`windows.h`, `mmsystem.h`, linked with `winmm`) | Audio playback via `PlaySoundA` |

GLEW, GLM, and GLUT are **not** required.

Recommended toolchains: **MinGW-w64 (MSYS2)** or **Visual Studio (MSVC)**.

---

## How to Build

### Option A — MinGW-w64 via MSYS2 (recommended)

1. Install [MSYS2](https://www.msys2.org/), then open the **MSYS2 MinGW 64-bit** terminal.
2. Install the compiler and GLFW:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw
   ```
3. Compile:
   ```bash
   g++ tataSurya3D.cpp -o tataSurya3D.exe -std=c++11 -lglfw3 -lopengl32 -lgdi32 -lwinmm
   ```
4. Run:
   ```bash
   ./tataSurya3D.exe
   ```

### Option B — MinGW-w64 with manually downloaded GLFW

1. Download the **pre-compiled Windows binaries** from the [GLFW download page](https://www.glfw.org/download.html) (choose the folder matching your compiler, e.g. `lib-mingw-w64`).
2. Compile, pointing to your GLFW folders:
   ```bash
   g++ tataSurya3D.cpp -o tataSurya3D.exe -std=c++11 ^
       -I"path\to\glfw\include" -L"path\to\glfw\lib-mingw-w64" ^
       -lglfw3 -lopengl32 -lgdi32 -lwinmm
   ```
3. If the program complains about a missing DLL at launch, add `-static` or copy `glfw3.dll` next to the executable.

### Option C — Visual Studio (MSVC)

1. Download the GLFW pre-compiled binaries (`lib-vc20xx` folder).
2. In a **Developer Command Prompt**:
   ```bat
   cl /EHsc /std:c++14 tataSurya3D.cpp /I"path\to\glfw\include" ^
      /link /LIBPATH:"path\to\glfw\lib-vc2022" ^
      glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib winmm.lib
   ```
3. Run `tataSurya3D.exe`.

> **Troubleshooting:** if you get errors about `memcpy` or `uint8_t` not being declared, add `#include <cstring>` and `#include <cstdint>` at the top of the source file.

---

## Controls

### Mouse

| Action | Effect |
|---|---|
| **Hover** over a planet / the Sun | Shows its name badge (with a soft blip sound) |
| **Left click** a planet / the Sun | Zooms in and follows the object; opens the info panel |
| **Click & drag** | Rotates the camera around the current target |
| **Scroll wheel** | Zoom in / out |
| **Click** the bottom "PRESS ESC TO STOP FOLLOWING" button | Stops following and returns to the overview |

### Keyboard

| Key | Action |
|---|---|
| `0` | Focus on the **Sun** |
| `1` – `8` | Focus on **Mercury**, **Venus**, **Earth**, **Mars**, **Jupiter**, **Saturn**, **Uranus**, **Neptune** |
| `Esc` | Stop following and return to the Solar System overview |
| `R` | Reset the view (overview, default camera angle, and 1x speed) |
| `Space` | Pause / resume time |
| `→` or `=` | Speed up the simulation (×1.25 per press) |
| `←` or `-` | Slow down the simulation (÷1.25 per press, minimum 0.05x) |

---

## Notes

- Planet sizes and orbital distances are **stylized for visibility** and are not to scale. The data shown in the info panel (diameter, distance, orbital period, etc.) uses real approximate values.
- The info panel uses the built-in stroke font, which supports uppercase letters, digits, and a small set of punctuation (`: . , ( ) -`).
- Console output prints a control summary and event logs (focus changes, pause, speed) while the program is running.

---

## Project Structure

```
.
├── tataSurya3D.cpp   # Entire application (single-file project)
└── README.md
```
