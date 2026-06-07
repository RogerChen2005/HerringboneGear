#include "gear_params.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool loadFromJson(GearParams& params, const std::string& path)
{
    std::ifstream file(path);
    if (!file) return false;

    json j;
    try {
        file >> j;
    } catch (const json::parse_error&) {
        return false;
    }

    if (j.contains("z"))     params.z     = j["z"].get<int>();
    if (j.contains("m"))     params.m     = j["m"].get<double>();
    if (j.contains("alpha")) params.alpha = j["alpha"].get<double>();
    if (j.contains("beta"))  params.beta  = j["beta"].get<double>();
    if (j.contains("F"))     params.F     = j["F"].get<double>();
    if (j.contains("Kt"))    params.Kt    = j["Kt"].get<int>();
    if (j.contains("Ka"))    params.Ka    = j["Ka"].get<int>();
    if (j.contains("Kr"))    params.Kr    = j["Kr"].get<int>();
    if (j.contains("Kz"))    params.Kz    = j["Kz"].get<int>();

    return true;
}
