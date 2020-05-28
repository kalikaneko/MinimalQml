#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQml>
#include <QTimer>

#include <string>

#include "lib/libgoshim.h"
#include "handlers.h"
#include "connection.h"
#include "qjsonmodel.h"


char* json ;

void onStatusChanged()
{
    qDebug() << "pulling context...";
    char* st = RefreshContext();
    json = st;
    printf("%s\n", json);
    free(st);
}

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    QJsonModel* model = new QJsonModel;
    std::string json = R"({})";
    model->loadJson(QByteArray::fromStdString(json));

    QQmlApplicationEngine engine;
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("jsonModel", model);
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

    /* register statusChanged callback */
    char* stCh = "OnStatusChanged";
    GoString statusChangedEvt = {stCh, strlen(stCh)};
    SubscribeToEvent(statusChangedEvt, (void*)onStatusChanged);

    /* trigger a dummy status change */
    // QTimer::singleShot(2000, [] { TriggerStatusChange(); });

    /* initialize connection context  */
    InitializeContext();
    MockUIInteraction();

    return app.exec();
}
