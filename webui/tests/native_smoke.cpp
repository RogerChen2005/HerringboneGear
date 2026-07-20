#include <cstddef>
#include <cstdint>
#include <iostream>

extern "C" {
int gear_build_mesh(int, double, double, double, double, int, int, int, int, double, double);
const float* gear_vertices();
std::size_t gear_vertex_count();
const std::uint32_t* gear_indices();
std::size_t gear_index_count();
const char* gear_last_error();
int gear_generate_roughing(int, double, double, double, double, int, int, int, int,
                           double, double, double, double, double, int, int);
const char* gear_text();
std::size_t gear_text_size();
}

int main()
{
    const int z = 36, kt = 16, ka = 6, kr = 8, kz = 28;
    const double m = 15, alpha = 20, beta = 25, face = 100, x = 0, rg = 10;
    if (!gear_build_mesh(z, m, alpha, beta, face, kt, ka, kr, kz, x, rg)) {
        std::cerr << gear_last_error() << '\n';
        return 1;
    }
    if (!gear_vertices() || !gear_indices() || gear_vertex_count() < 3 || gear_index_count() < 3)
        return 2;
    if (!gear_generate_roughing(z, m, alpha, beta, face, kt, ka, kr, kz, x, rg,
                                3, 6, .5, 1, 1)) {
        std::cerr << gear_last_error() << '\n';
        return 3;
    }
    if (!gear_text() || gear_text_size() == 0)
        return 4;
    std::cout << gear_vertex_count() << " vertices, "
              << gear_index_count() / 3 << " triangles, "
              << gear_text_size() << " NC bytes\n";
}
