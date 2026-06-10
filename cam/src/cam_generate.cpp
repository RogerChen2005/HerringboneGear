#include "cam_generate.h"

NCConverter generateRoughing(const GearParams& g, const RoughParams& p)
{
    NCConverter nc(g);
    RoughingCut roughCut(g, p);
    nc.ProgramHeader(1);
    nc += roughCut.Generate(p.teeth_count);
    nc.ProgramFooter();
    return nc;
}

NCConverter generateFinishing(const GearParams& g, const FinishParams& p)
{
    NCConverter nc(g);
    FinishingCut finishCut(g, p);
    nc.ProgramHeader(60);
    nc += finishCut.Generate(p.teeth_count);
    nc.ProgramFooter();
    return nc;
}
