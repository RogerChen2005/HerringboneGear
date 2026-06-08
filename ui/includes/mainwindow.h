#pragma once

#include "gear_params.h"
#include <QMainWindow>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

#include <vtkSmartPointer.h>
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

    QPushButton* btnGeometry_;
    QPushButton* btnCAM_;
    QLabel*      statusLabel_;

    // VTK
    QVTKOpenGLNativeWidget*               vtkWidget_;
    vtkSmartPointer<vtkRenderer>          renderer_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow_;

    void setupUi();
    GearParams readParams() const;
};
