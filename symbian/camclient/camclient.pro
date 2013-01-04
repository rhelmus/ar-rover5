# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

symbian:TARGET.UID3 = 0xE7DF72CB

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
CONFIG += mobility
MOBILITY += multimedia systeminfo

SOURCES += main.cpp \
    camclient.cpp \
    videosurface.cpp \
    ../../shared/tcputils.cpp
HEADERS += \
    camclient.h \
    videosurface.h \
    ../../shared/shared.h \
    ../../shared/tcputils.h
FORMS +=

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

# Stuff for landscape orientation, see
# http://www.developer.nokia.com/Community/Wiki/CS001517_-_Lock_application_orientation_in_Qt_for_Symbian
symbian: {
    LIBS += -lcone -leikcore -lavkon
}
