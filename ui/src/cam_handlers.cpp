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
    if (!validateParams(g)) return;

    auto rp = readRoughParams();
    auto fp = readFinishParams();

    try {
        // ── Roughing ────────────────────────────────────────────────────────
        setStatus("Generating roughing toolpath...", Status::Busy);
        QApplication::processEvents();

        auto rough = generateRoughing(g, rp);

        QString roughPath = QFileDialog::getSaveFileName(
            this, "Save Roughing NC", "rough.nc", "NC Files (*.nc)");
        if (!roughPath.isEmpty()) {
            rough.WriteToFile(roughPath.toUtf8().constData());
        }

        // ── Finishing ───────────────────────────────────────────────────────
        setStatus("Generating finishing toolpath...", Status::Busy);
        QApplication::processEvents();

        auto finish = generateFinishing(g, fp);

        QString finishPath = QFileDialog::getSaveFileName(
            this, "Save Finishing NC", "finish.nc", "NC Files (*.nc)");
        if (!finishPath.isEmpty()) {
            finish.WriteToFile(finishPath.toUtf8().constData());
        }
    } catch (const std::exception& e) {
        setStatus("CAM failed.", Status::Error);
        QMessageBox::critical(this, "CAM Error", e.what());
        return;
    }

    setStatus("CAM done.", Status::Done);
}