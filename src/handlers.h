#ifndef HANDLERS_H
#define HANDLERS_H

#include <QDebug>
#include <QObject>
#include "qjsonmodel.h"

class QJsonWatch : public QObject {

    Q_OBJECT
    QJsonModel *model;

    public:

    signals:

        void jsonChanged(QString json);

};

#endif  // HANDLERS_H
