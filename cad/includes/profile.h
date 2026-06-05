#pragma once

#include "gear_params.h"
#include <vector>
#include <utility>

using Profile = std::vector<std::pair<double, double>>;

// Compute 2D involute gear cross-section (all z teeth).
//
// Key circles:
//   pitch radius    r  = m*z/2
//   base circle     rb = r*cos(α)
//   addendum circle ra = r + m
//   dedendum circle rd = r - 1.25*m
//
// Per-tooth CCW traversal order:
//   left flank (t: root→tip) → tip arc → right flank (t: tip→root) → root arc
Profile computeProfile(const GearParams& g);
