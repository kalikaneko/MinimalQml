#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QtQml>
#include <string>

#include "handlers.h"
#include "lib/libgoshim.h"
#include "qjsonmodel.h"

/* Hi. I'm Troy McClure and I'll be your guide today! */

/* This is our glorious global object state. in here we will receive a
   serialized snapshot of the context from the application backend */

static char *json;

/* We are interesting in observing changes to the global json variable.
   The watchdog bridges the gap rom pure c callbacks to the rest of the c++
   logic. QJsonWatch is a QObject-derived class that can emit signals. */

QJsonWatch* qw;

struct jsonWatchdog {
    jsonWatchdog() {
        qw = new QJsonWatch;
    }
    void changed() {
        emit qw->jsonChanged(QString(json));
    }
};


extern "C" {
static void *getWatchdog(void) { return (void *)(new jsonWatchdog); }
static void jsonChanged(void *thisPtr) {
    if (thisPtr != NULL) {
        jsonWatchdog *classPtr = static_cast<jsonWatchdog *>(thisPtr);
        classPtr->changed();
    }
}
}

void *watchdog = getWatchdog();

void onStatusChanged() {
    char *st = RefreshContext();
    json = st;
    printf("%s\n", json);
    jsonChanged(watchdog);
    free(st);
}

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);

    QJsonProxy *jp = new QJsonProxy;
    QObject::connect(qw, SIGNAL(jsonChanged(QString)),
                     jp, SLOT(readJson(QString)));


    QJsonModel *model = new QJsonModel;
    std::string json = R"({})";
    model->loadJson(QByteArray::fromStdString(json));

    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext();
    ctx->setContextProperty("jsonModel", model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    HandleTextField handleTextField;

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    /* submitTextField signal is defined in qml.
     * emitted by Button.onClicked */
    QObject::connect(window, SIGNAL(submitTextField(QString)), &handleTextField,
                     SLOT(handleSubmitTextField(QString)));

    /* setTextField is a signal emitted by handletextfield handler
     * setTextFieldUI is the slot, defined in the qml */
    QObject::connect(&handleTextField, SIGNAL(setTextField(QVariant)), window,
                     SLOT(setTextFieldUI(QVariant)));

    /* register statusChanged callback */
    char *stCh = "OnStatusChanged";
    GoString statusChangedEvt = {stCh, strlen(stCh)};
    SubscribeToEvent(statusChangedEvt, (void *)onStatusChanged);

    /* trigger a dummy status change */
    // QTimer::singleShot(2000, [] { TriggerStatusChange(); });

    /* initialize connection context  */
    InitializeContext();
    MockUIInteraction();

    return app.exec();
}
