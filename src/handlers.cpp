#include <string>
#include <iostream>

#include "handlers.h"
#include "lib/libgoshim.h"

QJsonProxy::QJsonProxy(QObject *parent, QJsonModel *model) :
    QObject(parent)
{
}

void QJsonProxy::readJson(QString json)
{
    qDebug() << "received json" << json;
    std::string utf8_text = json.toUtf8().constData();
    printf("hello--->", utf8_text);

    //std::string js = json.toStdString();
    //qDebug() << "jsonStr" << utf8_text;
    std::string js = R"({"appName":"RiseupVPN","provider":"black.riseup.net","status":"off"})";
    //model->loadJson(QByteArray::fromStdString(json.toStdString()));
    //model->loadJson(QByteArray::fromStdString(js));
    //model->loadJson(QByteArray::fromStdString(utf8_text));

    //FIXME :: this is segfaulting on load()
    //better use https://github.com/benlau/qsyncable
    model->loadJson(QByteArray::fromStdString(js));
    
    // XXX does this do anything??? debug
    //model->load(json);
    qDebug() << "loaded...";
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
