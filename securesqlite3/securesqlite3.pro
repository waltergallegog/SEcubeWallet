TEMPLATE = lib
CONFIG += staticlib
CONFIG += debug_and_release

TARGET = securesqlite3
win32 {
    DEFINES -= UNICODE \
        _UNICODE
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

DEFINES += SQLITE_OS_OTHER \
    SQLITE_OS_SECURE \
    SQLITE_THREADSAFE=0

LIBS = -lSEfile

CONFIG(debug, debug|release) {
    LIBS += -L"$$OUT_PWD/../libs/SEfile/debug/"
}
CONFIG(release, debug|release){
    LIBS += -L"$$OUT_PWD/../libs/SEfile/release/"
}



win32 {
    DEPENDPATH = "$$PWD/../SEfile"
    INCLUDEPATH += ./ \
        "$$PWD/../SEfile"
} else {
    DEPENDPATH = "$$OUT_PWD/../libs/SEfile"

    INCLUDEPATH += ./ \
        "../SEfile"
}

HEADERS += sqlite3.h

SOURCES += sqlite3.c

