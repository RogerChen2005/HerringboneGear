#pragma once

#include "gear_params.h"
#include <string>
#include <vector>

// Pure NC format writer — no geometry, no toolpath logic.
class NCConverter {
public:
    explicit NCConverter(const GearParams& params);

    // --- Low-level NC commands ---
    void AddLine(const std::string& line);
    void RapidLine(double x, double y, double z, double a, double c);
    void CutLine(double x, double y, double z, double a, double c);
    void Comment(const std::string& text);
    void BlankLine();

    // --- Program structure ---
    void ProgramHeader();
    void ProgramFooter();

    // --- Output ---
    void WriteToFile(const std::string& filename);
    void ClearAll();

    void operator+=(const NCConverter& nc);

private:
    GearParams params_;
    std::vector<std::string> lines_;
};
