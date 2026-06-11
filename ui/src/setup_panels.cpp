#include "mainwindow.h"

#include <QFormLayout>
#include <QGroupBox>

#include <QVTKOpenGLNativeWidget.h>

// ── Region builders ───────────────────────────────────────────────────────
//
// Each function builds one region of the main window and returns its top-level
// widget; setupUi() (in mainwindow.cpp) arranges them into the final layout.

// Left CAD column: gear geometry / precision parameters and the model buttons.
QGroupBox* MainWindow::createGeometryPanel()
{
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

    // Qt default step 1.0 is too coarse for these double parameters.
    for (auto* spin : { spinModule_, spinAlpha_, spinBeta_, spinFaceWidth_,
                        spinX_, spinRg_ })
        spin->setSingleStep(0.1);

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

    return paramBox;
}

// Roughing parameters. Also creates the CAM parameters shared with finishing
// (remaining stock and cut-tooth count), which are read by both stages.
QGroupBox* MainWindow::createRoughPanel()
{
    const RoughParams rd;

    auto* roughBox  = new QGroupBox("粗加工参数");
    auto* roughForm = new QFormLayout(roughBox);
    roughForm->setVerticalSpacing(8);
    roughForm->setHorizontalSpacing(8);
    roughForm->setContentsMargins(8, 12, 8, 8);

    spinRoughDepth_  = new QDoubleSpinBox; spinRoughDepth_->setRange(0.01, 50.0);  spinRoughDepth_->setValue(rd.layer_depth);  spinRoughDepth_->setSuffix(" mm");
    spinRoughCutter_ = new QDoubleSpinBox; spinRoughCutter_->setRange(0.5, 50.0);  spinRoughCutter_->setValue(rd.cutter_diameter); spinRoughCutter_->setSuffix(" mm");
    spinRoughTool_   = new QSpinBox;       spinRoughTool_->setRange(1, 999);       spinRoughTool_->setValue(rd.tool_number);
    spinCutTeeth_  = new QSpinBox;       spinCutTeeth_->setRange(1, 200);      spinCutTeeth_->setValue(rd.teeth_count);
    spinCamRemain_ = new QDoubleSpinBox; spinCamRemain_->setRange(0, 10.0);    spinCamRemain_->setValue(rd.remain); spinCamRemain_->setSuffix(" mm");

    for (auto* spin : { spinRoughDepth_, spinRoughCutter_, spinCamRemain_ })
        spin->setSingleStep(0.1);

    roughForm->addRow("刀具序号:",     spinRoughTool_);
    roughForm->addRow("切削深度:",     spinRoughDepth_);
    roughForm->addRow("刀具直径:", spinRoughCutter_);
    roughForm->addRow("粗加工余量:",    spinCamRemain_);

    return roughBox;
}

// Finishing parameters plus the shared CAM action button and status label.
QGroupBox* MainWindow::createFinishPanel()
{
    const FinishParams fd;

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
    for (auto* spin : { spinFinishDepth_, spinFinishCutter_,
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

    connect(btnCAM_, &QPushButton::clicked, this, &MainWindow::onGenerateCAM);

    return finishBox;
}

// Right panel: VTK viewport (render window + renderer).
void MainWindow::setupViewport()
{
    vtkWidget_    = new QVTKOpenGLNativeWidget;
    renderWindow_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer_     = vtkSmartPointer<vtkRenderer>::New();

    renderWindow_->AddRenderer(renderer_);
    vtkWidget_->setRenderWindow(renderWindow_);

    renderer_->SetBackground(0.6, 0.6, 0.6);
    renderer_->AutomaticLightCreationOn();
}
