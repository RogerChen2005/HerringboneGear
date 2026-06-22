#include "mainwindow.h"
#include "gear_geometry.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QMessageBox>
#include <QStatusBar>
#include <QPixmap>

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

    applyTheme();

    auto* paramBox  = createGeometryPanel();
    auto* roughBox  = createRoughPanel();
    auto* finishBox = createFinishPanel();
    setupViewport();

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

    statusIcon_ = new QLabel;
    statusIcon_->setAlignment(Qt::AlignCenter);
	statusIcon_->setObjectName("statusIcon");
    statusLabel_ = new QLabel("Ready");
    statusLabel_->setObjectName("statusText");

    statusBar()->addWidget(statusIcon_);
    statusBar()->addWidget(statusLabel_, 1);
    setStatus("Ready", Status::Idle);
}

// Update the status-bar icon + text. Idle shows no icon.
void MainWindow::setStatus(const QString& text, Status state)
{
    const char* icon = nullptr;
    switch (state) {
        case Status::Busy:  icon = ":/icons/status_busy.png";  break;
        case Status::Done:  icon = ":/icons/status_done.png";  break;
        case Status::Error: icon = ":/icons/status_error.png"; break;
        case Status::Idle:  break;
    }

    if (icon) {
        statusIcon_->setPixmap(QPixmap(icon).scaled(
            16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        statusIcon_->show();
    } else {
        statusIcon_->clear();
        statusIcon_->hide();
    }

    statusLabel_->setText(text);
}

// The region builders (createGeometryPanel / createRoughPanel /
// createFinishPanel / setupViewport) live in setup_panels.cpp.

// ── Parameter readout ───────────────────────────────────────────────────────

// Gear shape: tooth count, sizing and the macro geometry.
void MainWindow::readGeometryParams(GearParams& g) const
{
    g.z     = spinTeeth_->value();
    g.m     = spinModule_->value();
    g.alpha = spinAlpha_->value();
    g.beta  = spinBeta_->value();
    g.F     = spinFaceWidth_->value();
    g.x     = spinX_->value();
    g.Rg    = spinRg_->value() / std::cos(g.beta);
}

// Tessellation: how finely each feature is sampled when meshing.
void MainWindow::readPrecisionParams(GearParams& g) const
{
    g.Kt = spinKt_->value();
    g.Ka = spinKa_->value();
    g.Kr = spinKr_->value();
    g.Kz = spinKz_->value();
}

GearParams MainWindow::readParams() const
{
    GearParams g;
    readGeometryParams(g);
    readPrecisionParams(g);
    return g;
}

bool MainWindow::validateParams(const GearParams& g)
{
    std::string err = gear::Validate(g);
    if (err.empty()) return true;

    setStatus("Invalid parameters.", Status::Error);
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