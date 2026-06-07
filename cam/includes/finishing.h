#pragma once

#include "nc_converter.h"
#include "gear_geometry.h"

class FinishingCut {
public:
    explicit FinishingCut(const GearParams& params);
    ~FinishingCut();

    NCConverter& Generate(int teeth_count);

private:
    NCConverter nc_;
    GearParams params_;

    void FinishTooth(const Profile& profile, int ppt,
                     int tooth_idx, double z_pos,
                     double twist_rad, double helix_deg);
    void FinishLayer(const Profile& profile, int ppt,
                     double z_pos, int layer_index);
};
