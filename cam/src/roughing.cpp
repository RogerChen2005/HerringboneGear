#include "roughing.h"
#include "gear_geometry.h"
#include <algorithm>
#include <cmath>
#include <string>

// ---------------------------------------------------------------------------
// Per-tooth roughing cut
// ---------------------------------------------------------------------------

static void RoughTooth(NCConverter& nc, const GearParams& g,
                       const Profile& profile, int ppt,
                       int tooth_idx, double z_pos,
                       double twist_rad, double helix_deg,
                       bool reverse)
{
    double tooth_base = tooth_idx * 2.0 * M_PI / g.z;
    double total_rot  = twist_rad + tooth_base;

    int start = tooth_idx * ppt;
    int end   = start + ppt;

    std::vector<std::pair<double, double>> pts;
    for (int i = start; i < end; ++i)
        pts.push_back(profile[i]);

    if (reverse)
        std::reverse(pts.begin(), pts.end());

    // Rapid to approach height
    {
        auto [rx, ry] = gear::RotatePoint(pts[0].first, pts[0].second, total_rot);
        nc.RapidLine(rx, ry, z_pos + 10.0, helix_deg, gear::RadToDeg(total_rot));
    }

    // Plunge
    {
        auto [px, py] = gear::RotatePoint(pts[0].first, pts[0].second, total_rot);
        nc.CutLine(px, py, z_pos, helix_deg, gear::RadToDeg(total_rot));
    }

    // Cut along profile
    for (size_t i = 1; i < pts.size(); ++i) {
        auto [cx, cy] = gear::RotatePoint(pts[i].first, pts[i].second, total_rot);
        nc.CutLine(cx, cy, z_pos, helix_deg, gear::RadToDeg(total_rot));
    }

    // Retract
    {
        auto [lx, ly] = gear::RotatePoint(pts.back().first, pts.back().second, total_rot);
        nc.RapidLine(lx, ly, z_pos + 10.0, helix_deg, gear::RadToDeg(total_rot));
    }

    nc.BlankLine();
}

// ---------------------------------------------------------------------------
// Per-layer roughing
// ---------------------------------------------------------------------------

static void RoughLayer(NCConverter& nc, const GearParams& g,
                       const Profile& profile, int ppt,
                       double z_pos, int layer_index)
{
    double twist_rad = gear::TwistAngle(g, z_pos);
    double helix_deg = gear::HelixAngleDeg(g, z_pos);

    nc.Comment("Rough layer " + std::to_string(layer_index) +
               "  Z=" + std::to_string(z_pos).substr(0, 7) +
               "  A=" + std::to_string(helix_deg).substr(0, 7));
    nc.BlankLine();

    bool reverse = (layer_index % 2 != 0);

    if (reverse) {
        for (int n = g.z - 1; n >= 0; --n)
            RoughTooth(nc, g, profile, ppt, n, z_pos, twist_rad, helix_deg, reverse);
    } else {
        for (int n = 0; n < g.z; ++n)
            RoughTooth(nc, g, profile, ppt, n, z_pos, twist_rad, helix_deg, reverse);
    }
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

void GenerateRoughing(NCConverter& nc, const GearParams& g)
{
    Profile profile = gear::ComputeProfile(g);
    int ppt = 2 * g.Kt + g.Ka + g.Kr + 2;

    double layer_thickness = 2.0;
    int total_layers = static_cast<int>(ceil(2.0 * g.F / layer_thickness));

    nc.Comment("=== ROUGHING PASS ===");
    nc.BlankLine();

    double ra = g.m * g.z / 2.0 + g.m;
    double helix0 = gear::HelixAngleDeg(g, -g.F);
    nc.RapidLine(ra + 50.0, 0.0, 300.0, helix0, 0.0);

    for (int layer = 0; layer <= total_layers; ++layer) {
        double z_pos = -g.F + layer * layer_thickness;
        if (z_pos > g.F) z_pos = g.F;
        RoughLayer(nc, g, profile, ppt, z_pos, layer);
    }
}
