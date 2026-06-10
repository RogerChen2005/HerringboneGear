#include "cam_generate.h"
#include "gear_geometry.h"
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

    std::cout << "Generating CAM toolpath for herringbone gear...\n";
    std::cout << "  Teeth:       " << g.z << "\n";
    std::cout << "  Module:      " << g.m << " mm\n";
    std::cout << "  Helix angle: " << g.beta << " deg\n";
    std::cout << "  Face width:  " << 2 * g.F << " mm\n";

    RoughParams rp;
    rp.layer_depth = 2.0;
    auto rough = generateRoughing(g, rp);
    rough.WriteToFile("rough.nc");
    std::cout << "NC file written: rough.nc\n";

    FinishParams fp;
    auto finish = generateFinishing(g, fp);
    finish.WriteToFile("finish.nc");
    std::cout << "NC file written: finish.nc\n";

    return 0;
}
