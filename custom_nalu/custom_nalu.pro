TARGET = custom_nalu
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

include('../common/common.pri')


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui
