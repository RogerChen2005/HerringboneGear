#include "mainwindow.h"
#include "gear_params.h"
#include "gear_mesh.h"
#include "cam_generate.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QApplication>

#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

#include <QVTKOpenGLNativeWidget.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("Herringbone Gear — CAD / CAM");
    resize(1200, 800);
}

// ── UI construction ─────────────────────────────────────────────────────────

void MainWindow::setupUi()
{
    auto* central = new QWidget(this);
    auto* hbox    = new QHBoxLayout(central);

    // ── Left panel: parameters ───────────────────────────────────────────────

    auto* paramBox  = new QGroupBox("Gear Parameters");
    auto* form      = new QFormLayout(paramBox);

    spinTeeth_     = new QSpinBox;     spinTeeth_->setRange(6, 200);   spinTeeth_->setValue(36);
    spinModule_    = new QDoubleSpinBox; spinModule_->setRange(0.1, 200); spinModule_->setValue(15.0); spinModule_->setSuffix(" mm");
    spinAlpha_     = new QDoubleSpinBox; spinAlpha_->setRange(1, 45);   spinAlpha_->setValue(20.0);  spinAlpha_->setSuffix("°");
    spinBeta_      = new QDoubleSpinBox; spinBeta_->setRange(0, 60);    spinBeta_->setValue(25.0);   spinBeta_->setSuffix("°");
    spinFaceWidth_ = new QDoubleSpinBox; spinFaceWidth_->setRange(1, 1000); spinFaceWidth_->setValue(100.0); spinFaceWidth_->setSuffix(" mm");
    spinKt_        = new QSpinBox;     spinKt_->setRange(2, 200);      spinKt_->setValue(16);
    spinKa_        = new QSpinBox;     spinKa_->setRange(2, 200);      spinKa_->setValue(6);
    spinKr_        = new QSpinBox;     spinKr_->setRange(2, 200);      spinKr_->setValue(8);
    spinKz_        = new QSpinBox;     spinKz_->setRange(2, 500);      spinKz_->setValue(28);

    form->addRow("Teeth (z):",        spinTeeth_);
    form->addRow("Module (m):",       spinModule_);
    form->addRow("Pressure angle:",   spinAlpha_);
    form->addRow("Helix angle:",      spinBeta_);
    form->addRow("Half face width:",  spinFaceWidth_);
    form->addRow("Kt (involute):",    spinKt_);
    form->addRow("Ka (tip arc):",     spinKa_);
    form->addRow("Kr (root fillet):", spinKr_);
    form->addRow("Kz (axial):",       spinKz_);

    btnGeometry_ = new QPushButton("Generate Geometry");
    btnCAM_      = new QPushButton("Generate CAM Code");
    statusLabel_ = new QLabel("Ready");

    form->addRow(btnGeometry_);
    form->addRow(btnCAM_);
    form->addRow("Status:", statusLabel_);

    connect(btnGeometry_, &QPushButton::clicked, this, &MainWindow::onGenerateGeometry);
    connect(btnCAM_,      &QPushButton::clicked, this, &MainWindow::onGenerateCAM);

    // ── Right panel: VTK viewport ────────────────────────────────────────────

    vtkWidget_    = new QVTKOpenGLNativeWidget;
    renderWindow_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer_     = vtkSmartPointer<vtkRenderer>::New();

    renderWindow_->AddRenderer(renderer_);
    vtkWidget_->setRenderWindow(renderWindow_);

    renderer_->SetBackground(0.15, 0.15, 0.18);
    renderer_->AutomaticLightCreationOn();

    // ── Splitter layout ──────────────────────────────────────────────────────

    auto* splitter = new QSplitter;
    paramBox->setMaximumWidth(320);
    splitter->addWidget(paramBox);
    splitter->addWidget(vtkWidget_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    hbox->addWidget(splitter);
    setCentralWidget(central);
}

// ── Generate Geometry (CAD) ──────────────────────────────────────────────────

GearParams MainWindow::readParams() const
{
    GearParams g;
    g.z     = spinTeeth_->value();
    g.m     = spinModule_->value();
    g.alpha = spinAlpha_->value();
    g.beta  = spinBeta_->value();
    g.F     = spinFaceWidth_->value();
    g.Kt    = spinKt_->value();
    g.Ka    = spinKa_->value();
    g.Kr    = spinKr_->value();
    g.Kz    = spinKz_->value();
    return g;
}

void MainWindow::onGenerateGeometry()
{
    statusLabel_->setText("Generating geometry...");
    QApplication::processEvents();

    GearParams g = readParams();

    auto gearMesh  = buildGearMesh(g);
    auto stockMesh = buildStockMesh(g);

    renderer_->RemoveAllViewProps();

    // Gear actor (steel-grey, semi-transparent)
    vtkNew<vtkPolyDataMapper> gearMapper;
    gearMapper->SetInputData(gearMesh);
    vtkNew<vtkActor> gearActor;
    gearActor->SetMapper(gearMapper);
    gearActor->GetProperty()->SetColor(0.75, 0.75, 0.80);
    gearActor->GetProperty()->SetOpacity(0.85);
    gearActor->GetProperty()->SetSpecular(0.4);
    gearActor->GetProperty()->SetSpecularPower(30);
    renderer_->AddActor(gearActor);

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

    renderer_->ResetCamera();
    renderWindow_->Render();

    statusLabel_->setText("Geometry generated.");
}

// ── Generate CAM Code ────────────────────────────────────────────────────────

void MainWindow::onGenerateCAM()
{
    statusLabel_->setText("Generating CAM toolpaths...");
    QApplication::processEvents();

    GearParams g = readParams();

    auto rough  = generateRoughing(g, 3, 2.0, 2.0, 0.5);
    rough.WriteToFile("rough.nc");

    auto finish = generateFinishing(g, g.z);
    finish.WriteToFile("finish.nc");

    statusLabel_->setText("CAM done — wrote rough.nc, finish.nc");
}
