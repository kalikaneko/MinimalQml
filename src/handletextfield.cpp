#include "handletextfield.h"
#include "lib/libgoshim.h"

HandleTextField::HandleTextField(QObject *parent) :
    QObject(parent)
{
}

void HandleTextField::handleSubmitTextField(const QString &in)
{
    qDebug() << "c++: HandleTextField::handleSubmitTextField:" << in;
    emit setTextField(in.toUpper());
    GoString name = {"dear user", strlen("dear user")};
    hello(name);
}
