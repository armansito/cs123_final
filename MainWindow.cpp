#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Databinding.h"
#include "Settings.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    #define BIND(b) { DataBinding *_b = (b); }//connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged())); }

    BIND( FloatBinding::bindSliderAndTextbox(ui->amplitudeSlider, ui->amplitudeDisplay, settings.ripple_amplitude, 0, 3) )
    BIND( FloatBinding::bindSliderAndTextbox(ui->energySlider, ui->energyDisplay, settings.ripple_energy, 0, 96) )
    BIND( FloatBinding::bindSliderAndTextbox(ui->speedSlider, ui->speedDisplay, settings.ripple_speed, 0, 96) )
}

MainWindow::~MainWindow() {
    delete ui;
}
