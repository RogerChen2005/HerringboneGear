#include "nc_converter.h"
#include <algorithm>
#include <cmath>
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

static std::string Fmt(double v, int prec = 4)
{
    std::ostringstream os;
    os << std::fixed << std::setprecision(prec) << v;
    return os.str();
}

static std::string FmtXYZ(double v) { return Fmt(v, 3); }
static std::string FmtAC(double v)  { return Fmt(v, 4); }

// ---------------------------------------------------------------------------
// NC command helpers (Heidenhain format)
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
    os << "L X" << FmtXYZ(x)
       << " Y" << FmtXYZ(y)
       << " Z" << FmtXYZ(z)
       << " A" << FmtAC(a)
       << " C" << FmtAC(c)
       << " FMAX";
    lines_.push_back(os.str());
}

void NCConverter::CutLine(double x, double y, double z, double a, double c)
{
    std::ostringstream os;
    os << "L X" << FmtXYZ(x)
       << " Y" << FmtXYZ(y)
       << " Z" << FmtXYZ(z)
       << " A" << FmtAC(a)
       << " C" << FmtAC(c)
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
    AddLine("BLK FORM 0.1 Z X" + FmtXYZ(-ra) +
            " Y" + FmtXYZ(-ra) +
            " Z" + FmtXYZ(-params_.F));
    AddLine("BLK FORM 0.2 X" + FmtXYZ(ra) +
            " Y" + FmtXYZ(ra) +
            " Z" + FmtXYZ(params_.F));

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
// Geometry helpers
// ---------------------------------------------------------------------------

double NCConverter::RadToDeg(double rad) const
{
    return rad * 180.0 / M_PI;
}

double NCConverter::TwistAngle(double z) const
{
    double r    = params_.m * params_.z / 2.0;
    double beta = params_.beta * M_PI / 180.0;
    return z * tan(beta) / r;
}

double NCConverter::HelixAngleDeg(double z) const
{
    return (z >= 0.0) ? params_.beta : -params_.beta;
}

std::pair<double, double> NCConverter::RotatePoint(
    double x, double y, double angle_rad) const
{
    double cs = cos(angle_rad), sn = sin(angle_rad);
    return { x * cs - y * sn, x * sn + y * cs };
}

Profile NCConverter::ComputeProfile() const
{
    const double deg = M_PI / 180.0;

    double r  = params_.m * params_.z / 2.0;
    double rb = r  * cos(params_.alpha * deg);
    double ra = r  + params_.m;
    double rd = r  - 1.25 * params_.m;

    auto inv = [](double t) { return t - atan(t); };

    double inv_pc     = tan(params_.alpha * deg) - params_.alpha * deg;
    double theta_half = M_PI / (2.0 * params_.z);
    double phi0       = theta_half + inv_pc;

    double t_tip  = sqrt((ra / rb) * (ra / rb) - 1.0);
    double t_root = (rd > rb) ? sqrt((rd / rb) * (rd / rb) - 1.0) : 0.0;
    double inv_tip = inv(t_tip);

    Profile pts;
    const double tooth_step = 2.0 * M_PI / params_.z;

    for (int n = 0; n < params_.z; ++n) {
        double base = n * tooth_step;

        for (int i = 0; i <= params_.Kt; ++i) {
            double t     = t_root + (t_tip - t_root) * i / params_.Kt;
            double angle = base - phi0 + inv(t);
            double rad   = rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        double ang_L = base - phi0 + inv_tip;
        double ang_R = base + phi0 - inv_tip;
        for (int i = 1; i <= params_.Ka; ++i) {
            double a = ang_L + (ang_R - ang_L) * i / params_.Ka;
            pts.push_back({ ra * cos(a), ra * sin(a) });
        }

        for (int i = params_.Kt; i >= 0; --i) {
            double t     = t_root + (t_tip - t_root) * i / params_.Kt;
            double angle = base + phi0 - inv(t);
            double rad   = rb * sqrt(1.0 + t * t);
            pts.push_back({ rad * cos(angle), rad * sin(angle) });
        }

        double ang_root_end  = base + phi0;
        double ang_root_next = base + tooth_step - phi0;
        for (int i = 1; i <= params_.Kr; ++i) {
            double a = ang_root_end + (ang_root_next - ang_root_end) * i / params_.Kr;
            pts.push_back({ rd * cos(a), rd * sin(a) });
        }
    }
    return pts;
}

// ---------------------------------------------------------------------------
// Toolpath generation
// ---------------------------------------------------------------------------

void NCConverter::GenerateAllLayers()
{
    double layer_thickness = 2.0;
    int total_layers = static_cast<int>(ceil(2.0 * params_.F / layer_thickness));

    Comment("--- Cutting begins ---");
    BlankLine();

    double ra = params_.m * params_.z / 2.0 + params_.m;
    double helix0 = HelixAngleDeg(-params_.F);
    RapidLine(ra + 50.0, 0.0, 300.0, helix0, 0.0);

    for (int layer = 0; layer <= total_layers; ++layer) {
        double z_pos = -params_.F + layer * layer_thickness;
        if (z_pos > params_.F) z_pos = params_.F;

        GenerateLayer(z_pos, layer, total_layers);
    }
}

void NCConverter::GenerateLayer(double z_pos, int layer_index, int /*total_layers*/)
{
    double twist_rad = TwistAngle(z_pos);
    double helix_deg = HelixAngleDeg(z_pos);

    Comment("Layer " + std::to_string(layer_index) +
            "  Z=" + FmtXYZ(z_pos) +
            "  twist=" + FmtAC(RadToDeg(twist_rad)) + " deg" +
            "  A=" + FmtAC(helix_deg));
    BlankLine();

    bool reverse = (layer_index % 2 != 0);

    if (reverse) {
        for (int n = params_.z - 1; n >= 0; --n)
            GenerateTooth(n, z_pos, twist_rad, helix_deg, reverse);
    } else {
        for (int n = 0; n < params_.z; ++n)
            GenerateTooth(n, z_pos, twist_rad, helix_deg, reverse);
    }
}

void NCConverter::GenerateTooth(int tooth_idx, double z_pos,
                                double twist_rad, double helix_deg,
                                bool reverse)
{
    Profile profile = ComputeProfile();
    int ppt = 2 * params_.Kt + params_.Ka + params_.Kr + 2;

    double tooth_base = tooth_idx * 2.0 * M_PI / params_.z;
    double total_rot = twist_rad + tooth_base;

    int start = tooth_idx * ppt;
    int end   = start + ppt;

    std::vector<std::pair<double, double>> tooth_pts;
    for (int i = start; i < end; ++i)
        tooth_pts.push_back(profile[i]);

    if (reverse)
        std::reverse(tooth_pts.begin(), tooth_pts.end());

    {
        auto [rx, ry] = RotatePoint(tooth_pts[0].first, tooth_pts[0].second, total_rot);
        RapidLine(rx, ry, z_pos + 10.0, helix_deg, RadToDeg(total_rot));
    }

    {
        auto [px, py] = RotatePoint(tooth_pts[0].first, tooth_pts[0].second, total_rot);
        CutLine(px, py, z_pos, helix_deg, RadToDeg(total_rot));
    }

    for (size_t i = 1; i < tooth_pts.size(); ++i) {
        auto [cx, cy] = RotatePoint(tooth_pts[i].first, tooth_pts[i].second, total_rot);
        CutLine(cx, cy, z_pos, helix_deg, RadToDeg(total_rot));
    }

    {
        auto [lx, ly] = RotatePoint(tooth_pts.back().first, tooth_pts.back().second, total_rot);
        RapidLine(lx, ly, z_pos + 10.0, helix_deg, RadToDeg(total_rot));
    }

    BlankLine();
}

// ---------------------------------------------------------------------------
// Main entry point
// ---------------------------------------------------------------------------

void NCConverter::Generate(const std::string& filename)
{
    lines_.clear();

    ProgramHeader();
    GenerateAllLayers();
    ProgramFooter();

    std::ofstream out(filename);
    if (!out)
        throw std::runtime_error("Cannot open file: " + filename);

    for (const auto& line : lines_)
        out << line << "\n";

    out.close();
}
