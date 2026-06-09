#pragma once

#include "nc_converter.h"

// Generate a roughing pass for the given number of teeth.
// Returns the NCConverter with the toolpath (no header/footer).
NCConverter generateRoughing(const GearParams& g, int teeth_count,
                             double layer_depth, double cutter_diameter,
                             double remain);

// Generate a finishing pass for all teeth.
// Returns the NCConverter with the toolpath (no header/footer).
NCConverter generateFinishing(const GearParams& g, int teeth_count,
                             double layer_depth, double cutter_diameter,
                             double remain, double h_cutter, double Ra);
