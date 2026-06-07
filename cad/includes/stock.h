#pragma once

#include "gear_params.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// Build the stock cylinder mesh.
// Returns the result without writing to disk.
vtkSmartPointer<vtkPolyData> buildStockMesh(const GearParams& g);

void createStock(GearParams g);