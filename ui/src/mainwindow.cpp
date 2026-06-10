#include "mainwindow.h"
#include "gear_geometry.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QApplication>
#include <QMessageBox>

#include <QVTKOpenGLNativeWidget.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("Herringbone Gear — CAD / CAM");
    resize(1300, 600);
}

// ── UI construction ─────────────────────────────────────────────────────────

void MainWindow::setupUi()
{
    auto* central = new QWidget(this);
    auto* hbox    = new QHBoxLayout(central);

    // Taller input fields / buttons with more breathing room.
    central->setStyleSheet(
        "QSpinBox, QDoubleSpinBox { min-height: 22px; padding: 2px 4px; }"
        "QPushButton { min-height: 22px; padding: 4px 8px; }");

    // ── Left panel: parameters ───────────────────────────────────────────────

    auto* paramBox  = new QGroupBox("齿轮参数");
    auto* form      = new QFormLayout(paramBox);
    form->setVerticalSpacing(8);
    form->setHorizontalSpacing(8);
    form->setContentsMargins(8, 12, 8, 8);

    const GearParams gd;   // single source of truth for default values

    spinTeeth_     = new QSpinBox;     spinTeeth_->setRange(6, 200);   spinTeeth_->setValue(gd.z);
    spinModule_    = new QDoubleSpinBox; spinModule_->setRange(0.1, 200); spinModule_->setValue(gd.m); spinModule_->setSuffix(" mm");
    spinAlpha_     = new QDoubleSpinBox; spinAlpha_->setRange(1, 45);   spinAlpha_->setValue(gd.alpha);  spinAlpha_->setSuffix("°");
    spinBeta_      = new QDoubleSpinBox; spinBeta_->setRange(0, 60);    spinBeta_->setValue(gd.beta);   spinBeta_->setSuffix("°");
    spinFaceWidth_ = new QDoubleSpinBox; spinFaceWidth_->setRange(1, 1000); spinFaceWidth_->setValue(gd.F); spinFaceWidth_->setSuffix(" mm");
    spinKt_        = new QSpinBox;     spinKt_->setRange(2, 200);      spinKt_->setValue(gd.Kt);
    spinKa_        = new QSpinBox;     spinKa_->setRange(2, 200);      spinKa_->setValue(gd.Ka);
    spinKr_        = new QSpinBox;     spinKr_->setRange(2, 200);      spinKr_->setValue(gd.Kr);
    spinKz_        = new QSpinBox;     spinKz_->setRange(2, 500);      spinKz_->setValue(gd.Kz);
    spinX_         = new QDoubleSpinBox; spinX_->setRange(0, 1);      spinX_->setValue(gd.x);
    spinRg_        = new QDoubleSpinBox; spinRg_->setRange(0, 50);      spinRg_->setValue(gd.Rg);

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

    const RoughParams  rd;
    const FinishParams fd;

    spinCamRemain_ = new QDoubleSpinBox; spinCamRemain_->setRange(0, 10.0); spinCamRemain_->setValue(rd.remain); spinCamRemain_->setSuffix(" mm");

    // ── Roughing parameters ─────────────────────────────────────────────────

    auto* roughBox  = new QGroupBox("粗加工参数");
    auto* roughForm = new QFormLayout(roughBox);
    roughForm->setVerticalSpacing(8);
    roughForm->setHorizontalSpacing(8);
    roughForm->setContentsMargins(8, 12, 8, 8);

    spinRoughDepth_  = new QDoubleSpinBox; spinRoughDepth_->setRange(0.01, 50.0);  spinRoughDepth_->setValue(rd.layer_depth);  spinRoughDepth_->setSuffix(" mm");
    spinRoughCutter_ = new QDoubleSpinBox; spinRoughCutter_->setRange(0.5, 50.0);  spinRoughCutter_->setValue(rd.cutter_diameter); spinRoughCutter_->setSuffix(" mm");
    spinRoughTool_   = new QSpinBox;       spinRoughTool_->setRange(1, 999);       spinRoughTool_->setValue(rd.tool_number);
    spinCutTeeth_  = new QSpinBox;       spinCutTeeth_->setRange(1, 200);      spinCutTeeth_->setValue(rd.teeth_count);

    roughForm->addRow("刀具序号:",     spinRoughTool_);
    roughForm->addRow("切削深度:",     spinRoughDepth_);
    roughForm->addRow("刀具直径:", spinRoughCutter_);
    roughForm->addRow("粗加工余量:",    spinCamRemain_);

    // ── Finishing parameters ────────────────────────────────────────────────

    auto* finishBox  = new QGroupBox("精加工参数");
    auto* finishForm = new QFormLayout(finishBox);
    finishForm->setVerticalSpacing(8);
    finishForm->setHorizontalSpacing(8);
    finishForm->setContentsMargins(8, 12, 8, 8);

    spinFinishDepth_  = new QDoubleSpinBox; spinFinishDepth_->setRange(0.01, 50.0);  spinFinishDepth_->setValue(fd.layer_depth);  spinFinishDepth_->setSuffix(" mm");
    spinFinishCutter_ = new QDoubleSpinBox; spinFinishCutter_->setRange(0.5, 50.0);  spinFinishCutter_->setValue(fd.cutter_diameter); spinFinishCutter_->setSuffix(" mm");
    spinFinishH_      = new QDoubleSpinBox; spinFinishH_->setRange(0.1, 100.0);     spinFinishH_->setValue(fd.h_cutter);    spinFinishH_->setSuffix(" mm");
    spinFinishRa_     = new QDoubleSpinBox; spinFinishRa_->setRange(0.001, 1.0);    spinFinishRa_->setSuffix(" mm");
    spinFinishTool_   = new QSpinBox;       spinFinishTool_->setRange(1, 999);      spinFinishTool_->setValue(fd.tool_number);

    // Qt defaults: step 1.0 is too coarse for double parameters; Ra needs
    // 3 decimals (default 0.064) and a finer step.
    for (auto* spin : { spinModule_, spinAlpha_, spinBeta_, spinFaceWidth_,
                        spinX_, spinRg_, spinCamRemain_, spinRoughDepth_,
                        spinRoughCutter_, spinFinishDepth_, spinFinishCutter_,
                        spinFinishH_, spinFinishRa_ })
        spin->setSingleStep(0.1);
    spinFinishRa_->setDecimals(3);
    spinFinishRa_->setSingleStep(0.01);
    spinFinishRa_->setValue(fd.Ra);

    finishForm->addRow("刀具序号:",     spinFinishTool_);
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

    // CAD column: gear parameters
    auto* cadColumn = new QWidget;
    auto* cadLayout = new QVBoxLayout(cadColumn);
    cadLayout->setContentsMargins(4, 4, 4, 4);
    cadLayout->setSpacing(12);
    cadLayout->addWidget(paramBox);
    cadLayout->addStretch();

    // CAM column: roughing / finishing parameters
    auto* camColumn = new QWidget;
    auto* camLayout = new QVBoxLayout(camColumn);
    camLayout->setContentsMargins(4, 4, 4, 4);
    camLayout->setSpacing(12);
    camLayout->addWidget(roughBox);
    camLayout->addWidget(finishBox);

    auto label = new QLabel("切割齿数");
    camLayout->addWidget(label);
    camLayout->addWidget(spinCutTeeth_);
    camLayout->addWidget(btnCAM_);
    camLayout->addWidget(statusLabel_);
    camLayout->addStretch();

    // Left panel holds the CAD and CAM columns side by side.
    auto* leftPanel = new QWidget;
    auto* leftLayout = new QHBoxLayout(leftPanel);
    leftLayout->setContentsMargins(8, 8, 8, 8);
    leftLayout->setSpacing(16);
    leftLayout->addWidget(cadColumn);
    leftLayout->addWidget(camColumn);

    auto* splitter = new QSplitter;
    leftPanel->setMaximumWidth(680);
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

bool MainWindow::validateParams(const GearParams& g)
{
    std::string err = gear::Validate(g);
    if (err.empty()) return true;

    statusLabel_->setText("Invalid parameters.");
    QMessageBox::warning(this, "参数错误", QString::fromStdString(err));
    return false;
}

RoughParams MainWindow::readRoughParams() const
{
    RoughParams c;
    c.layer_depth     = spinRoughDepth_->value();
    c.cutter_diameter = spinRoughCutter_->value();
    c.remain          = spinCamRemain_->value();
    c.teeth_count     = spinCutTeeth_->value();
    c.tool_number     = spinRoughTool_->value();
    return c;
}

FinishParams MainWindow::readFinishParams() const
{
    FinishParams c;
    c.layer_depth     = spinFinishDepth_->value();
    c.cutter_diameter = spinFinishCutter_->value();
    c.remain          = spinCamRemain_->value();
    c.teeth_count     = spinCutTeeth_->value();
    c.h_cutter        = spinFinishH_->value();
    c.Ra              = spinFinishRa_->value();
    c.tool_number     = spinFinishTool_->value();
    return c;
}