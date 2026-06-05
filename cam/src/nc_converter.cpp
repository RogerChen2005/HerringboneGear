#include "nc_converter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

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

// ---------------------------------------------------------------------------
// Program structure
// ---------------------------------------------------------------------------

void NCConverter::ProgramHeader()
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
    AddLine("TOOL CALL 1 Z S3000");
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
    std::ofstream out(filename);
    if (!out)
        throw std::runtime_error("Cannot open file: " + filename);

    for (const auto& line : lines_)
        out << line << "\n";
}
