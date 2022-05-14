#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UnrealLiveLinkCInterfaceAPI.h"
#include <QCloseEvent>
#include <QTimer>
#include "math.h"

struct Quaternion
{
    double w, x, y, z;
};




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void closeEvent (QCloseEvent *event);
    ~MainWindow();

private slots:


    void on_btnSetCameraStructure_clicked();
    void on_updateCameraFrame();

private:
    Ui::MainWindow *ui;
    struct UnrealLiveLink_CameraStatic m_CameraStatic_structure;
    struct UnrealLiveLink_Camera m_Camera_structure;
    const char *m_cameraName = "KFXCamera";
    double m_worldTime = 0.0;
    double m_updateCameraFreq = 24;
    QTimer *m_timer;


};
#endif // MAINWINDOW_H
