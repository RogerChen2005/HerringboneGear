#pragma once

#include "nc_converter.h"
#include "gear_derived.h"
#include "point.h"

class RoughingCut {
public:
    RoughingCut(const GearParams& params);
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
    GearDerived derived_;
    double depth_;
    double d_cutter_;
    double remain_;
    bool   reverse_;
    double twist_;

    void CutAcross(const Point& p);
    void RoughTooth(const double base);
    void RoughLayer(const double radius, const double start, const double end);
};
