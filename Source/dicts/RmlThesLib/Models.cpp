// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"
#include "morph_dict/common/utilit.h"

bool CThesaurus::LoadModelRelations(std::string Buff, CInnerModel &M) {
    StringTokenizer Line(Buff.c_str(), ";");

    while (true) {
        const char *q = Line();
        if (q == 0) break;
        std::string Rel = q;

        char s[200];
        CThesSynRelation R;

        if (sscanf(Rel.c_str(), "%[^(](%i,%i)", s, &R.m_Word1, &R.m_Word2) != 3)
            return false;

        std::string Name = s;
        Trim(Name);
        if (Name == "с_опр")
            R.m_Name = Coord;
        else if (Name == "упр")
            R.m_Name = Subord;
        else if (Name == "с_соч")
            R.m_Name = CoordSimilar;
        else if (Name == "деф")
            R.m_Name = Hyphen;
        else if (Name == "subord")
            R.m_Name = eSubord;
        else if (Name == "parat")
            R.m_Name = eParat;
        else
            return false;

        M.m_SynRelations.push_back(R);
    };
    return true;
};


bool CThesaurus::LoadAtomicGroups(std::string Buff, CInnerModel &M) {
    StringTokenizer Line(Buff.c_str(), ";");

    while (true) {
        const char *q = Line();
        if (q == 0) break;
        std::string SynGroup = q;
        Trim(SynGroup);
        CAtomGroup G;
        uint64_t dummy = 0;

        if (SynGroup == "DSC")
            G.m_bDigCmpl = true;
        // one model can contain patterns from different languages 
        else if (m_pEngGramTab->ProcessPOSAndGrammemsIfCan(SynGroup.c_str(), &G.m_PartOfSpeech, &dummy))
            G.m_LanguageId = morphEnglish;
        else if (m_pMainGramTab->ProcessPOSAndGrammemsIfCan(SynGroup.c_str(), &G.m_PartOfSpeech, &dummy))
            G.m_LanguageId = m_MainLanguage;
        else
            return false;

        G.m_Grammems = dummy;

        M.m_AtomGroups.push_back(G);
    };

    return true;
};

bool CThesaurus::LoadGroups(std::string Buff, CInnerModel &M) {
    StringTokenizer Line(Buff.c_str(), ";");
    assert (M.m_SynGroups.empty());
    while (true) {
        const char *q = Line();
        if (q == 0) break;
        std::string SynGroup = q;
        Trim(SynGroup);
        CThesSynGroup G;
        char s[200];
        G.m_First = 1000; // gcc 8.0: sscanf does not work with this initialization
        G.m_Last = 1000;
        if (sscanf(SynGroup.c_str(), "%[^(](%li,%li)", s, &G.m_First, &G.m_Last) != 3)
            return false;

        assert (G.m_First >= 0  && G.m_First < 100);
        assert (G.m_Last >= 0  && G.m_Last < 100);

        G.m_TypeStr = s;

        M.m_SynGroups.push_back(G);
    };

    return true;
};

std::string& strip_quotes(std::string& s) {
    if (!s.empty() && s[0] == '"') {
        s.erase(0, 1);
    }
    if (!s.empty() && s.back() == '"') {
        s.erase(s.length() - 1, 1);
    }
    return s;
}

void CThesaurus::LoadModels(std::string FileName) {
    m_Models.clear();
    std::ifstream ifs(FileName);
    if (!ifs.is_open()) {
        throw CExpc("cannot open %s", FileName.c_str());
    }
    std::string line;
    if (!getline(ifs, line))
        throw CExpc("cannot read header from %s", FileName.c_str());
    std::string Header = line;
    Trim(Header);
    if (Header != "FreqCollocTypeId;Length;AtomGroups;Relations;Examples;Enabled;LoadTextForm;LanguageId;Groups;")
        throw CExpc("bad header in %s", FileName.c_str());

    while (getline(ifs, line)) {
        CInnerModel M;
        //auto innerStr = convert_from_utf8(line.c_str(), m_MainLanguage);
        auto fields = split_string(line, FieldDelimiter[0]);
        M.m_ModelId = atoi(fields[0].c_str());

        if (!LoadAtomicGroups(strip_quotes(fields[2]), M)) {
            throw CExpc("LoadAtomicGroups failed in %s, line %s", FileName.c_str(),
                line.c_str());
        }

        if (!LoadModelRelations(strip_quotes(fields[3]), M)) {
            throw CExpc("LoadModelRelations failed in %s", FileName.c_str());
        }

        if (!LoadGroups(strip_quotes(fields[8]), M)) {
            throw CExpc("LoadGroups failed in %s", FileName.c_str());
        }
        m_Models.push_back(M);
    };

}


bool CInnerModel::IsRussian() const {
    for (size_t i = 0; i < m_AtomGroups.size(); i++)
        if (m_AtomGroups[i].m_LanguageId == morphRussian)
            return true;
    return false;
};
