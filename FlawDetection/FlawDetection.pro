QT += core widgets quick quickcontrols2
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



RESOURCES += qml.qrc \
    shader.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



unix:!macx: LIBS += -L$$PWD/../../../qt-sdk/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/ -lftd2xx

INCLUDEPATH += $$PWD/../../../qt-sdk/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include
DEPENDPATH += $$PWD/../../../qt-sdk/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include

HEADERS += \
    src/appmanager.h \
    src/ascan.h \
    src/ft2232hreadworker.h \
    src/ft2232hworker.h \
    src/ft2232hwrapper.h \
    src/ftd2xx.h \
    src/micros.h \
    src/WinTypes.h \
    src/workingthread.h \
    src/ascanfbo.h \
    src/chartrenderer.h \
    src/ascanfborenderer.h \
    src/batteryworker.h

SOURCES += \
    src/appmanager.cpp \
    src/ascan.cpp \
    src/ft2232hreadworker.cpp \
    src/ft2232hworker.cpp \
    src/ft2232hwrapper.cpp \
    src/main.cpp \
    src/workingthread.cpp \
    src/ascanfbo.cpp \
    src/chartrenderer.cpp \
    src/ascanfborenderer.cpp \
    src/batteryworker.cpp

TRANSLATIONS += res/translations/la_en.ts \
        res/translations/la_cn.ts

