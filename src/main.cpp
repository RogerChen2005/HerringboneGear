// herringbone_gear.cpp
// Generates a herringbone (double-helical) gear and exports as STL.
//
// Pipeline:
//   1. Compute 2D involute tooth cross-section (all z teeth)
//   2. Helical sweep → front half  (z: 0 → +F)
//   3. Helical sweep → back half   (z: 0 → -F, same twist direction = V-shape)
//   4. Merge both halves
//   5. Triangulate + merge duplicate vertices + fill open end caps
//   6. Export binary STL

#include "gear_params.h"
#include "herringbone_gear.h"
#include "stock.h"

int main()
{
    GearParams g;

    createHerringboneGear(g);
    createStock(g);

    return 0;
}
