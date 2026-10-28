#pragma once

#include "morph_dict/morph_wizard/wizard.h"

#include <QWidget>
#include <QAbstractTableModel>
#include <QPointer>
#include <array>
#include <memory>
#include <vector>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QMdiArea;
class QPlainTextEdit;
class QSpinBox;
class QTableView;
class QTableWidget;
class SlfEditor;

// the list of found paradigms; the strings are computed once, since lemma iterators
// can become invalid after the dictionary is changed
class FoundParadigmsModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Columns { ParadigmNoColumn = 0, LemmaColumn, GrammemsColumn, TypeGrammemsColumn, ColumnsCount };

    using QAbstractTableModel::QAbstractTableModel;
    void setRows(const MorphoWizard& wizard, const std::vector<lemma_iterator_t>& paradigms);
    void clear();
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    std::vector<std::array<QString, ColumnsCount>> m_Rows;
};


// the main window of a dictionary (CMorphwizardView + CMorphwizardDoc in the MFC version)
class WizardView : public QWidget {
    Q_OBJECT
public:
    // throws an exception if the dictionary cannot be loaded
    WizardView(QMdiArea* mdiArea, const QString& mwzPath, const QString& userName, bool usePredict, QWidget* parent = nullptr);
    ~WizardView() override;

    MorphoWizard* GetWizard() { return m_Wizard.get(); }
    QString filePath() const { return m_FilePath; }
    void echo(const QString& s);
    void updateTitle();

    bool isModified() const;
    bool save();
    // asks the user and saves the dictionary and edited paradigms, returns false if the user canceled
    bool saveModified();

    void registerEditor(SlfEditor* editor);
    void unregisterEditor(SlfEditor* editor);

public slots:
    void onFind();
    void onNextNonAccentedPara();

    // tools
    void onToolsSaveListFile();
    void onToolsExport();
    void onToolsImport();
    void onToolsSelectByFile();
    void onToolsPack();
    void onToolsSetParaNo();
    void onToolsSetAccentModelNo();
    void onToolsShowParadigmDifferences();
    void onToolsShowAccentDifferences();
    void onToolsAccentWizard();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onRemove();
    void onEdit();
    void onPredict();
    void onAdd();
    void onSetParadigmComments();
    void onFoundHeaderClicked(int column);
    void onPredictedHeaderClicked(int column);

private:
    std::unique_ptr<MorphoWizard> m_Wizard;
    QString m_FilePath;
    QMdiArea* m_MdiArea;
    std::vector<QPointer<SlfEditor>> m_Editors;
    // diff windows
    std::vector<QPointer<QWidget>> m_ChildWindows;

    std::vector<lemma_iterator_t> m_FoundParadigms;
    std::vector<CPredictSuffix> m_PredictedParadigms;
    std::string m_LemmaToPredict;
    int m_SortPredictColumn = -1;
    bool m_PartialAccentedPara = false;
    QStringList m_LastQueries;

    QComboBox* m_FindWhat;
    QButtonGroup* m_SearchType;
    QTableView* m_FoundList;
    FoundParadigmsModel* m_FoundModel;
    QLineEdit* m_Filter;
    QLineEdit* m_PredictWhat;
    QSpinBox* m_MinimalFrequence;
    QSpinBox* m_PredictSuffLength;
    QTableWidget* m_PredictedList;
    QCheckBox* m_OnlyMainPartOfSpeeches;
    QPlainTextEdit* m_Status;

    void createControls(bool readOnly);
    void loadHistory();
    void saveHistory() const;
    void changeHistory(const QString& query);
    void filterFoundParadigms();
    void showFoundParadigms();
    int selectedFoundIndex() const;
    SlfEditor* newSlfEditor();
    bool openExistingParadigm(lemma_iterator_t it, bool runAccentWizard = false);
    lemma_iterator_t findNonAccentedPara(bool partialAccentedPara);
    void openWindow(QWidget* w, QSize size);
};
