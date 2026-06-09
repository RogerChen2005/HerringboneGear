#pragma once

#include "gear_params.h"
#include <string>
#include <vector>
#include "point.h"

// Pure NC format writer — no geometry, no toolpath logic.
class NCConverter {
public:
    explicit NCConverter(const GearParams& params);

    // --- Low-level NC commands ---
    void AddLine(const std::string& line);
    void RapidLine(double x, double y, double z, double a, double c);
    void RapidLine(const Point p, double z);
    void CutLine(double x, double y, double z, double a, double c);
    void ChangeTool(int number);
    void Comment(const std::string& text);
    void BlankLine();

    // --- Program structure ---
    void ProgramHeader(int tool);
    void ProgramFooter();

    // --- Output ---
    void WriteToFile(const std::string& filename);
    void ClearAll();

    void operator+=(const NCConverter& nc);

private:
    GearParams params_;
    std::vector<std::string> lines_;
};
