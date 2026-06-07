# Herringbone Gear CAD/CAM

Parametric herringbone (double-helical) gear generator with CAM toolpath output and a Qt/VTK GUI for 3D preview.

## Project Structure

```
gear/
├── CMakeLists.txt              # Top-level build (C++17)
├── README.md
│
├── shared/                     # Static library: Gear::Params
│   ├── CMakeLists.txt
│   ├── includes/
│   │   ├── gear_params.h       # GearParams struct (z, m, alpha, beta, F, Kt, Ka, Kr, Kz)
│   │   └── gear_geometry.h     # Involute profile computation
│   └── src/
│       ├── gear_params.cpp
│       └── gear_geometry.cpp
│
├── cad/                        # Static library: Gear::CAD_Core + HerringboneGearCAD executable
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── includes/
│   │   ├── herringbone_gear.h  # buildGearMesh(), createHerringboneGear()
│   │   ├── stock.h             # buildStockMesh(), createStock()
│   │   └── sweep.h             # sweepHalf() — helical extrusion
│   └── src/
│       ├── herringbone_gear.cpp
│       ├── stock.cpp
│       └── sweep.cpp
│
├── cam/                        # Static library: Gear::CAM_Core + HerringboneGearCAM executable
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── includes/
│   │   ├── nc_converter.h      # HEIDENHAIN NC format writer
│   │   ├── roughing.h          # RoughingCut — layer-by-layer roughing
│   │   ├── finishing.h         # FinishingCut — profile-following finish
│   │   └── cam_generate.h      # generateRoughing(), generateFinishing()
│   └── src/
│       ├── nc_converter.cpp
│       ├── roughing.cpp
│       ├── finishing.cpp
│       └── cam_generate.cpp
│
└── ui/                         # Qt5 GUI: GearUI executable
    ├── CMakeLists.txt
    ├── includes/
    │   └── mainwindow.h
    └── src/
        ├── main.cpp
        ├── mainwindow.cpp
        ├── geometry_handlers.cpp   # "Generate Geometry" — CAD preview with smoothing
        └── cam_handlers.cpp        # "Generate CAM Code" — NC file output
```

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| CMake | ≥ 3.12 | Build system |
| VTK | 9.x | 3D mesh generation, rendering |
| Qt5 | 5.15+ | GUI framework |
| nlohmann_json | any | JSON parameter file parsing |

### Ubuntu / Debian install

```bash
sudo apt install cmake libvtk9-dev libvtk9-qt-dev qtbase5-dev libqt5opengl5-dev nlohmann-json3-dev
```

## Build

```bash
cmake -B build -S .
cmake --build build -j$(nproc)
```

### Outputs

| Binary | Description |
|--------|-------------|
| `build/ui/GearUI` | GUI application |
| `build/cad/HerringboneGearCAD` | CLI — writes `herringbone_gear.stl` + `gear_stock.stl` |
| `build/cam/HerringboneGearCAM` | CLI — writes `rough.nc` + `finish.nc` |

## Runtime

### CLI

Both CLI executables read optional `gear.json` in the working directory to override defaults:

```json
{
  "z": 24,
  "m": 10,
  "beta": 20,
  "F": 80
}
```

```bash
cd build
./cad/HerringboneGearCAD    # → herringbone_gear.stl, gear_stock.stl
./cam/HerringboneGearCAM    # → rough.nc, finish.nc
```

### GUI

```bash
./build/ui/GearUI
```

- **Left panel** — edit gear parameters (teeth, module, helix angle, etc.)
- **Generate Geometry** — builds the herringbone gear + stock cylinder and displays them in the 3D viewport (smoothed for preview)
- **Generate CAM Code** — writes HEIDENHAIN-format `rough.nc` and `finish.nc`
- **3D viewport** — mouse rotate / pan / zoom (trackball interaction)

### Gear Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| `z` | 36 | Number of teeth |
| `m` | 15 mm | Module (tooth size) |
| `alpha` | 20° | Pressure angle |
| `beta` | 25° | Helix angle per half |
| `F` | 100 mm | Half face width (total width = 2F) |
| `Kt` | 16 | Sample points per involute flank |
| `Ka` | 6 | Sample points per tip arc |
| `Kr` | 8 | Sample points per root fillet |
| `Kz` | 28 | Axial slices per half |
