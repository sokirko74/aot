#pragma once

#include <QDialog>
#include <QWidget>

#include <cstdint>
#include <vector>

class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTableWidget;
class RossDictionary;

// an entry of some dictionary in a pocket
struct PocketItem {
    RossDictionary* m_Dict;
    uint16_t m_UnitNo;
};

// a subset of entries from different dictionaries (CPocketForm in the MFC version)
class PocketWindow : public QWidget {
    Q_OBJECT
public:
    PocketWindow(const std::vector<PocketItem>& items, const QString& title, QWidget* parent = nullptr);
    bool contains(const RossDictionary* dict) const;

private slots:
    void onEdit();
    void onSave();
    void onSelectInMainList();

private:
    std::vector<PocketItem> m_Items;
    QString m_Title;
    QTableWidget* m_Table;
};

// searches entries by a substring (CFindByStrDlg in the MFC version)
class FindByStrDialog : public QDialog {
    Q_OBJECT
public:
    FindByStrDialog(RossDictionary* activeDict, QWidget* parent);
    void accept() override;

private:
    RossDictionary* m_ActiveDict;
    QLineEdit* m_String;
    QRadioButton* m_AllDicts;
};

// searches references to absent entries of other dictionaries (CFindWrongRefs in the MFC version)
class FindWrongRefsDialog : public QDialog {
    Q_OBJECT
public:
    FindWrongRefsDialog(RossDictionary* activeDict, QWidget* parent);
    void accept() override;

private:
    RossDictionary* m_ActiveDict;
    QLineEdit* m_Field;
};

// asks the user name for a dictionary, returns "guest" for read only access
std::string askUserName(QWidget* parent, const RossDictionary* dict);
