#pragma once

#include "nc_converter.h"

class RoughingCut {
public:
    RoughingCut(const GearParams& params);
    RoughingCut(const GearParams& params, double layer_depth, double cutter_diameter, double remain);
    ~RoughingCut();

    NCConverter& Generate(int teeth_count);

    // Getters
    double GetDepth() const { return depth_; }
    double GetRemain() const { return remain_; }
    double GetCutterDiameter() const { return d_cutter_; }

    // Setters
    void SetDepth(double depth) { depth_ = depth; }
    void SetRemain(double remain) { remain_ = remain; }
    void SetCutterDiameter(double d) { d_cutter_ = d; }

private:
    NCConverter nc_;
    GearParams params_;
    double depth_;
    double d_cutter_;
    double remain_;

    void RoughTooth(const double base);
    void RoughLayer(const double radius, const double start, const double end);
};
