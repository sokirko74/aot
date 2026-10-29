#pragma once

#include "seman/SemanLib/SemStructureBuilder.h"
#include "seman/SemanLib/struct_dict_holder.h"

#include <QObject>
#include <QString>

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// one open structural dictionary (CRossDoc in the MFC version).
// A registered dictionary (Ross, TimeRoss, Obor...) lives in the semantics holder, so
// the semantic analysis sees all changes of the editor at once.
class RossDictionary : public QObject {
    Q_OBJECT
public:
    explicit RossDictionary(QObject* parent = nullptr);
    ~RossDictionary() override;

    // loads the dictionary from the folder with config.rcf
    void load(const std::string& folder, bool readOnly);
    // writes binary files and ross.txt (the source of the binary files in git)
    void save();

    CDictionary* GetRoss() { return GetRossHolder()->GetRoss(); }
    const CDictionary* GetRoss() const { return GetRossHolder()->GetRoss(); }
    CStructDictHolder* GetRossHolder();
    const CStructDictHolder* GetRossHolder() const;
    DictTypeEnum GetRossType() const { return m_RossId; }
    QString dictName() const;
    std::string folder() const { return m_Folder; }

    bool isThesRoss() const;
    int getThesId() const;
    // the entry string, thesaurus dictionaries contain termin ids instead of entries
    std::string GetEntryStrUtf8(uint16_t UnitNo) const;

    bool isReadOnly() const { return m_ReadOnly; }
    // changes the user (guest makes the dictionary read only)
    void setAuthor(const std::string& author);
    std::string author() const { return m_Author; }

    bool isModified() const { return m_Modified; }
    // marks the dictionary as modified, the semantic analysis rebuilds its indices
    void setModified(bool modified = true);

    const char* GetGramFetFieldName() const { return "GF"; }
    const char* GetReferenceFieldName() const { return "REF"; }
    const char* GetEnglishFieldName() const { return "ENG"; }

    // items of metalanguage domains -> domain no (for the highlighter)
    std::unordered_map<std::string, BYTE> m_BasicDomItems;
    // all field names with actant numbers (for the highlighter)
    std::set<std::string> m_Fields;

signals:
    void modifiedChanged(bool modified);
    // the list of entries has been changed
    void entriesChanged();

private:
    CStructDictHolder m_ExternalRossHolder;
    DictTypeEnum m_RossId = NoneRoss;
    std::string m_Folder;
    std::string m_Author;
    bool m_ReadOnly = true;
    bool m_Modified = false;
    bool m_HasLock = false;

    std::string lockFileName() const;
    void removeLock();
    void buildBasicDomItems();
};

// the data shared by all windows of the application
class RossDevCore : public QObject {
    Q_OBJECT
public:
    static RossDevCore& instance();

    // loads morphology, syntax and other presemantic dictionaries
    void init();
    CSemStructureBuilder& semBuilder() { return *m_SemBuilder; }
    CAllRossesHolder* rossesHolder() { return m_SemBuilder->m_RusStr.m_pData; }

    const std::vector<RossDictionary*>& dictionaries() const { return m_Dictionaries; }
    RossDictionary* findDictionary(DictTypeEnum type) const;
    RossDictionary* findDictionaryByFolder(const std::string& folder) const;
    void addDictionary(RossDictionary* dict);
    void removeDictionary(RossDictionary* dict);
    // is the word an entry of some open dictionary
    bool isRossEntry(const std::string& word) const;

signals:
    void dictionariesChanged();

private:
    RossDevCore();
    std::unique_ptr<CSemStructureBuilder> m_SemBuilder;
    std::vector<RossDictionary*> m_Dictionaries;
};

inline QString fromUtf8(const std::string& s) {
    return QString::fromUtf8(s.c_str());
}

inline std::string toUtf8(const QString& s) {
    return s.toUtf8().toStdString();
}
