#include "stock.h"

#include <vtkNew.h>
#include <vtkTriangleFilter.h>
#include <vtkCylinderSource.h>
#include <vtkSTLWriter.h>

vtkSmartPointer<vtkPolyData> buildStockMesh(const GearParams& g)
{
    double ra = g.m * g.z / 2.0 + g.m;

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetRadius(ra);
    cylinder->SetHeight(2 * g.F);
    cylinder->SetCenter(0.0, -g.F, 0.0);
    cylinder->SetResolution(64);
    cylinder->SetCapping(true);
    cylinder->Update();

    vtkNew<vtkTriangleFilter> tri;
    tri->SetInputConnection(cylinder->GetOutputPort());
    tri->Update();

    return tri->GetOutput();
}

void createStock(GearParams g)
{
    auto mesh = buildStockMesh(g);

    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName("gear_stock.stl");
    writer->SetInputData(mesh);
    writer->SetFileTypeToBinary();
    writer->Write();
}
