#pragma once

#include <string>

struct GearParams {
    int    z     = 36;    // number of teeth
    double m     = 15;    // module (mm)  — controls tooth size
    double alpha = 20.0;  // pressure angle (degrees), standard = 20°
    double beta  = 25.0;  // helix angle per half (degrees), typically 15–30°
    double F     = 100.0; // half face width (mm); total gear width = 2F
    int    Kt    = 16;    // sample points per involute flank
    int    Ka    = 6;     // sample points per tip arc
    int    Kr    = 8;     // sample points per root fillet arc
    int    Kz    = 28;    // axial slices per half (more = smoother helix)
};

// Load parameters from a JSON file.  Only keys present in the file are
// overwritten; the rest keep their compiled-in defaults.
//
// Example gear.json:
//   {
//     "z": 24,
//     "m": 10,
//     "beta": 20,
//     "F": 80
//   }
//
// Returns true on success, false on error (file not found / parse error).
bool loadFromJson(GearParams& params, const std::string& path);
