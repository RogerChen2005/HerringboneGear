#include "roughing.h"
#include "gear_geometry.h"
#include <iostream>
#include <cmath>
#include <string>

RoughingCut::RoughingCut(const GearParams& params, double layer_depth, double cutter_diameter, double remain)
    : params_(params), nc_(params), depth_(layer_depth), d_cutter_(cutter_diameter), remain_(remain) {}

RoughingCut::~RoughingCut() = default;

void RoughingCut::RoughTooth(const double base) {
    const double deg = M_PI / 180.0;
    double r  = params_.m * params_.z / 2.0;
    double rb = r  * cos(params_.alpha * deg);
    double ra = r  + params_.m;
    double rd = r  - 1.25 * params_.m;
    
    auto inv = [](double t) { return t - atan(t); };

    double inv_pc     = inv(params_.alpha * deg);
    double theta_half = M_PI / (2.0 * params_.z);
    double phi0       = theta_half + inv_pc;

    for (double rc = ra ; rc >= rd; rc -= this->depth_) {
        double t  = (rc > rb) ? sqrt((rc / rb) * (rc / rb) - 1.0) : 0.0;
        double remain_phi = remain_ / rc;
        double start = base + phi0 - inv(t) + remain_phi;
        double end = base + M_PI / params_.z * 2 - phi0 + inv(t) - remain_phi;
        RoughLayer(rc, start, end);
    }

    nc_.BlankLine();
}

void RoughingCut::RoughLayer(const double radius, const double start, const double end) {
    const double twist = gear::TwistAngle(params_, params_.F);
    nc_.RapidLine(radius * std::sin(start), radius * std::cos(start), (params_.F + d_cutter_) * 2, -90, start);
    double d_phi = d_cutter_ / radius;
    const int t = static_cast<int>((end - start) / d_phi);
    bool reverse = false;
    double phi1 = 0, phi2 = 0;
    for (int i = 0;i < t;i++) {
        phi1 = start + d_phi * i;
        phi2 = phi1 + twist;
        nc_.CutLine(radius * std::sin(phi1), radius * std::cos(phi1), params_.F * (reverse ? 0 : 2), -90, -gear::RadToDeg(phi1));
        nc_.CutLine(radius * std::sin(phi2), radius * std::cos(phi2), params_.F, -90, -gear::RadToDeg(phi2));
        nc_.CutLine(radius * std::sin(phi1), radius * std::cos(phi1), params_.F * (reverse ? 2 : 0), -90, -gear::RadToDeg(phi1));
        reverse = !reverse;
    }
    if(reverse) {  
        nc_.RapidLine(radius * std::sin(phi1), radius * std::cos(phi1), params_.F * 0, -90, -gear::RadToDeg(phi1));
        nc_.RapidLine(radius * std::sin(phi2), radius * std::cos(phi2), params_.F, -90, -gear::RadToDeg(phi2));
        nc_.RapidLine(radius * std::sin(phi1), radius * std::cos(phi1), params_.F * 2, -90, -gear::RadToDeg(phi1)); 
    }
    nc_.RapidLine(radius * std::sin(end), radius * std::cos(end), (params_.F + d_cutter_) * 2, -90, end);
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

NCConverter& RoughingCut::Generate(int teeth_count)
{
    const double tooth_step = 2.0 * M_PI / params_.z;
    int cnt = teeth_count <= params_.z ? teeth_count : params_.z;
    nc_.ClearAll();
    nc_.Comment("=== Roughing PASS ===");
    for (int n = 0; n < cnt; ++n) {
        double base = n * tooth_step;
        RoughTooth(base);
    }
    return nc_;
}
