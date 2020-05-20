#ifndef HANDLERS_H
#define HANDLERS_H

#include <QObject>
#include <QDebug>

class HandleTextField :  public QObject
{
        Q_OBJECT
public:
        explicit HandleTextField(QObject *parent = 0);

signals:
        void setTextField(QVariant text);

public slots:
        void handleSubmitTextField(const QString& in);
};

#endif // HANDLERS_H
