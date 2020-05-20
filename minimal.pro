CONFIG += qt staticlib
unix:CONFIG += debug

QT += qml quick

SOURCES += src/main.cpp \
           src/handletextfield.cpp \

HEADERS += src/handletextfield.h \
           lib/libgoshim.h

RESOURCES += src/qml.qrc

TARGET=MinimalQml
LIBS += -L./lib -lgoshim -lpthread

