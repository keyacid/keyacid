QT += core gui widgets
TARGET = keyacid
TEMPLATE = app
SOURCES += main.cpp\
           widget.cpp \
           remoteprofile.cpp \
           localprofile.cpp \
           crypto.cpp
HEADERS += widget.h \
           remoteprofile.h \
           localprofile.h \
           crypto.h
FORMS += widget.ui
osx: INCLUDEPATH += /usr/local/include/
LIBS += -lsodium
