#include "roughing.h"
#include "gear_geometry.h"
#include <cmath>

RoughingCut::RoughingCut(const GearParams& params, const RoughParams& cfg)
    : ToolpathPass(params), cfg_(cfg)
{
}

void RoughingCut::CutTooth(const double base) {
    for (double rc = derived_.ra ; rc >= derived_.rd; rc -= cfg_.layer_depth) {
        double phi  = (rc > derived_.rb) ? GearDerived::inv(sqrt((rc / derived_.rb) * (rc / derived_.rb) - 1.0)) :
                 gear::calc_theta(rc, derived_.rd, params_.Rg) - gear::calc_theta(derived_.rb, derived_.rd, params_.Rg);
        double remain_phi = (cfg_.remain + cfg_.cutter_diameter / 2) / std::cos(params_.beta) / rc;
        double start = base + derived_.phi0 - phi + remain_phi;
        double end = base + derived_.theta_half * 4 - derived_.phi0 + phi - remain_phi;
        RoughLayer(rc, start, end);
    }

    nc_.BlankLine();
}

void RoughingCut::RoughLayer(const double radius, const double start, const double end) {
    nc_.RapidLine(radius * std::sin(start), radius * std::cos(start), (params_.F + cfg_.cutter_diameter) * 2, -90, start);
    double d_phi = cfg_.cutter_diameter / radius;
    const int t = static_cast<int>((end - start) / d_phi);
    Point p = Point::fromPolar(radius, start);
    for (int i = 0;i <= t;i++) CutAcross(p.rotated(i * d_phi), cfg_.cutter_diameter / 2);
    CutAcrossFinal(p.rotated(end - start), cfg_.cutter_diameter / 2);
    if(reverse_) CutAcross(p.rotated(end - start), cfg_.cutter_diameter / 2);
    nc_.RapidLine(radius * std::sin(end), radius * std::cos(end), (params_.F + cfg_.cutter_diameter) * 2, -90, end);
}
