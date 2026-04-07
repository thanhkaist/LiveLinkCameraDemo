# LiveLink Camera Demo

A Qt-based desktop application that streams virtual camera data to **Unreal Engine** in real time using the [Unreal Live Link C Interface](https://github.com/ue4plugins/UnrealLiveLinkCInterface). The GUI lets you interactively control camera position, orientation, and cinematic properties, which are reflected instantly inside a connected Unreal Engine session.

---

## Features

- **Real-time camera transform control** — position (X / Y / Z) and orientation (Roll / Pitch / Yaw) driven by sliders and dials
- **Cinematic camera properties** — Field of View, Aspect Ratio, Focal Length, Aperture, Focus Distance, and Projection Mode
- **Configurable static camera capabilities** — toggle which properties are streamed per-frame via the *Set Camera Structure* panel
- **60-fps update loop** powered by a Qt timer (~16 ms interval)
- **Graceful shutdown** — prompts for confirmation and cleanly unloads the Live Link interface on exit
- **Cross-platform** — primary target is Windows (DLL); Linux is supported via a shared object (`.so`)

---

## Prerequisites

| Requirement | Notes |
|---|---|
| **Qt 5 or Qt 6** (with Widgets module) | Install via [Qt Online Installer](https://www.qt.io/download) or your system package manager |
| **C++11 compiler** | MSVC, MinGW, or GCC |
| **Unreal Engine** with the **Live Link** plugin enabled | Required to receive the streamed data |
| `UnrealLiveLinkCInterface.dll` / `.so` | Pre-built library shipped with the repo (Windows DLL included) |

---

## Building

### Using Qt Creator (recommended)

1. Open **Qt Creator**.
2. Choose **File → Open File or Project** and select `DemoCameraLiveLink.pro`.
3. Configure the kit (Qt version + compiler) and click **Configure Project**.
4. Build with **Build → Build Project** (`Ctrl+B`).

### Using qmake from the command line

```bash
# Create a build directory
mkdir build && cd build

# Generate Makefile
qmake ../DemoCameraLiveLink.pro

# Build
make          # Linux / macOS
mingw32-make  # Windows with MinGW
```

> **Windows note:** The pre-built `UnrealLiveLinkCInterface.dll` and its import library `UnrealLiveLinkCInterface.lib` are already in the project root and are linked automatically by the `.pro` file.

---

## Running

1. Start **Unreal Engine** and enable the **Live Link** plugin (`Edit → Plugins → Live Link`).
2. Open the **Live Link** panel (`Window → Live Link`) inside Unreal Engine.
3. Launch the **DemoCameraLiveLink** application.
4. The application registers a Live Link source named **`CameraControl`**. It should appear automatically in the Unreal Live Link panel.
5. A subject named **`KFXCamera`** streams camera data. Assign it to a Camera Actor in your scene through the Live Link component or the Virtual Camera workflow.
6. Use the sliders and dials to move the camera. Changes are reflected in Unreal Engine in real time.

---

## Usage Guide

### Camera Transform

| Control | Description |
|---|---|
| **Horizontal Sliders 6 / 7 / 8** | Camera X, Y, Z world translation |
| **Dial 1** | Roll (radians converted from dial degrees) |
| **Dial 2** | Pitch |
| **Dial 3** | Yaw |

Rotation values are converted from Euler angles (degrees) to a quaternion before being sent to Live Link.

### Camera Properties

| Control | Property |
|---|---|
| `sFieldOfView` | Field of View (degrees) |
| `sAspectRatio` | Aspect Ratio (slider value / 100) |
| `sFocalLength` | Focal Length (mm) |
| `sAperture` | Aperture f-stop (slider value / 100) |
| `sFocusDistance` | Focus Distance (cm) |
| `CbxIsPerspective` | Perspective (`true`) vs. Orthographic (`false`) |

### Camera Static Structure

Click **Set Camera Structure** to push which optional properties are active to Unreal Engine. The checkboxes correspond to:

- Is Field of View Supported
- Is Aspect Ratio Supported
- Is Focal Length Supported
- Is Projection Mode Supported
- Is Aperture Supported
- Is Focus Distance Supported

---

## Project Structure

```
LiveLinkCameraDemo/
├── main.cpp                          # Application entry point (Qt)
├── mainwindow.h / .cpp / .ui         # Main window: GUI layout and camera update logic
├── CirclingTransform.c               # Standalone C demo: circles a transform subject
├── DemoCameraLiveLink.pro            # Qt project file
│
│   ── Unreal Live Link C Interface ──
├── UnrealLiveLinkCInterface.h        # High-level C++ API declarations
├── UnrealLiveLinkCInterfaceAPI.h     # Low-level C function pointer API
├── UnrealLiveLinkCInterfaceAPI.c     # API implementation (load/unload DLL/SO)
├── UnrealLiveLinkCInterfaceTypes.h   # Data types (Transform, Camera, Light, Animation…)
├── UnrealLiveLinkCInterface.dll      # Pre-built Windows shared library
└── UnrealLiveLinkCInterface.lib      # Windows import library
```

### Key Data Types

| Type | Description |
|---|---|
| `UnrealLiveLink_Transform` | Position (3 floats) + rotation quaternion (4 floats) + scale (3 floats) |
| `UnrealLiveLink_Camera` | Transform + FOV, aspect ratio, focal length, aperture, focus distance, projection mode |
| `UnrealLiveLink_CameraStatic` | Booleans indicating which camera properties are streamed per-frame |
| `UnrealLiveLink_Light` | Transform + temperature, intensity, color, cone angles, radius |
| `UnrealLiveLink_Animation` | Bone hierarchy + per-frame transforms |

---

## Live Link C Interface API Version

The bundled library targets **API version 5** (`UNREAL_LIVE_LINK_API_VERSION 5` in `UnrealLiveLinkCInterfaceTypes.h`).

---

## Third-Party Credits

The `UnrealLiveLinkCInterface` C wrapper layer is based on work originally created by **Patrick Palmer** at **The Jim Henson Company**, released under the [MIT License](https://opensource.org/licenses/MIT).

---

## License

This project is provided as a demo. The bundled Unreal Live Link C Interface files retain their original MIT License (see file headers). Please ensure you comply with Unreal Engine's [EULA](https://www.unrealengine.com/eula) when deploying.
