CONFIG += qt staticlib
# TODO remove this in cross build?
unix:CONFIG += debug

QT += qml quick

SOURCES += src/main.cpp \
           src/handlers.cpp \

HEADERS += src/handlers.h \
           lib/libgoshim.h

RESOURCES += src/qml.qrc

TARGET=MinimalQml
LIBS += -L./lib -lgoshim -lpthread

