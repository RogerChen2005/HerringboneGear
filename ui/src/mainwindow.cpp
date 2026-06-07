#include "mainwindow.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QApplication>

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

    renderer_->SetBackground(0.6, 0.6, 0.6);
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