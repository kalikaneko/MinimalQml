#ifndef HANDLERS_H
#define HANDLERS_H

#include <QDebug>
#include <QObject>
#include "qjsonmodel.h"

class QJsonWatch : public QObject {
    Q_OBJECT
    QJsonModel *model;

   signals:
    void jsonChanged(QString json);
};

class QJsonProxy : public QObject {
    Q_OBJECT

   public:
    explicit QJsonProxy(QObject *parent = 0);

   public slots:
    void readJson(QString json);
};

class HandleTextField : public QObject {
    Q_OBJECT

   public:
    explicit HandleTextField(QObject *parent = 0);

   signals:
    void setTextField(QVariant text);

   public slots:
    void handleSubmitTextField(const QString &in);
};

#endif  // HANDLERS_H
