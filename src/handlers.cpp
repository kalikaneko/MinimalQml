#include <QDebug>

#include "handlers.h"
#include "lib/libgoshim.h"

Backend::Backend(QObject *parent) : QObject(parent)
{
}

void Backend::switchOn()
{
    qDebug() << "switch on plz";
}

void Backend::switchOff()
{
    qDebug() << "switch off plz";
}

void Backend::unblock()
{
    qDebug() << "unblock plz";
}
