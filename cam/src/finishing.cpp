#include "finishing.h"
#include "gear_geometry.h"
#include <cmath>
#include <string>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

FinishingCut::FinishingCut(const GearParams& params)
    : nc_(params), params_(params) {}

FinishingCut::~FinishingCut() = default;

// ---------------------------------------------------------------------------
// Per-tooth finishing — single continuous pass, always forward
// ---------------------------------------------------------------------------

void FinishingCut::FinishTooth(const Profile& profile, int ppt,
                               int tooth_idx, double z_pos,
                               double twist_rad, double helix_deg)
{
    double tooth_base = tooth_idx * 2.0 * M_PI / params_.z;
    double total_rot  = twist_rad + tooth_base;
    double c_deg = gear::RadToDeg(total_rot);

    int start = tooth_idx * ppt;
    int end   = start + ppt;

    // Rapid to approach height
    {
        auto [rx, ry] = gear::RotatePoint(profile[start].first,
                                           profile[start].second, total_rot);
        nc_.RapidLine(rx, ry, z_pos + 5.0, helix_deg, c_deg);
    }

    // Plunge
    {
        auto [px, py] = gear::RotatePoint(profile[start].first,
                                           profile[start].second, total_rot);
        nc_.CutLine(px, py, z_pos, helix_deg, c_deg);
    }

    // Skim along profile (always forward for consistent surface finish)
    for (int i = start + 1; i < end; ++i) {
        auto [cx, cy] = gear::RotatePoint(profile[i].first,
                                           profile[i].second, total_rot);
        nc_.CutLine(cx, cy, z_pos, helix_deg, c_deg);
    }

    // Retract
    {
        auto [lx, ly] = gear::RotatePoint(profile[end - 1].first,
                                           profile[end - 1].second, total_rot);
        nc_.RapidLine(lx, ly, z_pos + 5.0, helix_deg, c_deg);
    }

    nc_.BlankLine();
}

// ---------------------------------------------------------------------------
// Per-layer finishing
// ---------------------------------------------------------------------------

void FinishingCut::FinishLayer(const Profile& profile, int ppt,
                               double z_pos, int layer_index)
{
    double twist_rad = gear::TwistAngle(params_, z_pos);
    double helix_deg = gear::HelixAngleDeg(params_, z_pos);

    nc_.Comment("Finish layer " + std::to_string(layer_index) +
                "  Z=" + std::to_string(z_pos).substr(0, 7) +
                "  A=" + std::to_string(helix_deg).substr(0, 7));
    nc_.BlankLine();

    // Always forward — no zigzag for finish quality
    for (int n = 0; n < params_.z; ++n)
        FinishTooth(profile, ppt, n, z_pos, twist_rad, helix_deg);
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

NCConverter& FinishingCut::Generate(int teeth_count)
{
    Profile profile = gear::ComputeProfile(params_);
    int ppt = 2 * params_.Kt + params_.Ka + params_.Kr + 2;

    double layer_thickness = 2.0;
    int total_layers = static_cast<int>(std::ceil(2.0 * params_.F / layer_thickness));
    int cnt = (teeth_count <= params_.z) ? teeth_count : params_.z;

    nc_.ClearAll();
    nc_.Comment("=== FINISHING PASS ===");
    nc_.BlankLine();

    double ra = params_.m * params_.z / 2.0 + params_.m;
    double helix0 = gear::HelixAngleDeg(params_, -params_.F);
    nc_.RapidLine(ra + 50.0, 0.0, 300.0, helix0, 0.0);

    for (int layer = 0; layer <= total_layers; ++layer) {
        double z_pos = -params_.F + layer * layer_thickness;
        if (z_pos > params_.F) z_pos = params_.F;
        FinishLayer(profile, ppt, z_pos, layer);
    }

    return nc_;
}
