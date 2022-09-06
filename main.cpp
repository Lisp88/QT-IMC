#include "mychatdialog.h"

#include <QApplication>
#include "kernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Kernel app;
    return a.exec();
}
