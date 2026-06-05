#include "nc_converter.h"
#include <iostream>

int main()
{
    GearParams g;

    if (loadFromJson(g, "gear.json"))
        std::cout << "Loaded parameters from gear.json\n";
    else
        std::cout << "Using compiled defaults (no gear.json found)\n";

    std::cout << "Generating CAM toolpath for herringbone gear...\n";
    std::cout << "  Teeth:       " << g.z << "\n";
    std::cout << "  Module:      " << g.m << " mm\n";
    std::cout << "  Helix angle: " << g.beta << " deg\n";
    std::cout << "  Face width:  " << 2 * g.F << " mm\n";

    NCConverter converter(g);
    converter.Generate("herringbone_gear.nc");

    std::cout << "NC file written: herringbone_gear.nc\n";
    return 0;
}
