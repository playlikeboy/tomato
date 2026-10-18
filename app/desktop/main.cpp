#include "app/desktop/main_window.h"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <iostream>
#include <string>
#ifndef TOMATO_VERSION
#define TOMATO_VERSION "0.1.0"
#endif

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--version") {
        std::cout << "Tomato " << TOMATO_VERSION << '\n';
        return 0;
    }
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Tomato");
    QCoreApplication::setOrganizationName("Tomato");
    QCoreApplication::setApplicationVersion(TOMATO_VERSION);

    tomato::engine::LibVLCAdapter engine;
    if (!engine.initialize()) {
        QMessageBox::critical(nullptr, "Tomato",
            "Tomato could not initialize LibVLC. Check that the LibVLC runtime and plugins are installed.");
        return 1;
    }

    tomato::app::MainWindow window(&engine);
    QStringList paths;
    for (int i = 1; i < argc; ++i) paths.push_back(QString::fromLocal8Bit(argv[i]));
    window.show();
    if (!paths.isEmpty()) QTimer::singleShot(0, &window, [&window, paths] { window.openPaths(paths); });
    return app.exec();
}
