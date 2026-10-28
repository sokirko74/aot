#include "MainWindow.h"
#include "morph_dict/common/utilit.h"

#include <QApplication>
#include <clocale>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

// defined in morph_dict/common/utilit.cpp, used by ErrorMessage
extern void (*GlobalErrorMessage)(const std::string&);

static void showErrorMessage(const std::string& s) {
    QMessageBox::critical(QApplication::activeWindow(), "MorphWizard", QString::fromUtf8(s.c_str()).trimmed());
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    // QApplication sets the system locale, but dates in sessions (strftime) and numbers
    // in dictionary files must not depend on it, as in the console tools
    setlocale(LC_TIME, "C");
    setlocale(LC_NUMERIC, "C");
    QApplication::setOrganizationName("aot");
    QApplication::setApplicationName("MorphWizard");

    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(logDir);
    init_plog(plog::verbose, (logDir + "/wizard.log").toUtf8().toStdString(), false);
    GlobalErrorMessage = showErrorMessage;

    bool usePredict = true;
    QStringList files;
    for (auto& arg : QApplication::arguments().mid(1)) {
        if (arg == "disable-predict" || arg == "--disable-predict") {
            usePredict = false;
        }
        else {
            files << arg;
        }
    }

    MainWindow w(usePredict);
    w.showMaximized();
    for (auto& f : files) {
        w.openFile(f);
    }
    return app.exec();
}
