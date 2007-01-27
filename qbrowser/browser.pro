DEBUG = 1
HEADERS = browser.hpp
SOURCES = browser.cpp
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp
UI_HEADERS_DIR = tmp

CFLAGS += -g
CPPFLAGS += -g
LDFLAGS += -g
INCLUDEPATH += ../libpl2 ./
unix:LIBS += -L../libpl2 -lpl2
win32:LIBS += ../libpl2/libpl2.lib
QT += opengl

