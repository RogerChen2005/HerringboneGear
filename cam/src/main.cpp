#include "nc_converter.h"
#include "roughing.h"
#include "finishing.h"
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

    NCConverter rough(g);
    NCConverter finish(g);

    rough.ProgramHeader();
    GenerateRoughing(rough, g);
    rough.ProgramFooter();

    finish.ProgramHeader();
    GenerateFinishing(finish, g);
    finish.ProgramHeader();

    rough.WriteToFile("rough.nc");
    std::cout << "NC file written: rough.nc\n";

    finish.WriteToFile("finish.nc");
    std::cout << "NC file written: rough.nc\n";

    return 0;
}
