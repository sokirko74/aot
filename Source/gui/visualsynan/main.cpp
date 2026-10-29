#include "MainWindow.h"
#include "VisualSentences.h"

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QMessageBox>
#include <QStandardPaths>
#include <clocale>

// defined in morph_dict/common/utilit.cpp, used by ErrorMessage
extern void (*GlobalErrorMessage)(const std::string&);

static void showErrorMessage(const std::string& s) {
    QMessageBox::critical(QApplication::activeWindow(), "VisualSynan", QString::fromUtf8(s.c_str()).trimmed());
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    // QApplication sets the system locale, the linguistic processors must not depend on it
    setlocale(LC_TIME, "C");
    setlocale(LC_NUMERIC, "C");
    QApplication::setOrganizationName("aot");
    QApplication::setApplicationName("VisualSynan");

    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(logDir);
    init_plog(plog::info, (logDir + "/visualsynan.log").toUtf8().toStdString(), false);
    GlobalErrorMessage = showErrorMessage;

    QFont wordsFont("Times New Roman");
    wordsFont.setStyleHint(QFont::Serif);
    wordsFont.setPixelSize(24);
    g_VisualFonts.setWordsFont(wordsFont);

    // the arguments: a language (Russian, German) and text files
    MorphLanguageEnum language = morphRussian;
    QStringList files;
    for (auto& arg : QApplication::arguments().mid(1)) {
        MorphLanguageEnum l;
        if (GetLanguageByString(arg.toUpper().toStdString(), l)) {
            language = l;
        }
        else {
            files << arg;
        }
    }

    MainWindow w(language);
    w.showMaximized();
    if (!w.getHolder(language)) {
        return 1;
    }
    if (files.isEmpty()) {
        w.onNew();
    }
    for (auto& f : files) {
        w.openFile(f);
    }
    return app.exec();
}
