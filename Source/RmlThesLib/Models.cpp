// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"
#include "../common/utilit.h"

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
        if (Name == _R("с_опр"))
            R.m_Name = Coord;
        else if (Name == _R("упр"))
            R.m_Name = Subord;
        else if (Name == _R("с_соч"))
            R.m_Name = CoordSimilar;
        else if (Name == _R("деф"))
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
        QWORD dummy = 0;

        if (SynGroup == "DSC")
            G.m_bDigCmpl = true;
        else if (m_pMainGramTab->ProcessPOSAndGrammemsIfCan(SynGroup.c_str(), &G.m_PartOfSpeech, &dummy))
            G.m_LanguageId = m_MainLanguage;
        else if (m_pEngGramTab->ProcessPOSAndGrammemsIfCan(SynGroup.c_str(), &G.m_PartOfSpeech, &dummy))
            G.m_LanguageId = morphEnglish;
        else
            return false;

        G.m_Grammems = (int) dummy;

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
        if (sscanf(SynGroup.c_str(), "%[^(](%i,%i)", s, &G.m_First, &G.m_Last) != 3)
            return false;

        assert (G.m_First >= 0  && G.m_First < 100);
        assert (G.m_Last >= 0  && G.m_Last < 100);

        G.m_TypeStr = s;

        M.m_SynGroups.push_back(G);
    };

    return true;
};

bool CThesaurus::LoadModels(std::string FileName) {
    m_Models.clear();
    FILE *fp = fopen(FileName.c_str(), "r");
    if (!fp) return false;
    char buff[2000];
    if (!fgets(buff, 2000, fp))
        return false;
    std::string Header = buff;
    Trim(Header);
    if (Header != "FreqCollocTypeId;Length;AtomGroups;Relations;Examples;Enabled;LoadTextForm;LanguageId;Groups;")
        return false;
    while (fgets(buff, 2000, fp)) {
        CInnerModel M;
        std::string innerStr = convert_from_utf8(buff, m_MainLanguage);
        StringTokenizer Line(innerStr.c_str(), FieldDelimiter);
        int i = 0;
        while (true) {
            const char *s = Line();
            if (s == 0) break;
            std::string Field = s;
            if (Field[0] == '"') {
                if (Field[Field.length() - 1] != '"')
                    return false;
                Field.erase(0, 1);
                Field.erase(Field.length() - 1, 1);

            };

            if (i == 0)
                M.m_ModelId = atoi(Field.c_str());
            else if (i == 2) {
                if (!LoadAtomicGroups(Field, M)) return false;
            } else if (i == 3) {
                if (!LoadModelRelations(Field, M)) return false;
            } else if (i == 8) {
                if (!LoadGroups(Field, M)) return false;
            };

            i++;
        };
        m_Models.push_back(M);
    };
    fclose(fp);

    return true;
}


bool CInnerModel::IsRussian() const {
    for (size_t i = 0; i < m_AtomGroups.size(); i++)
        if (m_AtomGroups[i].m_LanguageId == morphRussian)
            return true;
    return false;
};
