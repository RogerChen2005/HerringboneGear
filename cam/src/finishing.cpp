#include "finishing.h"
#include "gear_geometry.h"
#include <cmath>
#include <string>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

FinishingCut::FinishingCut(const GearParams& params)
    : nc_(params), params_(params), depth_(0.2), d_cutter_(4.0), remain_(0.5),
    reverse_(false), twist_(gear::TwistAngle(params, params.F))  {}

FinishingCut::~FinishingCut() = default;

void FinishingCut::FinishRoot(const double base) {
    const double deg = M_PI / 180.0;
    double r  = params_.m * params_.z / 2.0;
    double rb = r  * cos(params_.alpha * deg);
    double ra = r  + params_.m;
    double rd = r  - 1.25 * params_.m;

    auto inv = [](double t) { return t - atan(t); };

    double g = std::sqrt(8*0.064*d_cutter_*params_.Rg/(params_.Rg + d_cutter_));
    double inv_pc     = inv(params_.alpha * deg);
    double theta_half = M_PI / (2.0 * params_.z);
    double phi0       = theta_half + inv_pc;

    const int cnt = int(remain_ / depth_) + 1;
    for (int i = 1;i <= cnt;i++) {
        nc_.RapidLine(Point::fromPolar(rb,base + phi0), params_.F * 2 + 10);
        double dist = remain_ - depth_ * i ;
        dist = dist > 0 ? dist : 0;
        if ( params_.z < 42) {
            double theta = gear::calc_theta(rb, rd, params_.Rg);
            double phi = std::asin(sin(theta) / params_.Rg*rb);
            double d_phi = g / params_.Rg;
            Point conner_center = Point::fromPolar(rd + params_.Rg, base + phi0 + theta);
            int cnt2 = (int)(phi / d_phi);
            for (int j = cnt2;j >=0;j--) {
                double r = params_.Rg - dist - d_cutter_ / 2;
                CutAcross(conner_center.movedPolar(-r, base + phi0 + theta + j*d_phi));
            }
        }
        nc_.RapidLine(Point::fromPolar(rb,base + phi0), params_.F * 2 + 10);
    }
}

void FinishingCut::CutAcross(const Point& p) {
    Point mid = p.rotated(twist_);
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 0 : 2), -90, -gear::RadToDeg(p.angle()));
    nc_.CutLine(mid.y, mid.x, params_.F, -90, -gear::RadToDeg(mid.angle()));
    nc_.CutLine(p.y, p.x, params_.F * (reverse_ ? 2 : 0), -90, -gear::RadToDeg(p.angle()));
    this->reverse_ = !this->reverse_;
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
        FinishRoot(base);
    }
    return nc_;
}
