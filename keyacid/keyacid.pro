QT += core gui widgets
TARGET = keyacid
TEMPLATE = app
SOURCES += main.cpp\
           widget.cpp \
           remoteprofile.cpp \
           localprofile.cpp \
           crypto.cpp \
           disclaimerdialog.cpp
HEADERS += widget.h \
           remoteprofile.h \
           localprofile.h \
           crypto.h \
           disclaimerdialog.h
FORMS += widget.ui \
         disclaimerdialog.ui
osx {
    INCLUDEPATH += /usr/local/include/
    ICON = icons/macos.icns
}
win32: RC_ICONS = icons/win.ico
LIBS += -lsodium
