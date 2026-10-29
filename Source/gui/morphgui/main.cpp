#include "MorphGuiWindow.h"

#include <QApplication>
#include <QMessageBox>
#include <clocale>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    // QApplication sets the system locale, the morphology must not depend on it
    setlocale(LC_TIME, "C");
    setlocale(LC_NUMERIC, "C");
    QApplication::setOrganizationName("aot");
    QApplication::setApplicationName("MorphGui");

    MorphGuiWindow w;
    std::string error;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool loaded = w.loadMorphology(error);
    QApplication::restoreOverrideCursor();
    if (!loaded) {
        QMessageBox::critical(nullptr, "MorphGui", QString::fromUtf8(error.c_str()));
        return 1;
    }
    w.show();
    return app.exec();
}
