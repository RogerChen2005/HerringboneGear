#include "gear_geometry.h"
#include "sweep.h"
#include "gear_params.h"

#include <vtkNew.h>
#include <vtkAppendPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkSTLWriter.h>
#include <vtkMath.h>

void createHerringboneGear(const GearParams& g) {
    // 1. Compute 2D cross-section (all z teeth)
    Profile profile = gear::ComputeProfile(g);

    // 2. Helical sweep — front half (z: 0 → +F)
    auto half1 = sweepHalf(profile, g, +1.0);

    // 3. Helical sweep — back half  (z: 0 → -F)
    auto half2 = sweepHalf(profile, g, -1.0);

    // 4. Merge both halves
    vtkNew<vtkAppendPolyData> append;
    append->AddInputData(half1);
    append->AddInputData(half2);

    // 5a. Triangulate quads → triangles
    vtkNew<vtkTriangleFilter> tri;
    tri->SetInputConnection(append->GetOutputPort());

    // 5b. Merge the shared centre ring (k=0 of both halves is identical)
    vtkNew<vtkCleanPolyData> clean;
    clean->SetInputConnection(tri->GetOutputPort());
    clean->SetTolerance(1e-5);

    // 5c. Fill the two open end faces with Delaunay triangulation
    double ra = g.m * g.z / 2.0 + g.m;
    vtkNew<vtkFillHolesFilter> fill;
    fill->SetInputConnection(clean->GetOutputPort());
    fill->SetHoleSize(ra * ra * vtkMath::Pi() * 1.5);

    // 5d. Recompute consistent outward normals
    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(fill->GetOutputPort());
    normals->ConsistencyOn();
    normals->AutoOrientNormalsOn();
    normals->SplittingOff();

    // 6. Export binary STL
    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName("herringbone_gear.stl");
    writer->SetInputConnection(normals->GetOutputPort());
    writer->SetFileTypeToBinary();
    writer->Write();
}