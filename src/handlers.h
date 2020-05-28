#ifndef HANDLERS_H
#define HANDLERS_H

#include <QDebug>
#include <QObject>
#include <QtQml>
#include "qjsonmodel.h"

class QJsonWatch : public QObject {
    Q_OBJECT
    QJsonModel *model;

    public:

    signals:

        void jsonChanged(QString json);

};

// FIXME -- I think we can do w/o this proxy, since i'm using a lambda in the end
//
class QJsonProxy : public QObject {
    Q_OBJECT
    QQmlApplicationEngine *eng;

   public:

    explicit QJsonProxy(QObject *parent = 0, QQmlApplicationEngine *eng = 0);

   signals:
    void updateModel(QString s);

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
