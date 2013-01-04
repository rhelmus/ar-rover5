#include "camclient.h"

#include <QtGui/QApplication>

#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
        if (appUi)
        {
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
        }
    );

    CCamClient cclient;
    cclient.showMaximized();

    return app.exec();
}
