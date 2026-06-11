#pragma once

#include "gear_params.h"
#include "cam_generate.h"
#include <QMainWindow>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>

class QVTKOpenGLNativeWidget;
class QGroupBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

protected:
    // Re-apply the theme when the OS switches between light/dark mode.
    void changeEvent(QEvent* event) override;

private slots:
    void onGenerateGeometry();
    void onSaveGeometry();
    void onGenerateCAM();

private:
    // Parameter spinboxes
    QSpinBox*     spinTeeth_;
    QDoubleSpinBox* spinModule_;
    QDoubleSpinBox* spinAlpha_;
    QDoubleSpinBox* spinBeta_;
    QDoubleSpinBox* spinFaceWidth_;
    QSpinBox*     spinKt_;
    QSpinBox*     spinKa_;
    QSpinBox*     spinKr_;
    QSpinBox*     spinKz_;
    QDoubleSpinBox*     spinRg_;
    QDoubleSpinBox*     spinX_;

    // Shared CAM parameter
    QDoubleSpinBox* spinCamRemain_;

    // Roughing parameter spinboxes
    QDoubleSpinBox* spinRoughDepth_;
    QDoubleSpinBox* spinRoughCutter_;
    QSpinBox*       spinRoughTool_;
    QSpinBox*       spinCutTeeth_;

    // Finishing parameter spinboxes
    QDoubleSpinBox* spinFinishDepth_;
    QDoubleSpinBox* spinFinishCutter_;
    QDoubleSpinBox* spinFinishH_;
    QDoubleSpinBox* spinFinishRa_;
    QSpinBox*       spinFinishTool_;

    QPushButton* btnGeometry_;
    QPushButton* btnSaveGeometry_;
    QPushButton* btnCAM_;
    QLabel*      statusLabel_;
    QLabel*      statusIcon_;   // busy / done / error glyph left of the text

    // Status-bar state; setStatus() updates the icon and text together.
    enum class Status { Idle, Busy, Done, Error };
    void setStatus(const QString& text, Status state);

    // Stored meshes for export
    vtkSmartPointer<vtkPolyData> gearMesh_;
    vtkSmartPointer<vtkPolyData> stockMesh_;

    // VTK
    QVTKOpenGLNativeWidget*               vtkWidget_;
    vtkSmartPointer<vtkRenderer>          renderer_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow_;

    void setupUi();
    QGroupBox* createGeometryPanel();  // gear parameters + generate/save buttons
    QGroupBox* createRoughPanel();     // roughing parameters (+ shared remain/teeth)
    QGroupBox* createFinishPanel();    // finishing parameters
    void       setupViewport();        // VTK widget, renderer, render window
    void applyTheme();   // light/dark stylesheet based on the system palette
    bool applyingTheme_ = false;  // guards against re-entrant theme changes

    GearParams readParams() const;
    void readGeometryParams(GearParams& g) const;   // shape: z, m, α, β, F, x, Rg
    void readPrecisionParams(GearParams& g) const;  // tessellation: Kt, Ka, Kr, Kz

    // Returns true when the current gear parameters are valid;
    // otherwise shows an error dialog and returns false.
    bool validateParams(const GearParams& g);

    RoughParams  readRoughParams() const;
    FinishParams readFinishParams() const;
};
