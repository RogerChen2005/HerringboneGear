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
#include "gear_geometry.h"
#include "herringbone_gear.h"
#include "stock.h"
#include <iostream>

int main()
{
    GearParams g;

    if (loadFromJson(g, "gear.json"))
        std::cout << "Loaded parameters from gear.json\n";
    else
        std::cout << "Using compiled defaults (no gear.json found)\n";

    std::string err = gear::Validate(g);
    if (!err.empty()) {
        std::cerr << "Invalid parameters: " << err << "\n";
        return 1;
    }

    createHerringboneGear(g);
    createStock(g);

    return 0;
}
