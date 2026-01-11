#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Pharmacy Management System");
    window.resize(900, 650);
    window.show();

    return app.exec();
}
