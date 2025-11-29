QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Use console application configuration
CONFIG -= windows
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/mainwindow.cpp \
    src/regexprocessor.cpp \
    src/nfabuilder.cpp \
    src/dfabuilder.cpp \
    src/dfaminimizer.cpp \
    src/lexergenerator.cpp \
    src/lexertester.cpp

HEADERS += \
    include/mainwindow.h \
    include/regexprocessor.h \
    include/nfabuilder.h \
    include/dfabuilder.h \
    include/dfaminimizer.h \
    include/lexergenerator.h \
    include/lexertester.h

FORMS += \
    ui/mainwindow.ui

INCLUDEPATH += include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
