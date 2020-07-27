#include "mainwindow.h"
#include "globals.h"
#include "ratingsystem.h"

#include <QApplication>
#include <QDebug>
//MainWindow *main_window; //CHANGED FROM DEFAULT TO GIVE THIS GLOBAL POINTER T0 MAIN WINDOW (bad?)
MainWindow *main_window;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    main_window = new MainWindow();
    main_window->show();

    int result =  a.exec();
    delete main_window;
    main_window = NULL;

    return result;

}
