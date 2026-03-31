#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
#ifdef HAS_QUICK3D
    engine.load(QUrl(QStringLiteral("qrc:/Quick3DLesson/qml/MainQuick3D.qml")));
#else
    engine.load(QUrl(QStringLiteral("qrc:/Quick3DLesson/qml/MainFallback.qml")));
#endif

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
