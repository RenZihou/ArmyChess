QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src

DEFINES += "scaleRatio=1.5" "PORT=7173" "CHEAT"

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/board.cpp \
    src/chesslabel.cpp \
    src/createserverwindow.cpp \
    src/connectserverwindow.cpp

HEADERS += \
    src/mainwindow.h \
    src/board.h \
    src/chesslabel.h \
    src/createserverwindow.h \
    src/connectserverwindow.h

FORMS += \
    src/mainwindow.ui \
    src/board.ui \
    src/createserverwindow.ui \
    src/connectserverwindow.ui

RESOURCES += \
    resources/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
