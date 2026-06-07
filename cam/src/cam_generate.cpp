#include "cam_generate.h"
#include "roughing.h"
#include "finishing.h"

NCConverter generateRoughing(const GearParams& g, int teeth_count,
                             double layer_depth, double cutter_diameter,
                             double remain)
{
    NCConverter nc(g);
    RoughingCut roughCut(g, layer_depth, cutter_diameter, remain);
    nc.ProgramHeader();
    nc += roughCut.Generate(teeth_count);
    nc.ProgramFooter();
    return nc;
}

NCConverter generateFinishing(const GearParams& g, int teeth_count)
{
    NCConverter nc(g);
    FinishingCut finishCut(g);
    nc.ProgramHeader();
    nc += finishCut.Generate(teeth_count);
    nc.ProgramFooter();
    return nc;
}
