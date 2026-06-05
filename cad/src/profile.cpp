#include "profile.h"
#include <vtkMath.h>
#include <cmath>

Profile computeProfile(const GearParams& g)
{
    const double PI = vtkMath::Pi();
    const double deg = PI / 180.0;

    double r  = g.m * g.z / 2.0;
    double rb = r  * cos(g.alpha * deg);
    double ra = r  + g.m;
    double rd = r  - 1.25 * g.m;

    auto inv = [](double t) { return t - atan(t); };   // involute function

    // Angular offset from tooth centre to involute start on base circle
    double inv_pc    = tan(g.alpha * deg) - g.alpha * deg;   // inv(α)
    double theta_half = PI / (2.0 * g.z);                    // half pitch angle
    double phi0      = theta_half + inv_pc;

    double t_tip  = sqrt((ra / rb) * (ra / rb) - 1.0);
    double t_root = (rd > rb) ? sqrt((rd / rb) * (rd / rb) - 1.0) : 0.0;
    double inv_tip = inv(t_tip);

    Profile pts;
    const double tooth_step = 2.0 * PI / g.z;

    for (int n = 0; n < g.z; ++n) {
        double base = n * tooth_step;

        // (a) Left flank — CCW (t: t_root → t_tip, θ increases)
        for (int i = 0; i <= g.Kt; ++i) {
            double t      = t_root + (t_tip - t_root) * i / g.Kt;
            double angle  = base - phi0 + inv(t);
            double radius = rb * sqrt(1.0 + t * t);
            pts.push_back({ radius * cos(angle), radius * sin(angle) });
        }

        // (b) Tip arc — CCW (left tip → right tip across addendum circle)
        double ang_L = base - phi0 + inv_tip;
        double ang_R = base + phi0 - inv_tip;
        for (int i = 1; i <= g.Ka; ++i) {
            double a = ang_L + (ang_R - ang_L) * i / g.Ka;
            pts.push_back({ ra * cos(a), ra * sin(a) });
        }

        // (c) Right flank — CCW (t: t_tip → t_root, θ increases as t decreases)
        for (int i = g.Kt; i >= 0; --i) {
            double t      = t_root + (t_tip - t_root) * i / g.Kt;
            double angle  = base + phi0 - inv(t);
            double radius = rb * sqrt(1.0 + t * t);
            pts.push_back({ radius * cos(angle), radius * sin(angle) });
        }

        // (d) Root fillet arc — CCW (this tooth's right root → next tooth's left root)
        double ang_root_end  = base + phi0;
        double ang_root_next = base + tooth_step - phi0;
        for (int i = 1; i <= g.Kr; ++i) {
            double a = ang_root_end + (ang_root_next - ang_root_end) * i / g.Kr;
            pts.push_back({ rd * cos(a), rd * sin(a) });
        }
    }
    return pts;
}
