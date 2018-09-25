TEMPLATE = lib
CONFIG += static
TARGET = renderlib
QT += core gui quick widgets
CONFIG += c++11
#LIBS += -lGLESv2
SOURCES += \
    axisalignedboundingbox.cpp \
    chartrenderer.cpp


HEADERS += \
    axisalignedboundingbox.h \
    chartrenderer.h

DISTFILES += \
    lib.pri

RESOURCES += \
    img.qrc \
    shaders.qrc
