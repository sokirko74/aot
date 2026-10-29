#include "RossDictionary.h"

#include "seman/SemanLib/AllRossesHolder.h"

#include <QDateTime>
#include <QFile>
#include <QSysInfo>

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

static const char GuestUserName[] = "guest";

RossDictionary::RossDictionary(QObject* parent) : QObject(parent), m_ExternalRossHolder(NoneRoss) {
    m_Author = GuestUserName;
}

RossDictionary::~RossDictionary() {
    removeLock();
}

CStructDictHolder* RossDictionary::GetRossHolder() {
    if (m_RossId != NoneRoss) {
        return RossDevCore::instance().rossesHolder()->GetRossHolder(m_RossId);
    }
    return &m_ExternalRossHolder;
}

const CStructDictHolder* RossDictionary::GetRossHolder() const {
    if (m_RossId != NoneRoss) {
        return RossDevCore::instance().rossesHolder()->GetRossHolder(m_RossId);
    }
    return &m_ExternalRossHolder;
}

QString RossDictionary::dictName() const {
    return fromUtf8(GetRossHolder()->GetDictName());
}

std::string RossDictionary::lockFileName() const {
    return (fs::path(m_Folder) / "rossdev.lck").string();
}

void RossDictionary::removeLock() {
    if (m_HasLock) {
        std::error_code ec;
        fs::remove(lockFileName(), ec);
        m_HasLock = false;
    }
}

void RossDictionary::load(const std::string& folder, bool readOnly) {
    m_Folder = folder;
    // a lock file means that the dictionary is edited by somebody else
    m_ReadOnly = readOnly || fs::exists(lockFileName());
    CAllRossesHolder* holder = RossDevCore::instance().rossesHolder();
    holder->m_LastUpdateTime++;
    m_RossId = holder->GetRegisteredRossId(folder);
    if (!GetRossHolder()->LoadStructDict(folder, false, holder->m_LastUpdateTime)) {
        throw CExpc("Cannot load dictionary from %s", folder.c_str());
    }
    buildBasicDomItems();
    setModified(false);
}

void RossDictionary::setAuthor(const std::string& author) {
    m_Author = author;
    if (m_Author == GuestUserName) {
        m_ReadOnly = true;
    }
    if (!m_ReadOnly && !m_HasLock) {
        QFile f(QString::fromStdString(lockFileName()));
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QString("Date = %1\nUserName = %2\nHost = %3\n")
                .arg(QDateTime::currentDateTime().toString(Qt::ISODate), fromUtf8(m_Author), QSysInfo::machineHostName()).toUtf8());
            m_HasLock = true;
        }
    }
}

void RossDictionary::setModified(bool modified) {
    if (modified) {
        // the semantic analysis rebuilds indices of modified dictionaries
        CAllRossesHolder* holder = RossDevCore::instance().rossesHolder();
        GetRossHolder()->SetUpdateTime(++holder->m_LastUpdateTime);
    }
    if (m_Modified != modified) {
        m_Modified = modified;
        emit modifiedChanged(modified);
    }
}

void RossDictionary::save() {
    if (m_ReadOnly) {
        throw CExpc("Cannot write: the dictionary is read only.");
    }
    if (!GetRoss()->Save()) {
        throw CExpc("Cannot save %s", m_Folder.c_str());
    }
    // ross.txt is the source of the binary files (see ConvertStructDict in CMakeLists.txt)
    GetRoss()->ExportToTextFile((fs::path(m_Folder) / "ross.txt").string());
    setModified(false);
}

bool RossDictionary::isThesRoss() const {
    return ::IsThesRoss(m_RossId);
}

int RossDictionary::getThesId() const {
    return GetThesIdByRossId(m_RossId);
}

std::string RossDictionary::GetEntryStrUtf8(uint16_t UnitNo) const {
    std::string entry = GetRoss()->GetEntryStr(UnitNo);
    if (!isThesRoss()) {
        return entry;
    }
    int TerminId = atoi(entry.c_str());
    if (TerminId == 0) {
        return entry;
    }
    const CThesaurus* Thes = RossDevCore::instance().rossesHolder()->GetThes(getThesId());
    if (!Thes) {
        return entry;
    }
    int TerminNo = Thes->GetTerminNoByTextEntryId(TerminId);
    return TerminNo == -1 ? entry : Thes->m_Termins[TerminNo].m_TerminStr;
}

void RossDictionary::buildBasicDomItems() {
    CDictionary* ross = GetRoss();
    ross->UpdateConstDomens();
    m_BasicDomItems.clear();
    for (BYTE dom_no = 0; dom_no < ross->m_Domens.size(); dom_no++) {
        const CDomen& D = ross->m_Domens[dom_no];
        if (D.GetDomainSource() == dsMetaText || D.GetDomainSource() == dsExpres) {
            for (auto& s : D.GetAllStrings()) {
                m_BasicDomItems[s] = dom_no;
            }
        }
    }
    // all fields with actant numbers
    m_Fields.clear();
    for (auto& f : ross->Fields) {
        m_Fields.insert(f.FieldStr);
        if (f.IsApplicToActant) {
            for (size_t k = 1; k < MaxNumAct + 1; k++) {
                m_Fields.insert(Format("%s%zu", f.FieldStr.c_str(), k));
            }
        }
    }
    GetRossHolder()->InitConsts();
}

//----------------------------------------------------------------------------

RossDevCore::RossDevCore() : m_SemBuilder(std::make_unique<CSemStructureBuilder>()) {
}

RossDevCore& RossDevCore::instance() {
    static RossDevCore core;
    return core;
}

void RossDevCore::init() {
    // presemantic dictionaries: morphology, syntax, aspect and bilingual dictionaries
    rossesHolder()->Init();
    // the same as CSemStructureBuilder::InitDicts
    m_SemBuilder->m_RusStr.m_pData->GetSynan()->SetKillHomonymsMode(CoverageKillHomonyms);
}

RossDictionary* RossDevCore::findDictionary(DictTypeEnum type) const {
    for (auto d : m_Dictionaries) {
        if (d->GetRossType() == type) {
            return d;
        }
    }
    return nullptr;
}

RossDictionary* RossDevCore::findDictionaryByFolder(const std::string& folder) const {
    std::error_code ec;
    for (auto d : m_Dictionaries) {
        if (fs::equivalent(d->folder(), folder, ec)) {
            return d;
        }
    }
    return nullptr;
}

void RossDevCore::addDictionary(RossDictionary* dict) {
    m_Dictionaries.push_back(dict);
    emit dictionariesChanged();
}

void RossDevCore::removeDictionary(RossDictionary* dict) {
    m_Dictionaries.erase(std::remove(m_Dictionaries.begin(), m_Dictionaries.end(), dict), m_Dictionaries.end());
    emit dictionariesChanged();
}

bool RossDevCore::isRossEntry(const std::string& word) const {
    for (auto d : m_Dictionaries) {
        if (d->GetRoss()->LocateUnit(word.c_str(), 1) != ErrUnitNo) {
            return true;
        }
    }
    return false;
}
