#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QtQml>
#include <string>

#include "handlers.h"
#include "qjsonmodel.h"
#include "lib/libgoshim.h"

/*
   Hi! I'm Troy McClure and I'll be your guide today. You probably remember me
   from blockbusters like "here be dragons" and "darling, I wrote you a little
   contraption" */

/* This is our glorious global object state. In here we will receive a
   serialized snapshot of the context from the application backend */

static char *json;

/* We are interested in observing changes to the global json variable.
   The watchdog bridges the gap from pure c callbacks to the rest of the c++
   logic. QJsonWatch is a QObject-derived class that can emit signals. */

QJsonWatch *qw;

struct jsonWatchdog {
    jsonWatchdog() { qw = new QJsonWatch; }
    void changed() { emit qw->jsonChanged(QString(json)); }
};

/* we need C wrappers around every C++ object, so that we can invoke their methods
 * in the callbacks passed to CGO. */
extern "C" {
static void *newWatchdog(void) { return (void *)(new jsonWatchdog); }
static void jsonChanged(void *thisPtr) {
    if (thisPtr != NULL) {
        jsonWatchdog *classPtr = static_cast<jsonWatchdog *>(thisPtr);
        classPtr->changed();
    }
}
}

void *wd = newWatchdog();

/* onStatusChanged is the registered C callback that we pass to CGO.
   When called, it pulls a string serialization of the context object, than we
   then pass along to the Qt objects, and from there to the Qml representation.
*/
void onStatusChanged() {
    char *st = RefreshContext();
    json = st;
    /* call the wrapped watchdog method, that emits a Qt signal in turn */
    jsonChanged(wd);
    free(st);
}

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext();

    QJsonModel *model = new QJsonModel;
    std::string json = R"({"appName": "unknown", "provider": "unknown"})";
    model->loadJson(QByteArray::fromStdString(json));

    /* set the backend handler class that holds all the slots responsible for
     * calling back to Go with action requests */
    Backend backend;
    ctx->setContextProperty("backend", &backend);

    /* set the json model, this gets re-loaded every time we receive an update from Go */
    ctx->setContextProperty("jsonModel", model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    /* connect the jsonChanged signal explicitely. The signals in the
     * GUI->Backend direction are done in the Qml layer and delegated to the
     * Backend class. */
    QObject::connect(qw, &QJsonWatch::jsonChanged, [ctx, model](QString js) {
        model->loadJson(js.toUtf8());
    });

    /* register statusChanged callback */
    const char *stCh = "OnStatusChanged";
    GoString statusChangedEvt = {stCh, (long int)strlen(stCh)};
    SubscribeToEvent(statusChangedEvt, (void *)onStatusChanged);

    /* initialize connection context  */
    InitializeContext();
    MockUIInteraction();

    return app.exec();
}
