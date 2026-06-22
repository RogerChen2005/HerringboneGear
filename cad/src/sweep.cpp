#include "sweep.h"
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <cmath>

vtkSmartPointer<vtkPolyData> sweepHalf(
    const Profile& profile, const GearParams& g, const GearDerived& d, double z_sign)
{
    constexpr double PI = vtkMath::Pi();
    double r    = d.r;
    double beta = g.beta * PI / 180.0;
    int M = static_cast<int>(profile.size());
    int K = g.Kz;

    vtkNew<vtkPoints>    pts;
    vtkNew<vtkCellArray> polys;
    pts->SetNumberOfPoints((K + 1) * M);

    for (int k = 0; k <= K; ++k) {
        double frac  = static_cast<double>(k) / K;
        double z     = z_sign * g.F * frac;
        double twist = frac * g.F * tan(beta) / r;
        double cs = cos(twist), sn = sin(twist);

        for (int j = 0; j < M; ++j) {
            double x = profile[j].x, y = profile[j].y;
            pts->SetPoint(k * M + j, x*cs - y*sn, x*sn + y*cs, z);
        }
    }

    // Quad lateral faces.
    // Winding is reversed for z_sign<0 so normals point consistently outward.
    for (int k = 0; k < K; ++k) {
        for (int j = 0; j < M; ++j) {
            int jn = (j + 1) % M;
            vtkIdType ids[4];
            if (z_sign > 0) {
                ids[0] = k*M + j;      ids[1] = k*M + jn;
                ids[2] = (k+1)*M + jn; ids[3] = (k+1)*M + j;
            } else {
                ids[0] = k*M + j;      ids[1] = (k+1)*M + j;
                ids[2] = (k+1)*M + jn; ids[3] = k*M + jn;
            }
            polys->InsertNextCell(4, ids);
        }
    }

    vtkNew<vtkPolyData> pd;
    pd->SetPoints(pts);
    pd->SetPolys(polys);
    return pd;
}
