#include "mainwindow.h"

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