#include <QtQml>
#include <string>
#include <iostream>

#include "handlers.h"
#include "lib/libgoshim.h"

QJsonProxy::QJsonProxy(QObject *parent, QQmlApplicationEngine *eng) :
    QObject(parent)
{
}

void QJsonProxy::readJson(QString js)
{
    qDebug() << "received json (now parse)" << js;
    emit updateModel(js);
}


HandleTextField::HandleTextField(QObject *parent) :
    QObject(parent)
{
}

void HandleTextField::handleSubmitTextField(const QString &in)
{
    qDebug() << "c++: HandleTextField::handleSubmitTextField:" << in;
    /* we emit a signal here */
    emit setTextField(in.toUpper());
    SayHello();
}
