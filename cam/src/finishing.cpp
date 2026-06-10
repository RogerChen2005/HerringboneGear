#include "finishing.h"
#include "gear_geometry.h"
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

FinishingCut::FinishingCut(const GearParams& params, const FinishParams& cfg)
    : ToolpathPass(params), cfg_(cfg),
      theta_(gear::calc_theta(derived_.rb, derived_.rd, params.Rg))
{
}

void FinishingCut::FinishRoot(const double base, const double dist) {
    if (derived_.hasRootRelief()) FinishCorner(base, dist, Left);
    double start = base + derived_.phi0 + theta_;
    double end = base + derived_.theta_half * 4 - derived_.phi0 - theta_;
    double g_ = CalcG(cfg_.Ra, derived_.rd);
    if (start < end){
        double radius = derived_.rd + dist + cfg_.cutter_diameter / 2;
        double d_phi = g_ / radius;
        const int t = static_cast<int>((end - start) / d_phi);
        Point p = Point::fromPolar(radius, start);
        for (int i = 0;i <= t;i++) CutAcross(p.rotated(i * d_phi));
        CutAcross(p.rotated(end - start));
    }
    if (derived_.hasRootRelief()) FinishCorner(base, dist, Right);
}

void FinishingCut::FinishCorner(const double base, const double dist, const Direction direction){
    double phi = std::asin(std::clamp(sin(theta_) / params_.Rg * derived_.rb, -1.0, 1.0));
    double g_ = CalcG(cfg_.Ra, params_.Rg);
    double d_phi = g_ / params_.Rg;
    Point conner_center = Point::fromPolar(derived_.rd + params_.Rg,
        base + ((direction == Left) ? (derived_.phi0 + theta_) : (derived_.theta_half * 4 - derived_.phi0 - theta_)));
    int cnt = static_cast<int>(phi / d_phi);
    double r = params_.Rg - dist - cfg_.cutter_diameter / 2;
    for (int i = cnt;i >= 0;i--)
        CutAcross(conner_center.movedPolar(-r,
            conner_center.angle() + ((direction == Left) ? i * d_phi : (i - cnt) * d_phi)));
}

void FinishingCut::FinishCurve(const double base, const double dist, const Direction direction) {
    double arc = sqrt(derived_.ra * derived_.ra - derived_.rb * derived_.rb);
    double g_ = CalcG(cfg_.Ra, arc);
    int cnt = static_cast<int>(arc / g_) + 1;
    if ( direction == Left ) {
        double tb = base + derived_.phi0;
        double ta = tb - arc / derived_.rb;
        for (int i = 0;i <= cnt;i++){
            double height = cfg_.h_cutter - (cfg_.h_cutter + cfg_.cutter_diameter) * sqrt((double)i / cnt);
            double dt = (tb - ta) * i / cnt;
            double t = ta + dt;
            double len = arc - dt * derived_.rb;
            Point p = Point::fromPolar(derived_.rb - height, t);
            p.movePolar(len + dist + cfg_.cutter_diameter / 2, t + M_PI / 2);
            MillAcross(p, t);
        }
    }
    else {
        double tb = base + derived_.theta_half * 4 -  derived_.phi0;
        double ta = tb + arc / derived_.rb;
        for (int i = cnt;i >= 0;i--){
            double height = cfg_.h_cutter - (cfg_.h_cutter + cfg_.cutter_diameter) * sqrt((double)i / cnt);
            double dt = (ta - tb) * i / cnt;
            double t = ta - dt;
            double len = arc - dt * derived_.rb;
            Point p = Point::fromPolar(derived_.rb - height, t);
            p.movePolar(len + dist + cfg_.cutter_diameter / 2, t - M_PI / 2);
            MillAcross(p, t);
            if (i == 0 && reverse_) MillAcross(p, t);
        }
    }
}

void FinishingCut::CutTooth(const double base) {
    const int cnt = static_cast<int>(cfg_.remain / cfg_.layer_depth) + 1;
    for (int i = 1;i <= cnt;i++) {
        double t_tip  = sqrt((derived_.ra / derived_.rb) * (derived_.ra / derived_.rb) - 1.0);
        double inv_t = t_tip - atan(t_tip);
        nc_.RapidLine(Point::fromPolar(derived_.ra,base + derived_.phi0 - inv_t), params_.F * 2 + 10);
        double dist = cfg_.remain - cfg_.layer_depth * i ;
        dist = dist > 0 ? dist : 0;
        FinishCurve(base, dist, Left);
        FinishRoot(base, dist);
        FinishCurve(base, dist, Right);
        nc_.RapidLine(Point::fromPolar(derived_.ra,base + 4 * derived_.theta_half - derived_.phi0 + inv_t), params_.F * 2 + 10);
    }
}

void FinishingCut::MillAcross(const Point& p, const double phi) {
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(phi));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(phi + twist_));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(phi));
    reverse_ = !reverse_;
}

double FinishingCut::CalcG(const double Ra, const double R) const {
    return std::sqrt(8 * Ra * cfg_.cutter_diameter * R / (R + cfg_.cutter_diameter));
}
