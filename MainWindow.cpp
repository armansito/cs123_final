#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Databinding.h"
#include "Settings.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);


    #define BIND(b) { DataBinding *_b = (b); }//connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged())); }

    BIND( FloatBinding::bindSliderAndTextbox(ui->amplitudeSlider, ui->amplitudeDisplay, settings.ripple_amplitude, 0.1, 3) )
    BIND( FloatBinding::bindSliderAndTextbox(ui->energySlider, ui->energyDisplay, settings.ripple_energy, 300, 1000) )
    BIND( FloatBinding::bindSliderAndTextbox(ui->speedSlider, ui->speedDisplay, settings.ripple_speed, 1, 5) )

    settings.ripple_amplitude = 0.1;
    settings.ripple_energy = 700;
    settings.ripple_speed = 2;

    ui->amplitudeDisplay->setText(QString("0.1"));
    ui->energyDisplay->setText(QString("700"));
    ui->speedDisplay->setText(QString("1"));
}

MainWindow::~MainWindow() {
    delete ui;
}
