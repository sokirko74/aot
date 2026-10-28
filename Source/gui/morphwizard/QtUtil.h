#pragma once

#include <QString>
#include <QFile>
#include <cstdio>
#include <string>

// MorphoWizard works with utf-8 strings
inline QString fromUtf8(const std::string& s) {
    return QString::fromUtf8(s.data(), (qsizetype)s.size());
}

inline std::string toUtf8(const QString& s) {
    return s.toUtf8().toStdString();
}

// fopen that works with non-ascii file names on all platforms
inline FILE* openFile(const QString& path, const char* mode) {
#ifdef _WIN32
    return _wfopen(path.toStdWString().c_str(), QString::fromLatin1(mode).toStdWString().c_str());
#else
    return fopen(QFile::encodeName(path).constData(), mode);
#endif
}
