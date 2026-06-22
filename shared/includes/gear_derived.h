#pragma once

#include "gear_params.h"
#include <cmath>

struct GearDerived {
    double mt;
    double r;           // pitch radius
    double rb;          // base circle radius
    double ra;          // addendum (tip) radius
    double rd;          // dedendum (root) radius
    double inv_pc;      // involute function at pressure angle
    double theta_half;  // half tooth angular span = PI / (2*z)
    double phi0;        // initial angular offset = theta_half + inv_pc

    explicit GearDerived(const GearParams& g)
        : mt(g.m / std::cos(g.beta * M_PI / 180.0))
        , r(mt * g.z / 2.0)
        , rb(r * std::cos(g.alpha * M_PI / 180.0))
        , ra(r + (1 + g.x) * mt)
        , rd(r - (1.25 - g.x) * mt)
        , inv_pc(inv(std::tan(g.alpha * M_PI / 180.0)))
        , theta_half(M_PI / (2.0 * g.z))
        , phi0(theta_half + inv_pc)
    {}

    static double inv(double t) { return t - std::atan(t); }

    // True when the root circle lies inside the base circle, so the flank
    // cannot reach the root and a connecting fillet arc (radius Rg) is needed.
    bool hasRootRelief() const { return rd < rb; }
};
