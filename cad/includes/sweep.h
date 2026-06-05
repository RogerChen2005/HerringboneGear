#pragma once

#include "profile.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// Helical sweep of a 2D profile into one half of the herringbone gear.
//
// z_sign = +1 → z grows from centre (0) to +F
// z_sign = −1 → z grows from centre (0) to −F
//
// Both halves use the SAME twist magnitude, producing the V-shape.
vtkSmartPointer<vtkPolyData> sweepHalf(
    const Profile& profile, const GearParams& g, double z_sign);
