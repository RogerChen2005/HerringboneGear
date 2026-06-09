#include "finishing.h"
#include "gear_geometry.h"
#include "gear_derived.h"
#include <cmath>
#include <string>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

FinishingCut::FinishingCut(const GearParams& params)
    : nc_(params), params_(params), derived_(params), 
    depth_(0.2), d_cutter_(4.0),h_cutter_(25.0), remain_(0.5), Ra_(0.064),
    reverse_(false), twist_(gear::TwistAngle(params, params.F)),
    theta_(gear::calc_theta(derived_.rb, derived_.rd, params.Rg))  {}

FinishingCut::~FinishingCut() = default;

void FinishingCut::FinishRoot(const double base, const double dist) {
    if ( params_.z < 42) FinishCorner(base, dist, Left);
    double start = base + derived_.phi0 + theta_;
    double end = base + derived_.theta_half * 4 - derived_.phi0 - theta_;
    double g_ = CalcG(Ra_, derived_.rd);
    if (start < end){
        double radius = derived_.rd + dist + d_cutter_ / 2;
        double d_phi = g_ / radius;
        const int t = static_cast<int>((end - start) / d_phi);
        Point p = Point::fromPolar(radius, start);
        for (int i = 0;i <= t;i++) CutAcross(p.rotated(i * d_phi));
        CutAcross(p.rotated(end - start));
    } 
    if ( params_.z < 42) FinishCorner(base, dist, Right);
}

void FinishingCut::FinishCorner(const double base, const double dist, const Direction direction){
    // nc_.Comment("Finishing " + ((direction == Left) ? std::string("Left"):std::string("Right")) + " Corner");
    double phi = std::asin(sin(theta_) / params_.Rg*derived_.rb);
    double g_ = CalcG(Ra_, params_.Rg);
    double d_phi = g_ / params_.Rg;
    Point conner_center = Point::fromPolar(derived_.rd + params_.Rg, 
        base + ((direction == Left) ? (derived_.phi0 + theta_) : (derived_.theta_half * 4 - derived_.phi0 - theta_)));
    int cnt = static_cast<int>(phi / d_phi);
    double r = params_.Rg - dist - d_cutter_ / 2;
    for (int i = cnt;i >= 0;i--) 
        CutAcross(conner_center.movedPolar(-r, 
            conner_center.angle() + ((direction == Left) ? i * d_phi : (i - cnt) * d_phi)));
}

void FinishingCut::FinishCurve(const double base, const double dist, const Direction direction) {
    double arc = sqrt(derived_.ra * derived_.ra - derived_.rb * derived_.rb);
    double g_ = CalcG(Ra_, arc);
    int cnt = static_cast<int>(arc / g_) + 1;
    if ( direction == Left ) {
        double tb = base + derived_.phi0;
        double ta = tb - arc / derived_.rb;
        for (int i = 0;i <= cnt;i++){
            double height = h_cutter_ - (h_cutter_ + d_cutter_) * sqrt((double)i / cnt);
            double dt = (tb - ta) * i / cnt;
            double t = ta + dt;
            double len = arc - dt * derived_.rb;
            Point p = Point::fromPolar(derived_.rb - height, t);
            p.movePolar(len + dist + d_cutter_ / 2, t + M_PI / 2);
            MillAcross(p, t);
        }
    }
    else {
        double tb = base + derived_.theta_half * 4 -  derived_.phi0;
        double ta = tb + arc / derived_.rb;
        for (int i = cnt;i >= 0;i--){
            double height = h_cutter_ - (h_cutter_ + d_cutter_) * sqrt((double)i / cnt);
            double dt = (ta - tb) * i / cnt;
            double t = ta - dt;
            double len = arc - dt * derived_.rb;
            Point p = Point::fromPolar(derived_.rb - height, t);
            p.movePolar(len + dist + d_cutter_ / 2, t - M_PI / 2);
            MillAcross(p, t);
            if (i == 0 && reverse_) MillAcross(p, t);
        }
    }
}

void FinishingCut::FinishLayer(const double base) {
    const int cnt = static_cast<int>(remain_ / depth_) + 1;
    for (int i = 1;i <= cnt;i++) {
        double t_tip  = sqrt((derived_.ra / derived_.rb) * (derived_.ra / derived_.rb) - 1.0);
        double inv_t = t_tip - atan(t_tip);
        nc_.RapidLine(Point::fromPolar(derived_.ra,base + derived_.phi0 - inv_t), params_.F * 2 + 10);
        double dist = remain_ - depth_ * i ;
        dist = dist > 0 ? dist : 0;
        FinishCurve(base, dist, Left);
        FinishRoot(base, dist);
        FinishCurve(base, dist, Right);
        nc_.RapidLine(Point::fromPolar(derived_.ra,base + 4 * derived_.theta_half - derived_.phi0 + inv_t), params_.F * 2 + 10);
    }
}

void FinishingCut::CutAcross(const Point& p) {
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(p.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(p.angle()));
    this->reverse_ = !this->reverse_;
}

void FinishingCut::MillAcross(const Point& p, const double phi) {
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(phi));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(phi + twist_));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(phi));
    this->reverse_ = !this->reverse_;
}

double FinishingCut::CalcG(const double Ra, const double R) {
    return std::sqrt(8 * Ra * d_cutter_ * R / (R + d_cutter_));
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

NCConverter& FinishingCut::Generate(int teeth_count)
{
    const double tooth_step = 2.0 * M_PI / params_.z;
    int cnt = teeth_count <= params_.z ? teeth_count : params_.z;
    nc_.ClearAll();
    nc_.Comment("=== FINISHING PASS ===");
    for (int n = 0; n < cnt; ++n) {
        double base = n * tooth_step;
        FinishLayer(base);
    }
    return nc_;
}
