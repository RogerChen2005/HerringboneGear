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

    auto* paramBox  = new QGroupBox("齿轮参数");
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
    spinX_         = new QDoubleSpinBox; spinX_->setRange(0, 1);      spinX_->setValue(0);
    spinRg_        = new QDoubleSpinBox; spinRg_->setRange(0, 50);      spinRg_->setValue(38);

    form->addRow("齿数 (z):",        spinTeeth_);
    form->addRow("模数 (m):",       spinModule_);
    form->addRow("压力角:",   spinAlpha_);
    form->addRow("旋转角:",      spinBeta_);
    form->addRow("半高:",  spinFaceWidth_);
    form->addRow("Kt (齿面精度):",    spinKt_);
    form->addRow("Ka (齿顶精度):",     spinKa_);
    form->addRow("Kr (齿底精度):", spinKr_);
    form->addRow("Kz (高度精度):",       spinKz_);
    form->addRow("X (0-1):",          spinX_);
    form->addRow("连接圆弧半径:",   spinRg_);

    btnGeometry_    = new QPushButton("生成模型");
    btnSaveGeometry_ = new QPushButton("保存模型");
    btnSaveGeometry_->setEnabled(false);
    form->addRow(btnGeometry_);
    form->addRow(btnSaveGeometry_);

    connect(btnGeometry_,    &QPushButton::clicked, this, &MainWindow::onGenerateGeometry);
    connect(btnSaveGeometry_, &QPushButton::clicked, this, &MainWindow::onSaveGeometry);

    // ── Shared CAM parameter ────────────────────────────────────────────────

    spinCamRemain_ = new QDoubleSpinBox; spinCamRemain_->setRange(0, 10.0); spinCamRemain_->setValue(0.5); spinCamRemain_->setSuffix(" mm");

    // ── Roughing parameters ─────────────────────────────────────────────────

    auto* roughBox  = new QGroupBox("粗加工参数");
    auto* roughForm = new QFormLayout(roughBox);

    spinRoughDepth_  = new QDoubleSpinBox; spinRoughDepth_->setRange(0.01, 50.0);  spinRoughDepth_->setValue(3.0);  spinRoughDepth_->setSuffix(" mm");
    spinRoughCutter_ = new QDoubleSpinBox; spinRoughCutter_->setRange(0.5, 50.0);  spinRoughCutter_->setValue(6.0); spinRoughCutter_->setSuffix(" mm");
    spinCutTeeth_  = new QSpinBox;       spinCutTeeth_->setRange(1, 200);      spinCutTeeth_->setValue(1);

    roughForm->addRow("切削深度:",     spinRoughDepth_);
    roughForm->addRow("刀具直径:", spinRoughCutter_);
    roughForm->addRow("粗加工余量:",    spinCamRemain_);

    // ── Finishing parameters ────────────────────────────────────────────────

    auto* finishBox  = new QGroupBox("精加工参数");
    auto* finishForm = new QFormLayout(finishBox);

    spinFinishDepth_  = new QDoubleSpinBox; spinFinishDepth_->setRange(0.01, 50.0);  spinFinishDepth_->setValue(0.2);  spinFinishDepth_->setSuffix(" mm");
    spinFinishCutter_ = new QDoubleSpinBox; spinFinishCutter_->setRange(0.5, 50.0);  spinFinishCutter_->setValue(4.0); spinFinishCutter_->setSuffix(" mm");
    spinFinishH_      = new QDoubleSpinBox; spinFinishH_->setRange(0.1, 100.0);     spinFinishH_->setValue(25.0);    spinFinishH_->setSuffix(" mm");
    spinFinishRa_     = new QDoubleSpinBox; spinFinishRa_->setRange(0.001, 1.0);    spinFinishRa_->setValue(0.064);  spinFinishRa_->setSuffix(" mm");

    finishForm->addRow("切削深度:",     spinFinishDepth_);
    finishForm->addRow("刀具直径:", spinFinishCutter_);
    finishForm->addRow("刀具高度:",   spinFinishH_);
    finishForm->addRow("Ra (粗糙度):",  spinFinishRa_);

    btnCAM_      = new QPushButton("生成 CAM 代码");
    statusLabel_ = new QLabel("Ready");

    connect(btnCAM_, &QPushButton::clicked, this, &MainWindow::onGenerateCAM);

    // ── Right panel: VTK viewport ────────────────────────────────────────────

    vtkWidget_    = new QVTKOpenGLNativeWidget;
    renderWindow_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer_     = vtkSmartPointer<vtkRenderer>::New();

    renderWindow_->AddRenderer(renderer_);
    vtkWidget_->setRenderWindow(renderWindow_);

    renderer_->SetBackground(0.6, 0.6, 0.6);
    renderer_->AutomaticLightCreationOn();

    // ── Splitter layout ──────────────────────────────────────────────────────

    auto* leftPanel = new QWidget;
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(paramBox);
    leftLayout->addWidget(roughBox);
    leftLayout->addWidget(finishBox);

    auto label = new QLabel("切割齿数");
    leftLayout->addWidget(label);
    leftLayout->addWidget(spinCutTeeth_);
    leftLayout->addWidget(btnCAM_);
    leftLayout->addWidget(statusLabel_);
    leftLayout->addStretch();

    auto* splitter = new QSplitter;
    leftPanel->setMaximumWidth(320);
    splitter->addWidget(leftPanel);
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
    g.x     = spinX_->value();
    g.Rg    = spinRg_->value();
    return g;
}

MainWindow::RoughParams MainWindow::readRoughParams() const
{
    RoughParams c;
    c.layer_depth     = spinRoughDepth_->value();
    c.cutter_diameter = spinRoughCutter_->value();
    c.remain          = spinCamRemain_->value();
    c.teeth_count     = spinCutTeeth_->value();
    return c;
}

MainWindow::FinishParams MainWindow::readFinishParams() const
{
    FinishParams c;
    c.layer_depth     = spinFinishDepth_->value();
    c.cutter_diameter = spinFinishCutter_->value();
    c.remain          = spinCamRemain_->value();
    c.teeth_count     = spinCutTeeth_->value();
    c.h_cutter        = spinFinishH_->value();
    c.Ra              = spinFinishRa_->value();
    return c;
}