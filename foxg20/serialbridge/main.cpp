#include <QCoreApplication>
#include <QStringList>

#include "serialbridge.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const QStringList args = QCoreApplication::arguments();

    if (args.size() < 2)
        qFatal("Please specify server port.");
    
    CSerialBridge sb(args.at(2), &a);

    return a.exec();
}
