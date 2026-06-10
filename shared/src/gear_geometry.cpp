#include "gear_geometry.h"
#include "gear_derived.h"
#include <cmath>

namespace gear {

double calc_theta(const double _rb, const double _rd, const double _rg) {
    if (_rd > _rb) return 0.0;
    double a = _rd + _rg, b = _rb, c = _rg;
    return std::acos((a * a + b * b - c * c) / (2 * a * b));
}

double calc_radius(const double _theta, const double _rd, const double _rg) {
    double a = _rd + _rg, c = _rg;
    double ct = std::cos(_theta);
    return a * ct - std::sqrt(a * a * ct * ct +  c * c - a * a); 
}

// ---------------------------------------------------------------------------
// Involute profile — same math as CAD project (profile.cpp).
// ---------------------------------------------------------------------------

Profile ComputeProfile(const GearParams& g)
{
    GearDerived d(g);

    auto inv = [](double t) { return t - atan(t); };

    double phi0 = d.phi0;

    double t_tip  = sqrt((d.ra / d.rb) * (d.ra / d.rb) - 1.0);
    double t_root = (d.rd > d.rb) ? sqrt((d.rd / d.rb) * (d.rd / d.rb) - 1.0) : 0.0;
    double inv_tip = inv(t_tip);

    Profile pts;
    const double tooth_step = 2.0 * M_PI / g.z;

    double theta = g.z < 42 ? calc_theta(d.rb, d.rd, g.Rg) : 0.0;

    for (int n = 0; n < g.z; ++n) {
        double base = n * tooth_step;

        // Left flank
        for (int i = 0; i <= g.Kt; ++i) {
            double t     = t_root + (t_tip - t_root) * i / g.Kt;
            double angle = base - phi0 + inv(t);
            double rad   = d.rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        // Tip arc
        double ang_L = base - phi0 + inv_tip;
        double ang_R = base + phi0 - inv_tip;
        for (int i = 1; i <= g.Ka; ++i) {
            double a = ang_L + (ang_R - ang_L) * i / g.Ka;
            pts.push_back({ d.ra * cos(a), d.ra * sin(a) });
        }

        // Right flank
        for (int i = g.Kt; i >= 0; --i) {
            double t     = t_root + (t_tip - t_root) * i / g.Kt;
            double angle = base + phi0 - inv(t);
            double rad   = d.rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        double ang_root_end  = base + phi0;
        double ang_root_next = base + tooth_step - phi0;

        if (g.z < 42)
            for (int i = 1; i <= g.Kr; ++i) {
                double rad = calc_radius(theta * (1.0 - (double)i / g.Kr), d.rd , g.Rg);
                double a = ang_root_end + theta * i / g.Kr;
                pts.push_back({ rad * cos(a), rad * sin(a) });
            }
        
        // Root fillet arc
        for (int i = 1; i <= g.Kr; ++i) {
            double end = ang_root_end + theta, next = ang_root_next - theta;
            double a = end + (next - end) * i / g.Kr;
            pts.push_back({ d.rd * cos(a), d.rd * sin(a) });
        }

        if (g.z < 42)
            for (int i = 1; i <= g.Kr; ++i) {
                double rad = calc_radius(theta * i / g.Kr, d.rd , g.Rg);
                double a = ang_root_next - theta * (1.0 - (double)i / g.Kr);
                pts.push_back({ rad * cos(a), rad * sin(a) });
            }
    }
    return pts;
}

// ---------------------------------------------------------------------------
// Helix helpers
// ---------------------------------------------------------------------------

double TwistAngle(const GearParams& g, double z)
{
    double r    = g.m * g.z / 2.0;
    double beta = g.beta * M_PI / 180.0;
    return z * tan(beta) / r;
}

double HelixAngleDeg(const GearParams& g, double z)
{
    return (z >= 0.0) ? g.beta : -g.beta;
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

std::pair<double, double> RotatePoint(double x, double y, double angle_rad)
{
    double cs = cos(angle_rad), sn = sin(angle_rad);
    return { x * cs - y * sn, x * sn + y * cs };
}

double RadToDeg(double rad)
{
    return rad * 180.0 / M_PI;
}

} // namespace gear
