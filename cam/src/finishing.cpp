#include "finishing.h"
#include "gear_geometry.h"
#include <cmath>
#include <string>

// ---------------------------------------------------------------------------
// Per-tooth finishing cut — single continuous pass, no zigzag.
// ---------------------------------------------------------------------------

static void FinishTooth(NCConverter& nc, const GearParams& g,
                        const Profile& profile, int ppt,
                        int tooth_idx, double z_pos,
                        double twist_rad, double helix_deg)
{
    double tooth_base = tooth_idx * 2.0 * M_PI / g.z;
    double total_rot  = twist_rad + tooth_base;

    int start = tooth_idx * ppt;
    int end   = start + ppt;

    // Rapid to approach height
    {
        auto [rx, ry] = gear::RotatePoint(profile[start].first,
                                           profile[start].second, total_rot);
        nc.RapidLine(rx, ry, z_pos + 5.0, helix_deg, gear::RadToDeg(total_rot));
    }

    // Plunge
    {
        auto [px, py] = gear::RotatePoint(profile[start].first,
                                           profile[start].second, total_rot);
        nc.CutLine(px, py, z_pos, helix_deg, gear::RadToDeg(total_rot));
    }

    // Skim along profile (always forward — no reverse for finish quality)
    for (int i = start + 1; i < end; ++i) {
        auto [cx, cy] = gear::RotatePoint(profile[i].first,
                                           profile[i].second, total_rot);
        nc.CutLine(cx, cy, z_pos, helix_deg, gear::RadToDeg(total_rot));
    }

    // Retract
    {
        auto [lx, ly] = gear::RotatePoint(profile[end - 1].first,
                                           profile[end - 1].second, total_rot);
        nc.RapidLine(lx, ly, z_pos + 5.0, helix_deg, gear::RadToDeg(total_rot));
    }

    nc.BlankLine();
}

// ---------------------------------------------------------------------------
// Per-layer finishing
// ---------------------------------------------------------------------------

static void FinishLayer(NCConverter& nc, const GearParams& g,
                        const Profile& profile, int ppt,
                        double z_pos, int layer_index)
{
    double twist_rad = gear::TwistAngle(g, z_pos);
    double helix_deg = gear::HelixAngleDeg(g, z_pos);

    nc.Comment("Finish layer " + std::to_string(layer_index) +
               "  Z=" + std::to_string(z_pos).substr(0, 7) +
               "  A=" + std::to_string(helix_deg).substr(0, 7));
    nc.BlankLine();

    // Always forward for consistent surface finish
    for (int n = 0; n < g.z; ++n)
        FinishTooth(nc, g, profile, ppt, n, z_pos, twist_rad, helix_deg);
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

void GenerateFinishing(NCConverter& nc, const GearParams& g)
{
    Profile profile = gear::ComputeProfile(g);
    int ppt = 2 * g.Kt + g.Ka + g.Kr + 2;

    double layer_thickness = 2.0;
    int total_layers = static_cast<int>(ceil(2.0 * g.F / layer_thickness));

    nc.Comment("=== FINISHING PASS ===");
    nc.BlankLine();

    double ra = g.m * g.z / 2.0 + g.m;
    double helix0 = gear::HelixAngleDeg(g, -g.F);
    nc.RapidLine(ra + 50.0, 0.0, 300.0, helix0, 0.0);

    for (int layer = 0; layer <= total_layers; ++layer) {
        double z_pos = -g.F + layer * layer_thickness;
        if (z_pos > g.F) z_pos = g.F;
        FinishLayer(nc, g, profile, ppt, z_pos, layer);
    }
}
