#include "mainwindow.h"
#include "herringbone_gear.h"
#include "stock.h"
#include <QApplication>
#include <QFileDialog>
#include <vtkNew.h>
#include <vtkSTLWriter.h>
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
    GearParams g = readParams();
    if (!validateParams(g)) return;

    setStatus("Generating geometry...", Status::Busy);
    QApplication::processEvents();

    gearMesh_  = buildGearMesh(g);
    stockMesh_ = buildStockMesh(g);

    renderer_->RemoveAllViewProps();

    // Stock actor (orange wireframe overlay)
    vtkNew<vtkPolyDataMapper> stockMapper;
    stockMapper->SetInputData(stockMesh_);
    vtkNew<vtkActor> stockActor;
    stockActor->SetMapper(stockMapper);
    stockActor->GetProperty()->SetColor(1.0, 0.6, 0.2);
    stockActor->GetProperty()->SetOpacity(0.15);
    stockActor->GetProperty()->EdgeVisibilityOn();
    stockActor->GetProperty()->SetEdgeColor(1.0, 0.5, 0.0);
    renderer_->AddActor(stockActor);

    // Gear outline actor (steel-grey, semi-transparent)
    vtkNew<vtkOutlineFilter> outlineFilter;
    outlineFilter->SetInputData(gearMesh_);
    vtkNew<vtkPolyDataMapper> outlineMapper;
    outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
    vtkNew<vtkActor> outlineActor;
    outlineActor->SetMapper(outlineMapper);
    renderer_->AddActor(outlineActor);

    // Gear actor (steel-grey, semi-transparent)
    vtkNew<vtkPolyDataNormals> normalGen;
    normalGen->SetInputData(gearMesh_);
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

    btnSaveGeometry_->setEnabled(true);
    setStatus("Geometry generated.", Status::Done);
}

// ── Save Geometry (STL) ─────────────────────────────────────────────────────
void MainWindow::onSaveGeometry()
{
    if (!gearMesh_ && !stockMesh_) return;

    QString path = QFileDialog::getSaveFileName(
        this, "Save STL", "gear.stl", "STL Files (*.stl)");
    if (path.isEmpty()) return;

    setStatus("Saving STL...", Status::Busy);
    QApplication::processEvents();

    if (gearMesh_) {
        vtkNew<vtkSTLWriter> writer;
        writer->SetFileName(path.toUtf8().constData());
        writer->SetInputData(gearMesh_);
        writer->Write();
    }

    if (stockMesh_) {
        QString stockPath = path;
        stockPath.replace(".stl", "_stock.stl");
        vtkNew<vtkSTLWriter> writer;
        writer->SetFileName(stockPath.toUtf8().constData());
        writer->SetInputData(stockMesh_);
        writer->Write();
    }

    setStatus("STL saved: " + path, Status::Done);
}