#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(860, 520);
    window.show();

    return app.exec();
}
