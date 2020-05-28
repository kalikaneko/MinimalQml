#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QtQml>
#include <string>

#include "handlers.h"
#include "qjsonmodel.h"
#include "lib/libgoshim.h"

/* Hi. I'm Troy McClure and I'll be your guide today to this little
   contraption! Hold with me...
*/

/* This is our glorious global object state. in here we will receive a
   serialized snapshot of the context from the application backend */

static char *json;

/* We are interested in observing changes to the global json variable.
   The watchdog bridges the gap rom pure c callbacks to the rest of the c++
   logic. QJsonWatch is a QObject-derived class that can emit signals. */

QJsonWatch *qw;

struct jsonWatchdog {
    jsonWatchdog() { qw = new QJsonWatch; }
    void changed() { emit qw->jsonChanged(QString(json)); }
};

/* we need C wrappers around every C++ object, so that we can invoke their methods
 * in the callbacks passed to CGO. */
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

/* onStatusChanged is the registered C callback that we pass to CGO.
   When called, it pulls a string serialization of the context object, than we
   then pass along to the Qt objects, and from there to the Qml representation.
*/
void onStatusChanged() {
    char *st = RefreshContext();
    json = st;
    /* call the wrapped watchdog method, that emits a Qt signal in turn */
    jsonChanged(watchdog);
    free(st);
}

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext();

    QJsonModel *model = new QJsonModel;
    std::string json = R"({"appName": "dummy", "provider": "example.org"})";
    model->loadJson(QByteArray::fromStdString(json));
    ctx->setContextProperty("jsonModel", model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    /* connect signals */
    QObject::connect(qw, &QJsonWatch::jsonChanged, [ctx, model](QString js) {
        model->loadJson(js.toUtf8());
    });

    /* register statusChanged callback */
    char *stCh = "OnStatusChanged";
    GoString statusChangedEvt = {stCh, strlen(stCh)};
    SubscribeToEvent(statusChangedEvt, (void *)onStatusChanged);

    /* initialize connection context  */
    InitializeContext();
    MockUIInteraction();

    return app.exec();
}
