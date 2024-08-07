#include "MAPostMain.h"
#include "morph_dict/agramtab/agramtab.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

CPostMorphInteface *NewRussianPostMorph() {
    CMAPost *M = new CMAPost;
    if (!M) return 0;
    try {
        auto& h = GetMHolder(morphRussian);
        M->Init(h.m_pLemmatizer, h.m_pGramTab);
        return M;
    }
    catch (...) {
        delete M;
        throw;
    }
}

CMAPost::CMAPost() {
    m_Language = morphRussian;
    m_bCanChangeInputText = true;
    m_bHumanFriendlyOutput = false;
}

CMAPost::~CMAPost() {
    m_Words.clear();
    m_pRusLemmatizer = 0;

}

CFixedColloc::CFixedColloc(std::string LemmaStr, long MainWordNo, std::string InterfaceString,
                           const CAgramtab *piRusGramTab) {
    m_LemmaStr = LemmaStr;
    m_MainWordNo = MainWordNo;
    m_InterfaceString = InterfaceString;
    StringTokenizer tok(LemmaStr.c_str(), " ");
    while (tok()) {
        CLemmaAndPOS L;
        std::string Item = tok.val();
        int i = Item.find("[");
        if (i != std::string::npos) {
            int k = Item.find("]");
            std::string POS = Item.substr(i + 1, k - i - 1);
            uint64_t dummy;
            piRusGramTab->ProcessPOSAndGrammems(POS.c_str(), L.m_POS, dummy);
            L.m_Lemma = Item.substr(0, i);
        } else {
            L.m_Lemma = Item;
            L.m_POS = 255;
        };
        m_Lemmas.push_back(L);
    };
    assert (m_MainWordNo < m_Lemmas.size());
    assert (m_MainWordNo >= 0);
};

void CMAPost::ReadCollocs() {
    std::string FileName = GetRegistryString("Software\\Dialing\\Mapost\\Collocs");
    FILE *fp = fopen(FileName.c_str(), "rb");
    if (!fp) {
        throw ("Cannot find file %s in CMAPost::ReadCollocs()", FileName.c_str());
    };
    char s[1000];
    while (fgets(s, 1000, fp)) {
        std::string LemmaStr;;
        int MainWordNo;
        std::string InterfaceString;
        StringTokenizer tok(s, "#");
        int i = 0;
        while (tok()) {
            std::string q = tok.val();
            Trim(q);
            if (i == 0)
                LemmaStr = q;
            else if (i == 1)
                MainWordNo = atoi(q.c_str());
            else if (i == 2)
                InterfaceString = q;
            i++;
        };

        m_FixedCollocs.push_back(CFixedColloc(LemmaStr, MainWordNo, InterfaceString, m_pRusGramTab));
    };
    fclose(fp);
};


void CMAPost::Init(const CLemmatizer *RusLemmatizer, const CAgramtab *RusGramTab) {
    m_pRusGramTab = reinterpret_cast<const CRusGramTab*>(RusGramTab);
    assert(m_pRusGramTab);

    m_pRusLemmatizer = RusLemmatizer;
    ReadCollocs();

    m_DURNOVOGramCode = m_pRusGramTab->GetPlugNouInfo().m_GramCode;
    auto codes = m_pRusGramTab->GetAllGramCodes(NOUN, _QM(rIndeclinable) | _QM(rInitialism), GrammemsInclusion);
    for (size_t i = 0; i < codes.length(); i += 2) {
        m_AbbrIndeclGramCodes.insert(codes.substr(i, 2));
    }
    assert (m_AbbrIndeclGramCodes.size() == 6);
}

