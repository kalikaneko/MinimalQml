#include "handlers.h"
#include "lib/libgoshim.h"

QJsonProxy::QJsonProxy(QObject *parent) :
    QObject(parent)
{
    QJsonModel *model = new QJsonModel;
}

void QJsonProxy::readJson(QString json)
{
    qDebug() << "json changed! now load..." << json;
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
