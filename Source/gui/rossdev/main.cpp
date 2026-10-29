#include "MainWindow.h"
#include "RossDictionary.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStatusBar>
#include <clocale>

// defined in morph_dict/common/utilit.cpp, used by ErrorMessage
extern void (*GlobalErrorMessage)(const std::string&);

static void showErrorMessage(const std::string& s) {
    QMessageBox::critical(QApplication::activeWindow(), "RossDev", QString::fromUtf8(s.c_str()).trimmed());
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    // QApplication sets the system locale, the linguistic processors must not depend on it
    setlocale(LC_TIME, "C");
    setlocale(LC_NUMERIC, "C");
    QApplication::setOrganizationName("aot");
    QApplication::setApplicationName("RossDev");

    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(logDir);
    init_plog_seman(plog::info, (logDir + "/rossdev.log").toUtf8().toStdString());
    GlobalErrorMessage = showErrorMessage;

    // the arguments: dictionary folders (or their config.rcf), "--readonly"
    bool readOnly = false;
    QStringList dicts;
    for (auto& arg : QApplication::arguments().mid(1)) {
        if (arg == "--readonly") {
            readOnly = true;
        }
        else {
            dicts << arg;
        }
    }

    MainWindow w;
    w.showMaximized();
    w.statusBar()->showMessage("Loading morphology and syntax...");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    try {
        RossDevCore::instance().init();
    }
    catch (std::exception& e) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(&w, "RossDev", QString("Cannot load presemantic dictionaries: ") + QString::fromUtf8(e.what()));
        return 1;
    }
    QApplication::restoreOverrideCursor();
    w.statusBar()->showMessage("Ready");
    for (auto& d : dicts) {
        w.openDictionary(d, readOnly);
    }
    return app.exec();
}
