#include "roughing.h"
#include "gear_geometry.h"
#include <iostream>
#include <cmath>
#include <string>

RoughingCut::RoughingCut(const GearParams& params)
    : params_(params), nc_(params), depth_(3), d_cutter_(6.0), remain_(0.5), 
    reverse_(false), twist_(gear::TwistAngle(params, params.F)) {}

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
        double phi  = (rc > rb) ? inv(sqrt((rc / rb) * (rc / rb) - 1.0)) :
                 gear::calc_theta(rc, rd, params_.Rg) - gear::calc_theta(rb, rd, params_.Rg);
        double remain_phi = (remain_ + d_cutter_ / 2) / rc;
        double start = base + phi0 - phi + remain_phi;
        double end = base + theta_half * 4 - phi0 + phi - remain_phi;
        RoughLayer(rc, start, end);
    }

    nc_.BlankLine();
}

void RoughingCut::RoughLayer(const double radius, const double start, const double end) {
    nc_.RapidLine(radius * std::sin(start), radius * std::cos(start), (params_.F + d_cutter_) * 2, -90, start);
    double d_phi = d_cutter_ / radius;
    const int t = static_cast<int>((end - start) / d_phi);
    Point p = Point::fromPolar(radius, start);
    for (int i = 0;i <= t;i++) CutAcross(p.rotated(i * d_phi));
    CutAcross(p.rotated(end - start));
    if(reverse_) CutAcross(p.rotated(end - start));
    nc_.RapidLine(radius * std::sin(end), radius * std::cos(end), (params_.F + d_cutter_) * 2, -90, end);
}

void RoughingCut::CutAcross(const Point& p) {
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(p.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(p.angle()));
    this->reverse_ = !this->reverse_;
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
