#pragma once

#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

#include <QWidget>
#include <memory>

class QButtonGroup;
class QLineEdit;
class QTableWidget;

// the cross-platform version of windows/Demo: finds paradigms of a word in all dictionaries
class MorphGuiWindow : public QWidget {
    Q_OBJECT
public:
    explicit MorphGuiWindow(QWidget* parent = nullptr);

    // loads dictionaries of all languages, returns false and fills error if it cannot
    bool loadMorphology(std::string& error);

public slots:
    void onFind();
    void onSubjectChanged(int subject);

private:
    CMorphanHolder m_Russian;
    CMorphanHolder m_English;
    CMorphanHolder m_German;

    QLineEdit* m_Source = nullptr;
    QTableWidget* m_Results = nullptr;
    QButtonGroup* m_Languages = nullptr;
    QButtonGroup* m_Subjects = nullptr;

    CMorphanHolder& getHolder(MorphLanguageEnum langua);
    bool buildParadigmsForOneLanguage(MorphLanguageEnum langua, const QString& src, bool onlyDigits);
};
