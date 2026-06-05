#include "stock.h"

#include <vtkNew.h>
#include <vtkAppendPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkSTLWriter.h>
#include <vtkTriangleFilter.h>

void createStock(GearParams g) {
    auto cylinder = vtkCylinderSource::New();
    double r  = g.m * g.z / 2.0;
    double ra = r  + g.m;
    cylinder->SetRadius(ra);
    cylinder->SetHeight(2*g.F);
    cylinder->SetCenter(0.0, -g.F, 0.0);
    cylinder->SetResolution(64);   // number of sides around circumference
    cylinder->SetCapping(true);    // close top and bottom faces
    cylinder->Update();

    vtkNew<vtkTriangleFilter> triangulate;
    triangulate->SetInputConnection(cylinder->GetOutputPort());
    triangulate->Update();

    vtkNew<vtkSTLWriter> writer;
    writer->SetFileName("gear_stock.stl");
    writer->SetInputConnection(triangulate->GetOutputPort());
    writer->SetFileTypeToBinary();
    writer->Write();
}