#pragma once

#include "gear_params.h"
#include <vector>
#include <string>
#include <utility>

using Profile = std::vector<std::pair<double, double>>;

class NCConverter {
public:
    explicit NCConverter(const GearParams& params);

    void Generate(const std::string& filename);

private:
    GearParams params_;
    std::vector<std::string> lines_;

    // --- NC helpers (Heidenhain format) ---
    void AddLine(const std::string& line);
    void RapidLine(double x, double y, double z, double a, double c);
    void CutLine(double x, double y, double z, double a, double c);
    void Comment(const std::string& text);
    void BlankLine();

    // --- Program structure ---
    void ProgramHeader();
    void ProgramFooter();

    // --- Toolpath generation ---
    void GenerateAllLayers();
    void GenerateLayer(double z_pos, int layer_index, int total_layers);
    void GenerateTooth(int tooth_idx, double z_pos,
                       double twist_rad, double helix_deg, bool reverse);

    // --- Geometry helpers ---
    double TwistAngle(double z) const;
    double HelixAngleDeg(double z) const;
    Profile ComputeProfile() const;
    std::pair<double, double> RotatePoint(double x, double y, double angle_rad) const;
    double RadToDeg(double rad) const;
};
