#pragma once

#include "toolpath_pass.h"

// Roughing pass configuration.
struct RoughParams {
    double layer_depth     = 3.0;  // radial depth per layer (mm)
    double cutter_diameter = 6.0;  // mm
    double remain          = 0.5;  // stock left for finishing (mm)
    int    teeth_count     = 1;    // number of teeth to cut
};

class RoughingCut : public ToolpathPass {
public:
    RoughingCut(const GearParams& params, const RoughParams& cfg);

private:
    RoughParams cfg_;

    void CutTooth(double base) override;
    const char* PassName() const override { return "Roughing"; }

    void RoughLayer(double radius, double start, double end);
};
