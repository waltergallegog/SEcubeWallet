#-------------------------------------------------
#
# Project created by QtCreator 2018-01-29T20:45:15
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scr
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    environmentdialog.cpp \
    logindialog.cpp \
    securefiledialog.cpp \
    addentry.cpp \
    mysortfilterproxymodel.cpp \
    helpwindow.cpp \
    myqtableview.cpp \
    passworditemdelegate.cpp \
    filtersaligned.cpp \
    saveconfirmation.cpp \
    myapplication.cpp \
    preferencesdialog.cpp \
    clickablelabel.cpp \
    passwordInfo.cpp \
    dictlist.cpp \
    edituserdict.cpp \
    userwordslist.cpp \
    passphrasegen.cpp
    co

HEADERS  += mainwindow.h \
    environmentdialog.h \
    logindialog.h \
    securefiledialog.h \
    addentry.h \
    mysortfilterproxymodel.h \
    helpwindow.h \
    myqtableview.h \
    passworditemdelegate.h \
    filtersaligned.h \
    saveconfirmation.h \
    myapplication.h \
    preferencesdialog.h \
    clickablelabel.h \
    passwordInfo.h \
    dictlist.h \
    edituserdict.h \
    userwordslist.h \
    passphrasegen.h

FORMS    += mainwindow.ui \
    addentry.ui \
    helpwindow.ui \
    saveconfirmation.ui \
    preferencesdialog.ui \
    passwordInfo.ui \
    dictlist.ui \
    edituserdict.ui \
    userwordslist.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SEfile/release/ -lSEfile
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SEfile/debug/ -lSEfile
else:unix: LIBS += -L$$OUT_PWD/../SEfile/ -lSEfile

INCLUDEPATH += $$PWD/../SEfile
DEPENDPATH += $$PWD/../SEfile

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SEfile/release/libSEfile.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SEfile/debug/libSEfile.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SEfile/release/SEfile.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SEfile/debug/SEfile.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../SEfile/libSEfile.a

#LIBS += -ldl


CONFIG(NO_SECUBE){ # if no se_cube connected, use non encrypted sqlite3
    LIBS += -lsqlite3
#    DEFINES += SQLITE_OS_UNIX
#    LIBS += -ldl

} else {
    DEFINES += SECUBE
    DEFINES += SQLITE_OS_SECURE # used for secuereqlite files
    DEFINES += SQLITE_OS_OTHER

    # Added secureSQLite installation path variables, matching our setup guide

    win32 {
        SECURESQLPATH = "$$OUT_PWD/../securesqlite3"
        SEFILEPATH  = "$$OUT_PWD/../SEfile"
        CONFIG(debug,debug|release){
            SECURESQLPATH = $$SECURESQLPATH/debug
            SEFILEPATH = $$SEFILEPATH/debug
        }
        CONFIG(release,debug|release){
            SECURESQLPATH = $$SECURESQLPATH/release
            SEFILEPATH = $$SEFILEPATH/release
        }
    }
    !win32 {
        SECURESQLPATH = "../securesqlite3"
        SEFILEPATH  = "../SEfile"
    }

    LIBS += -L$$SECURESQLPATH \
        -L$$SEFILEPATH

    LIBS += -lsecuresqlite3 -lSEfile

    INCLUDEPATH += "$$PWD/../securesqlite3" \
        "$$PWD/../SEfile"
    DEPENDPATH += "$$PWD/../securesqlite3"\
        "$$PWD/../SEfile" \
        $$SECURESQLPATH \
        $$SEFILEPATH
}

RESOURCES += \
    ../icons/icons.qrc

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../pwgen/release/ -lpwgen
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../pwgen/debug/ -lpwgen
#else:unix: LIBS += -L$$OUT_PWD/../pwgen/ -lpwgen

#INCLUDEPATH += $$PWD/../pwgen
#DEPENDPATH += $$PWD/../pwgen

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pwgen/release/libpwgen.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pwgen/debug/libpwgen.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pwgen/release/pwgen.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pwgen/debug/pwgen.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../pwgen/libpwgen.a

#DISTFILES +=

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zxcvbn/release/ -lzxcvbn
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zxcvbn/debug/ -lzxcvbn
#else:unix: LIBS += -L$$OUT_PWD/../zxcvbn/ -lzxcvbn

#INCLUDEPATH += $$PWD/../zxcvbn
#DEPENDPATH += $$PWD/../zxcvbn

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../zxcvbn/release/libzxcvbn.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../zxcvbn/debug/libzxcvbn.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../zxcvbn/release/zxcvbn.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../zxcvbn/debug/zxcvbn.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../zxcvbn/libzxcvbn.a
