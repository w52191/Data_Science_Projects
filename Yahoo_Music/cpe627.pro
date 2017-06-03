TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Bias.cpp \
    globals.cpp \
    readUserData.cpp

HEADERS += \
    Bias.h \
    globals.h \
    ratingStructures.h \
    readUserData.h

DISTFILES += \
    test.dat
