#pragma once

#include "gear_params.h"
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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

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
    QSpinBox*       spinCutTeeth_;

    // Finishing parameter spinboxes
    QDoubleSpinBox* spinFinishDepth_;
    QDoubleSpinBox* spinFinishCutter_;
    QDoubleSpinBox* spinFinishH_;
    QDoubleSpinBox* spinFinishRa_;

    QPushButton* btnGeometry_;
    QPushButton* btnSaveGeometry_;
    QPushButton* btnCAM_;
    QLabel*      statusLabel_;

    // Stored meshes for export
    vtkSmartPointer<vtkPolyData> gearMesh_;
    vtkSmartPointer<vtkPolyData> stockMesh_;

    // VTK
    QVTKOpenGLNativeWidget*               vtkWidget_;
    vtkSmartPointer<vtkRenderer>          renderer_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow_;

    void setupUi();
    GearParams readParams() const;

    struct RoughParams {
        double layer_depth;
        double cutter_diameter;
        double remain;
        int    teeth_count;
    };
    struct FinishParams {
        double layer_depth;
        double cutter_diameter;
        double remain;
        int    teeth_count;
        double h_cutter;
        double Ra;
    };
    RoughParams  readRoughParams() const;
    FinishParams readFinishParams() const;
};
