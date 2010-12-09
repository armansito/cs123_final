#include <QtGui/QApplication>
#include <iostream>
#ifdef __APPLE__
#include <QtOpenGL/qgl.h>
#include <util.h>
#else
#include <qgl.h>
#include <pty.h>
#endif
#include "MainWindow.h"
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    cout << "\033[2J\033[0;0H\033[01;34mCS123 Lab 09 2010\033[0m" << endl;
    MainWindow w;
    w.show();
    return a.exec();
}
