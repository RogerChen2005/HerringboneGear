#pragma once

#include "nc_converter.h"
#include "gear_geometry.h"
#include "gear_derived.h"
#include "point.h"

enum Direction {
    Left,
    Right
};

class FinishingCut {
public:
    explicit FinishingCut(const GearParams& params);
    ~FinishingCut();

    NCConverter& Generate(int teeth_count);

    double GetDepth() const { return depth_; }
    double GetRemain() const { return remain_; }
    double GetCutterDiameter() const { return d_cutter_; }
    double GetCutterHeight() const { return h_cutter_; }
    double GetRa() const { return Ra_; }

    // Setters
    void SetDepth(double depth) { depth_ = depth; }
    void SetRemain(double remain) { remain_ = remain; }
    void SetCutterDiameter(double d) { d_cutter_ = d; }
    void SetCutterHeight(double h) { h_cutter_ = h; }
    void SetRa(double Ra) { Ra_ = Ra; }

private:
    NCConverter nc_;
    GearParams params_;
    GearDerived derived_;
    double depth_;
    double d_cutter_;
    double h_cutter_;
    double remain_;
    double Ra_;

    bool   reverse_;
    double twist_;
    double theta_;
    
    void FinishLayer(const double base);
    void FinishRoot(const double base, const double dist);
    void FinishCorner(const double base, const double dist, const Direction direction);
    void FinishCurve(const double base, const double dist, const Direction direction);
    double CalcG(const double Ra, const double R);

    void CutAcross(const Point& p);
    void MillAcross(const Point& p, const double phi);
};
