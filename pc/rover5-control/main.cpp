#include "rover5control.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CRover5Control w;
    w.show();
    
    return a.exec();
}
