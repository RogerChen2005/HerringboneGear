#include "cam_generate.h"
#include "roughing.h"
#include "finishing.h"

NCConverter generateRoughing(const GearParams& g, int teeth_count,
                             double layer_depth, double cutter_diameter,
                             double remain)
{
    NCConverter nc(g);
    RoughingCut roughCut(g);
    roughCut.SetCutterDiameter(cutter_diameter);
    roughCut.SetDepth(layer_depth);
    roughCut.SetRemain(remain);
    nc.ProgramHeader(1);
    nc += roughCut.Generate(teeth_count);
    nc.ProgramFooter();
    return nc;
}

NCConverter generateFinishing(const GearParams& g, int teeth_count,
                             double layer_depth, double cutter_diameter,
                             double remain, double h_cutter, double Ra)
{
    NCConverter nc(g);
    FinishingCut finishCut(g);
    finishCut.SetCutterDiameter(cutter_diameter);
    finishCut.SetDepth(layer_depth);
    finishCut.SetRemain(remain);
    finishCut.SetCutterHeight(h_cutter);
    finishCut.SetRa(Ra);
    nc.ProgramHeader(60);
    nc += finishCut.Generate(teeth_count);
    nc.ProgramFooter();
    return nc;
}
