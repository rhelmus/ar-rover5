#include "camclient.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CCamClient cclient;
    cclient.showMaximized();

    return app.exec();
}
