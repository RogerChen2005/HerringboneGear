#pragma once

#include "gear_params.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// Build the herringbone gear mesh (profile → sweep → triangulate → fill caps).
// Returns the result without writing to disk.
vtkSmartPointer<vtkPolyData> buildGearMesh(const GearParams& g);

void createHerringboneGear(const GearParams& g);