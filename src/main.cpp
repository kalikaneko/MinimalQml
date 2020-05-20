#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>

#include "handlers.h"


int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    HandleTextField handleTextField;

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    /* submitTextField signal is defined in qml.
     * emitted by Button.onClicked */
    QObject::connect(window, SIGNAL(submitTextField(QString)),
                     &handleTextField, SLOT(handleSubmitTextField(QString)));

    /* setTextField is a signal emitted by handletextfield handler
     * setTextFieldUI is the slot, defined in the qml */
    QObject::connect(&handleTextField, SIGNAL(setTextField(QVariant)),
                     window, SLOT(setTextFieldUI(QVariant)));

    return app.exec();
}
