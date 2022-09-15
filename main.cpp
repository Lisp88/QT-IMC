#include "DialogLogin.h"
#include <QApplication>
#include "kernel.h"
#include "DialogMain.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Kernel app;
    return a.exec();
}
