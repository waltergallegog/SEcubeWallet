TEMPLATE = lib
CONFIG += staticlib
CONFIG += debug_and_release

TARGET = zxcvbn


SOURCES += \
    dict-generate.cpp \
    zxcvbn.cpp

HEADERS += \
    zxcvbn.h \
    dict-src.h

DISTFILES += \
    makefile
