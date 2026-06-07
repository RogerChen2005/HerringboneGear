#pragma once

#include "nc_converter.h"

class RoughingCut {
public:
    RoughingCut(const GearParams& params, double layer_depth, double cutter_diameter, double remain);
    ~RoughingCut();

    NCConverter& Generate(int teeth_count);

private:
    NCConverter nc_;
    GearParams params_;
    double depth_;
    double d_cutter_;
    double remain_;

    void RoughTooth(const double base);
    void RoughLayer(const double radius, const double start, const double end);
};
