#include "herringbone_gear.h"
#include "sweep.h"

#include <vtkNew.h>
#include <vtkAppendPolyData.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkMath.h>
#include <vtkSTLWriter.h>

vtkSmartPointer<vtkPolyData> buildGearMesh(const GearParams& g)
{
    GearDerived d(g);
    Profile profile = gear::ComputeProfile(g);

    auto half1 = sweepHalf(profile, g, d, +1.0);
    auto half2 = sweepHalf(profile, g, d, -1.0);

    vtkNew<vtkAppendPolyData> append;
    append->AddInputData(half1);
    append->AddInputData(half2);

    vtkNew<vtkTriangleFilter> tri;
    tri->SetInputConnection(append->GetOutputPort());

    vtkNew<vtkCleanPolyData> clean;
    clean->SetInputConnection(tri->GetOutputPort());
    clean->SetTolerance(1e-5);

    vtkNew<vtkFillHolesFilter> fill;
    fill->SetInputConnection(clean->GetOutputPort());
    fill->SetHoleSize(d.ra * d.ra * vtkMath::Pi() * 1.5);

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(fill->GetOutputPort());
    normals->ConsistencyOn();
    normals->AutoOrientNormalsOn();
    normals->SplittingOff();
    normals->Update();

    return normals->GetOutput();
}

void createHerringboneGear(const GearParams& g)
{
    auto mesh = buildGearMesh(g);

    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName("herringbone_gear.stl");
    writer->SetInputData(mesh);
    writer->SetFileTypeToBinary();
    writer->Write();
}
