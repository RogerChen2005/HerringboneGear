#include "mainwindow.h"
#include "gear_params.h"
#include "cam_generate.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

// ── Generate CAM Code ────────────────────────────────────────────────────────
void MainWindow::onGenerateCAM()
{
    GearParams g = readParams();
    auto rp = readRoughParams();
    auto fp = readFinishParams();

    try {
        // ── Roughing ────────────────────────────────────────────────────────
        statusLabel_->setText("Generating roughing toolpath...");
        QApplication::processEvents();

        auto rough = generateRoughing(g, rp.teeth_count, rp.layer_depth, rp.cutter_diameter, rp.remain);

        QString roughPath = QFileDialog::getSaveFileName(
            this, "Save Roughing NC", "rough.nc", "NC Files (*.nc)");
        if (!roughPath.isEmpty()) {
            rough.WriteToFile(roughPath.toUtf8().constData());
        }

        // ── Finishing ───────────────────────────────────────────────────────
        statusLabel_->setText("Generating finishing toolpath...");
        QApplication::processEvents();

        auto finish = generateFinishing(g, fp.teeth_count, fp.layer_depth, fp.cutter_diameter, fp.remain, fp.h_cutter, fp.Ra);

        QString finishPath = QFileDialog::getSaveFileName(
            this, "Save Finishing NC", "finish.nc", "NC Files (*.nc)");
        if (!finishPath.isEmpty()) {
            finish.WriteToFile(finishPath.toUtf8().constData());
        }
    } catch (const std::exception& e) {
        statusLabel_->setText("CAM failed.");
        QMessageBox::critical(this, "CAM Error", e.what());
        return;
    }

    statusLabel_->setText("CAM done.");
}