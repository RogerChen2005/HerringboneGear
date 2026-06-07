#include "mainwindow.h"
#include "herringbone_gear.h"
#include "stock.h"
#include <QApplication>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataNormals.h>

// ── Generate Geometry (CAD) ──────────────────────────────────────────────────
void MainWindow::onGenerateGeometry()
{
    statusLabel_->setText("Generating geometry...");
    QApplication::processEvents();

    GearParams g = readParams();

    auto gearMesh  = buildGearMesh(g);
    auto stockMesh = buildStockMesh(g);

    renderer_->RemoveAllViewProps();

    // Stock actor (orange wireframe overlay)
    vtkNew<vtkPolyDataMapper> stockMapper;
    stockMapper->SetInputData(stockMesh);
    vtkNew<vtkActor> stockActor;
    stockActor->SetMapper(stockMapper);
    stockActor->GetProperty()->SetColor(1.0, 0.6, 0.2);
    stockActor->GetProperty()->SetOpacity(0.15);
    stockActor->GetProperty()->EdgeVisibilityOn();
    stockActor->GetProperty()->SetEdgeColor(1.0, 0.5, 0.0);
    renderer_->AddActor(stockActor);

    // Gear outline actor (steel-grey, semi-transparent)
    vtkNew<vtkOutlineFilter> outlineFilter;
    outlineFilter->SetInputData(gearMesh);
    vtkNew<vtkPolyDataMapper> outlineMapper;
    outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
    vtkNew<vtkActor> outlineActor;
    outlineActor->SetMapper(outlineMapper);
    renderer_->AddActor(outlineActor);

    // Gear actor (steel-grey, semi-transparent)
    vtkNew<vtkPolyDataNormals> normalGen;
    normalGen->SetInputData(gearMesh);
    normalGen->SetFeatureAngle(30.0);   // split edges sharper than 30° → keeps tooth tips / chevron apex crisp
    normalGen->SplittingOn();           // duplicate verts at feature edges
    normalGen->ConsistencyOn();         // fix inconsistent winding
    normalGen->AutoOrientNormalsOn();   // ensure outward orientation
    normalGen->Update();

    vtkNew<vtkPolyDataMapper> gearMapper;
    gearMapper->SetInputConnection(normalGen->GetOutputPort());
    vtkNew<vtkActor> gearActor;
    gearActor->SetMapper(gearMapper);
    gearActor->GetProperty()->SetColor(0.75, 0.75, 0.80);
    // gearActor->GetProperty()->SetOpacity(0.95);
    gearActor->GetProperty()->SetSpecular(0.9);
    gearActor->GetProperty()->SetSpecularPower(10);
    renderer_->AddActor(gearActor);

    renderer_->ResetCamera();
    renderWindow_->Render();

    statusLabel_->setText("Geometry generated.");
}