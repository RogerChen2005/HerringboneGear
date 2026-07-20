#include "gear_geometry.h"
#include "gear_derived.h"
#include "cam_generate.h"

#include <cmath>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include <emscripten/emscripten.h>

namespace {

std::vector<float> vertices;
std::vector<std::uint32_t> indices;
std::string result_text;
std::string last_error;

GearParams makeGear(int z, double m, double alpha, double beta, double face,
                    int kt, int ka, int kr, int kz, double x, double rg)
{
    GearParams g;
    g.z = z; g.m = m; g.alpha = alpha; g.beta = beta; g.F = face;
    g.Kt = kt; g.Ka = ka; g.Kr = kr; g.Kz = kz; g.x = x; g.Rg = rg;
    return g;
}

bool validate(const GearParams& g)
{
    last_error = gear::Validate(g);
    return last_error.empty();
}

void appendTriangle(std::uint32_t a, std::uint32_t b, std::uint32_t c)
{
    indices.push_back(a); indices.push_back(b); indices.push_back(c);
}

void buildMesh(const GearParams& g)
{
    const Profile profile = gear::ComputeProfile(g);
    const GearDerived d(g);
    const std::uint32_t ring = static_cast<std::uint32_t>(profile.size());
    const int layers = g.Kz * 2 + 1;

    vertices.clear();
    indices.clear();
    vertices.reserve(static_cast<std::size_t>(layers) * ring * 3 + 6);
    indices.reserve(static_cast<std::size_t>(layers - 1) * ring * 6 + ring * 6);

    for (int layer = 0; layer < layers; ++layer) {
        const double z = -g.F + (2.0 * g.F * layer) / (layers - 1);
        const double fraction = std::abs(z) / g.F;
        const double twist = fraction * g.F * std::tan(gear::DegToRad(g.beta)) / d.r;
        const double cs = std::cos(twist), sn = std::sin(twist);
        for (const Point& p : profile) {
            vertices.push_back(static_cast<float>(p.x * cs - p.y * sn));
            vertices.push_back(static_cast<float>(p.x * sn + p.y * cs));
            vertices.push_back(static_cast<float>(z));
        }
    }

    for (int layer = 0; layer < layers - 1; ++layer) {
        const std::uint32_t base = static_cast<std::uint32_t>(layer) * ring;
        const std::uint32_t next = base + ring;
        for (std::uint32_t j = 0; j < ring; ++j) {
            const std::uint32_t jn = (j + 1) % ring;
            appendTriangle(base + j, base + jn, next + jn);
            appendTriangle(base + j, next + jn, next + j);
        }
    }

    // The involute outline is radial and star-shaped around the gear axis, so
    // center fans close both ends without requiring a general polygon library.
    const std::uint32_t bottomCenter = static_cast<std::uint32_t>(vertices.size() / 3);
    vertices.insert(vertices.end(), {0.0f, 0.0f, static_cast<float>(-g.F)});
    const std::uint32_t topCenter = static_cast<std::uint32_t>(vertices.size() / 3);
    vertices.insert(vertices.end(), {0.0f, 0.0f, static_cast<float>(g.F)});
    const std::uint32_t top = static_cast<std::uint32_t>(layers - 1) * ring;
    for (std::uint32_t j = 0; j < ring; ++j) {
        const std::uint32_t jn = (j + 1) % ring;
        appendTriangle(bottomCenter, jn, j);
        appendTriangle(topCenter, top + j, top + jn);
    }
}

template <typename Fn>
int generateText(Fn&& fn)
{
    try {
        result_text = fn();
        last_error.clear();
        return 1;
    } catch (const std::exception& e) {
        result_text.clear();
        last_error = e.what();
        return 0;
    }
}

} // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE int gear_build_mesh(
    int z, double m, double alpha, double beta, double face,
    int kt, int ka, int kr, int kz, double x, double rg)
{
    const GearParams g = makeGear(z, m, alpha, beta, face, kt, ka, kr, kz, x, rg);
    if (!validate(g)) return 0;
    try { buildMesh(g); return 1; }
    catch (const std::exception& e) { last_error = e.what(); return 0; }
}

EMSCRIPTEN_KEEPALIVE const float* gear_vertices() { return vertices.data(); }
EMSCRIPTEN_KEEPALIVE std::size_t gear_vertex_count() { return vertices.size() / 3; }
EMSCRIPTEN_KEEPALIVE const std::uint32_t* gear_indices() { return indices.data(); }
EMSCRIPTEN_KEEPALIVE std::size_t gear_index_count() { return indices.size(); }
EMSCRIPTEN_KEEPALIVE const char* gear_last_error() { return last_error.c_str(); }

EMSCRIPTEN_KEEPALIVE int gear_generate_roughing(
    int z, double m, double alpha, double beta, double face,
    int kt, int ka, int kr, int kz, double x, double rg,
    double depth, double cutter, double remain, int teeth, int tool)
{
    const GearParams g = makeGear(z, m, alpha, beta, face, kt, ka, kr, kz, x, rg);
    if (!validate(g)) return 0;
    RoughParams p;
    p.layer_depth = depth; p.cutter_diameter = cutter; p.remain = remain;
    p.teeth_count = teeth; p.tool_number = tool;
    if (depth <= 0 || cutter <= 0 || remain < 0 || teeth < 1) {
        last_error = "粗加工参数必须为正值"; return 0;
    }
    return generateText([&] { return generateRoughing(g, p).ToString(); });
}

EMSCRIPTEN_KEEPALIVE int gear_generate_finishing(
    int z, double m, double alpha, double beta, double face,
    int kt, int ka, int kr, int kz, double x, double rg,
    double depth, double cutter, double remain, int teeth,
    double cutterHeight, double ra, int tool)
{
    const GearParams g = makeGear(z, m, alpha, beta, face, kt, ka, kr, kz, x, rg);
    if (!validate(g)) return 0;
    FinishParams p;
    p.layer_depth = depth; p.cutter_diameter = cutter; p.remain = remain;
    p.teeth_count = teeth; p.h_cutter = cutterHeight; p.Ra = ra; p.tool_number = tool;
    if (depth <= 0 || cutter <= 0 || remain < 0 || teeth < 1 || cutterHeight <= 0 || ra <= 0) {
        last_error = "精加工参数必须为正值"; return 0;
    }
    return generateText([&] { return generateFinishing(g, p).ToString(); });
}

EMSCRIPTEN_KEEPALIVE const char* gear_text() { return result_text.c_str(); }
EMSCRIPTEN_KEEPALIVE std::size_t gear_text_size() { return result_text.size(); }

} // extern "C"
