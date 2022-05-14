#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "UnrealLiveLinkCInterfaceAPI.h"
#include <QApplication>
#include <QMessageBox>

# define M_PI           3.14159265358979323846  /* pi */

struct Quaternion ToQuaternion(double yaw, double pitch, double roll) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    struct Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Load Unreal Engine Library
#ifdef WIN32
    qDebug() << "WIN32";
    const wchar_t *sharedObj =L"UnrealLiveLinkCInterface.dll";
#else
    const char * sharedObj = "UnrealLiveLinkCInterface.so";
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 16 * 1000000;
#endif
    auto rc = UnrealLiveLink_Load(sharedObj, "CameraControl");
    if (rc != UNREAL_LIVE_LINK_OK)
    {
        qDebug("error: unable to load %s\n", sharedObj);
        qApp->quit();
    }
    //Init camera variable
    UnrealLiveLink_InitCameraStatic(&this->m_CameraStatic_structure);
    UnrealLiveLink_InitCamera(&this->m_Camera_structure);

    // Init periodic timer for update camera frame
    this->m_timer = new QTimer(this);
    this->m_timer->setInterval(60 % int(this->m_updateCameraFreq));
    connect(this->m_timer,&QTimer::timeout,this,&MainWindow::on_updateCameraFrame);
    this->m_timer->start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "VIRTUAL CAMERA CONTROL",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        qDebug() << "Try to unload unreal live link";
        UnrealLiveLink_Unload();
        event->accept();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_btnSetCameraStructure_clicked()
{
    this->m_CameraStatic_structure.isFieldOfViewSupported = ui->cbxIsFieldOfViewProperty->isChecked();
    this->m_CameraStatic_structure.isAspectRatioSupported = ui->cbxIsAspectRatioProperty->isChecked();
    this->m_CameraStatic_structure.isFocalLengthSupported = ui->cbxIsFocalLengthProperty->isChecked();
    this->m_CameraStatic_structure.isProjectionModeSupported = ui->cbxIsProjectionModeProperty->isChecked();
    this->m_CameraStatic_structure.filmBackWidth = -1.0f;
    this->m_CameraStatic_structure.filmBackHeight = -1.0f;
    this->m_CameraStatic_structure.isApertureSupported = ui->cbxIsApertureProperty->isChecked();
    this->m_CameraStatic_structure.isFocusDistanceSupported = ui->cbxIsFocusDistanceProperty->isChecked();

    UnrealLiveLink_SetCameraStructure(this->m_cameraName,NULL,&this->m_CameraStatic_structure);

}

void MainWindow::on_updateCameraFrame()
{
    auto x=this->ui->horizontalSlider_6->value();
    auto y=this->ui->horizontalSlider_7->value();
    auto z=this->ui->horizontalSlider_8->value();
    auto roll=float(this->ui->dial->value())/360*2*M_PI;
    auto pitch=float(this->ui->dial_2->value())/360*2*M_PI;
    auto yaw=float(this->ui->dial_3->value())/360*2*M_PI;
    struct Quaternion q = ToQuaternion(yaw,pitch,roll);
    this->m_Camera_structure.transform.rotation[0]=q.x;
    this->m_Camera_structure.transform.rotation[1]=q.y;
    this->m_Camera_structure.transform.rotation[2]=q.z;
    this->m_Camera_structure.transform.rotation[3]=q.w;
    this->m_Camera_structure.transform.translation[0]=x;
    this->m_Camera_structure.transform.translation[1]=y;
    this->m_Camera_structure.transform.translation[2]=z;
    this->m_Camera_structure.fieldOfView = this->ui->sFieldOfView->value();
    this->m_Camera_structure.aspectRatio = float(this->ui->sAspectRatio->value())/100;
    this->m_Camera_structure.focalLength = this->ui->sFocalLength->value();
    this->m_Camera_structure.aperture = float(this->ui->sAperture->value())/100;
    this->m_Camera_structure.focusDistance = this->ui->sFocusDistance->value();
    this->m_Camera_structure.isPerspective = this->ui->CbxIsPerspective->isChecked();

    UnrealLiveLink_UpdateCameraFrame(this->m_cameraName,this->m_worldTime,NULL,NULL,&this->m_Camera_structure);
    this->m_worldTime +=60 % int(this->m_updateCameraFreq);
}

