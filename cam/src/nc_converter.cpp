#include "nc_converter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

NCConverter::NCConverter(const GearParams& params)
    : params_(params)
{
}

// ---------------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------------

static std::string Fmt(double v, int prec)
{
    std::ostringstream os;
    os << std::fixed << std::setprecision(prec) << v;
    return os.str();
}

// ---------------------------------------------------------------------------
// Low-level NC commands
// ---------------------------------------------------------------------------

void NCConverter::AddLine(const std::string& line)
{
    lines_.push_back(line);
}

void NCConverter::Comment(const std::string& text)
{
    lines_.push_back(";" + text);
}

void NCConverter::BlankLine()
{
    lines_.emplace_back();
}

void NCConverter::RapidLine(double x, double y, double z, double a, double c)
{
    std::ostringstream os;
    os << "L X" << Fmt(x, 3)
       << " Y" << Fmt(y, 3)
       << " Z" << Fmt(z, 3)
       << " A" << Fmt(a, 4)
       << " C" << Fmt(c, 4)
       << " FMAX";
    lines_.push_back(os.str());
}

void NCConverter::RapidLine(const Point p, double z)
{
    std::ostringstream os;
    os << "L X" << Fmt(p.y, 3)
       << " Y" << Fmt(p.x, 3)
       << " Z" << Fmt(z, 3)
       << " A" << Fmt(-90, 4)
       << " C" << Fmt(-p.angle(), 4)
       << " FMAX";
    lines_.push_back(os.str());
}

void NCConverter::CutLine(double x, double y, double z, double a, double c)
{
    std::ostringstream os;
    os << "L X" << Fmt(x, 3)
       << " Y" << Fmt(y, 3)
       << " Z" << Fmt(z, 3)
       << " A" << Fmt(a, 4)
       << " C" << Fmt(c, 4)
       << " F1000";
    lines_.push_back(os.str());
}

void NCConverter::ChangeTool(int number) {
    AddLine("TOOL CALL " + std::to_string(number) + " Z S3000");
}

// ---------------------------------------------------------------------------
// Program structure
// ---------------------------------------------------------------------------

void NCConverter::ProgramHeader(int tool)
{
    AddLine("BEGIN PGM 100 MM");

    double ra = params_.m * params_.z / 2.0 + params_.m;
    AddLine("BLK FORM 0.1 Z X" + Fmt(-ra, 3) +
            " Y" + Fmt(-ra, 3) +
            " Z" + Fmt(-params_.F, 3));
    AddLine("BLK FORM 0.2 X" + Fmt(ra, 3) +
            " Y" + Fmt(ra, 3) +
            " Z" + Fmt(params_.F, 3));

    BlankLine();
    AddLine("M129");
    ChangeTool(tool);
    AddLine("M3");
    AddLine("M11 M16 M140 MB MAX F5000");
    BlankLine();

    AddLine("L IZ-10 F1000");
    AddLine("L X0.0 Y0.0 A0.0 C0.0 FMAX");
    AddLine("M126 M128");
    AddLine("L Z300. F600");
    BlankLine();
}

void NCConverter::ProgramFooter()
{
    BlankLine();
    AddLine("M05 M09 M127 M129");
    AddLine("M140 MB MAX F5000");
    AddLine("L A0.0 C0.0 FMAX");
    AddLine("L X0.0 Y0.0 FMAX");
    AddLine("M30");
    AddLine("END PGM 100 MM");
}

// ---------------------------------------------------------------------------
// Output
// ---------------------------------------------------------------------------

void NCConverter::WriteToFile(const std::string& filename)
{
    // The UI passes a UTF-8 path. On Windows std::ofstream(const char*) opens
    // via the ANSI code page, which mangles non-ASCII paths (e.g. Chinese
    // folder names) and fails to open. u8path reinterprets the bytes as UTF-8
    // and yields a native (wide) path, matching how VTK writes the STL files.
    std::ofstream out(std::filesystem::u8path(filename));
    if (!out)
        throw std::runtime_error("Cannot open file: " + filename);

    for (const auto& line : lines_)
        out << line << "\n";
}

std::string NCConverter::ToString() const
{
    std::ostringstream out;
    for (const auto& line : lines_)
        out << line << '\n';
    return out.str();
}

void NCConverter::ClearAll() {
    this->lines_.clear();
}

NCConverter& NCConverter::operator+=(const NCConverter& nc) {
    lines_.insert(lines_.end(), nc.lines_.begin(), nc.lines_.end());
    return *this;
}
