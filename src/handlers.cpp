#include "handlers.h"
#include "lib/libgoshim.h"

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
