TEMPLATE = app
TARGET = test_lexer
QT += core
CONFIG += console
CONFIG -= app_bundle

SOURCES += lexertest.cpp

INCLUDEPATH += ../include

LIBS += -L../build
