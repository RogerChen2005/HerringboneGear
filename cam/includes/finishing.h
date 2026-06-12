#pragma once

#include "toolpath_pass.h"

// Finishing pass configuration.
struct FinishParams {
    double layer_depth     = 0.2;   // radial depth per layer (mm)
    double cutter_diameter = 4.0;   // mm
    double remain          = 0.5;   // stock left by roughing (mm)
    int    teeth_count     = 1;     // number of teeth to cut
    double h_cutter        = 25.0;  // cutter height (mm)
    double Ra              = 0.064; // target surface roughness (mm)
    int    tool_number     = 60;    // NC tool number (TOOL CALL)
};

class FinishingCut : public ToolpathPass {
public:
    FinishingCut(const GearParams& params, const FinishParams& cfg);

private:
    enum Direction { Left, Right };

    FinishParams cfg_;
    double theta_;

    void CutTooth(double base) override;
    const char* PassName() const override { return "Finishing"; }

    void FinishRoot(double base, double dist);
    void FinishCorner(double base, double dist, Direction direction);
    void FinishCurve(double base, double dist, Direction direction);
    double CalcG(double Ra, double R) const;

    void MillAcross(const Point& p, double phi, const double r_cutter);
};
