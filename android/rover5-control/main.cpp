#include <QApplication>

#include "../../control-base/rover5control.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CRover5Control w;
    w.showMaximized();

    return a.exec();
}
