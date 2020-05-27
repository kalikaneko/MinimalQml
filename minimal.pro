CONFIG += qt staticlib
# TODO remove this in cross build?
unix:CONFIG += debug
lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

QT += qml quick

SOURCES += src/main.cpp \
           src/handlers.cpp \
           src/qjsonmodel.cpp

HEADERS += src/handlers.h \
           src/qjsonmodel.h \
           lib/libgoshim.h

RESOURCES += src/qml.qrc

TARGET=MinimalQml
LIBS += -L./lib -lgoshim -lpthread

