TEMPLATE = lib
CONFIG += staticlib
CONFIG += debug_and_release

TARGET = pwgen

SOURCES += \
    pw_phonemes.c \
    pw_rand.c \
    pwgen.c \
    randnum.c \
    sha1.c \
    sha1num.c

HEADERS += \
    pwgen.h \
    sha1.h
