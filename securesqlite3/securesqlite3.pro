TEMPLATE = lib
CONFIG += staticlib
CONFIG += debug_and_release

TARGET = securesqlite3
win32 {
    DEFINES -= UNICODE \
        _UNICODE
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

DEFINES +=  SQLITE_THREADSAFE=0 \
    SQLITE_OS_OTHER \
    SQLITE_OS_SECURE

#CONFIG(NO_SECUBE){ # if no se_cube connected, use non encrypted sqlite3
##    DEFINES += SQLITE_OS_UNIX
##    LIBS += -ldl

#} else {
#    DEFINES += SECUBE
#    DEFINES += SQLITE_OS_SECURE # used for secuereqlite files
#    DEFINES += SQLITE_OS_OTHER
#}


#LIBS += -ldl
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

