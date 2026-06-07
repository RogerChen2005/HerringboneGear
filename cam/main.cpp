#include "cam_generate.h"
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

    auto rough = generateRoughing(g, 3, 2.0, 2.0, 0.5);
    rough.WriteToFile("rough.nc");
    std::cout << "NC file written: rough.nc\n";

    auto finish = generateFinishing(g, g.z);
    finish.WriteToFile("finish.nc");
    std::cout << "NC file written: finish.nc\n";

    return 0;
}
