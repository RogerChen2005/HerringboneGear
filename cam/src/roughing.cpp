#include "roughing.h"
#include "gear_geometry.h"
#include "gear_derived.h"
#include <iostream>
#include <cmath>
#include <string>

RoughingCut::RoughingCut(const GearParams& params)
    : params_(params), nc_(params), derived_(params), depth_(3), d_cutter_(6.0), remain_(0.5),
    reverse_(false), twist_(gear::TwistAngle(params, params.F)) {}

RoughingCut::~RoughingCut() = default;

void RoughingCut::RoughTooth(const double base) {
    for (double rc = derived_.ra ; rc >= derived_.rd; rc -= this->depth_) {
        double phi  = (rc > derived_.rb) ? GearDerived::inv(sqrt((rc / derived_.rb) * (rc / derived_.rb) - 1.0)) :
                 gear::calc_theta(rc, derived_.rd, params_.Rg) - gear::calc_theta(derived_.rb, derived_.rd, params_.Rg);
        double remain_phi = (remain_ + d_cutter_ / 2) / rc;
        double start = base + derived_.phi0 - phi + remain_phi;
        double end = base + derived_.theta_half * 4 - derived_.phi0 + phi - remain_phi;
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
