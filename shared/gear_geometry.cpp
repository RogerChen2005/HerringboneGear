#include "gear_geometry.h"
#include <cmath>

namespace gear {

// ---------------------------------------------------------------------------
// Involute profile — same math as CAD project (profile.cpp).
// ---------------------------------------------------------------------------

Profile ComputeProfile(const GearParams& g)
{
    const double deg = M_PI / 180.0;

    double r  = g.m * g.z / 2.0;
    double rb = r  * cos(g.alpha * deg);
    double ra = r  + g.m;
    double rd = r  - 1.25 * g.m;

    auto inv = [](double t) { return t - atan(t); };

    double inv_pc     = tan(g.alpha * deg) - g.alpha * deg;
    double theta_half = M_PI / (2.0 * g.z);
    double phi0       = theta_half + inv_pc;

    double t_tip  = sqrt((ra / rb) * (ra / rb) - 1.0);
    double t_root = (rd > rb) ? sqrt((rd / rb) * (rd / rb) - 1.0) : 0.0;
    double inv_tip = inv(t_tip);

    Profile pts;
    const double tooth_step = 2.0 * M_PI / g.z;

    for (int n = 0; n < g.z; ++n) {
        double base = n * tooth_step;

        // Left flank
        for (int i = 0; i <= g.Kt; ++i) {
            double t     = t_root + (t_tip - t_root) * i / g.Kt;
            double angle = base - phi0 + inv(t);
            double rad   = rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        // Tip arc
        double ang_L = base - phi0 + inv_tip;
        double ang_R = base + phi0 - inv_tip;
        for (int i = 1; i <= g.Ka; ++i) {
            double a = ang_L + (ang_R - ang_L) * i / g.Ka;
            pts.push_back({ ra * cos(a), ra * sin(a) });
        }

        // Right flank
        for (int i = g.Kt; i >= 0; --i) {
            double t     = t_root + (t_tip - t_root) * i / g.Kt;
            double angle = base + phi0 - inv(t);
            double rad   = rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        // Root fillet arc
        double ang_root_end  = base + phi0;
        double ang_root_next = base + tooth_step - phi0;
        for (int i = 1; i <= g.Kr; ++i) {
            double a = ang_root_end + (ang_root_next - ang_root_end) * i / g.Kr;
            pts.push_back({ rd * cos(a), rd * sin(a) });
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
