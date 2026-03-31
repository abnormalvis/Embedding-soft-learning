#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "StudentModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    StudentModel studentModel;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("studentModel", &studentModel);
    engine.load(QUrl(QStringLiteral("qrc:/QSqlitedDemo/qml/Main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}