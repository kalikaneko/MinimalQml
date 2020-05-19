CONFIG += qt debug staticlib
QT += qml quick

SOURCES += src/main.cpp \
           src/handletextfield.cpp \

HEADERS += src/handletextfield.h \
           lib/goshim.h

RESOURCES += src/qml.qrc

TARGET=MinimalQml

LIBS += -L./lib -lgoshim -lpthread

win32:INCLUDEPATH += "C:/mylibs/extra_headers"
unix:INCLUDEPATH += "/home/user/extra_headers"

