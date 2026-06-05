#pragma once

#include "nc_converter.h"

// Roughing pass — layer-by-layer zigzag, removes bulk material.
void GenerateRoughing(NCConverter& nc, const GearParams& g);
