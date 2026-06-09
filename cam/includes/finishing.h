#pragma once

#include "nc_converter.h"
#include "gear_geometry.h"
#include "point.h"

class FinishingCut {
public:
    explicit FinishingCut(const GearParams& params);
    ~FinishingCut();

    NCConverter& Generate(int teeth_count);

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
    double reverse_;
    double twist_;
    
    void FinishRoot(const double base);
    void CutAcross(const Point& p);

    // void FinishCurve(const double base);
};
