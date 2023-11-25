// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include  "StdSynan.h"
#include  "RusSyntaxOpt.h"
#include  "RusSentence.h"
#include  "RusThesaurus.h"
#include  "RusOborot.h"
#include  "dicts/StructDictLib/TempArticle.h"


const int rSyntaxGroupTypesCount = 48;
const std::string rSyntaxGroupTypes[rSyntaxGroupTypesCount] =
        {
                "ПРИЛ_СУЩ", "КОЛИЧ",
                "СЛОЖ_ЧИСЛ", "НАР_ЧИСЛ_СУЩ", "СУЩ_ЧИСЛ",
                "ПГ", "ОДНОР_ПРИЛ", "НАРЕЧ_ГЛАГОЛ",
                "НАР_ПРИЛ", "НАР_НАР", "СРАВН_СТЕПЕНЬ",
                "ОТР_ФОРМА", "ПРЯМ_ДОП", "ГЕНИТ_ИГ",
                "ОДНОР_НАР", "ПЕР_ГЛАГ_ИНФ", "ОДНОР_ИНФ",
                "ФИО", "ОДНОР_ИГ", "ЧИСЛ_СУЩ",
                "ИГ", "ЭЛЕКТ_ИГ", "ЭЛ_АДРЕС",
                "ОДНОР_ЧИСЛ", "МОДИФ_ПРИЛ", "АППРОКС_ИГ",
                "НАР_НАР_ЧИСЛ", "АППРОКС_ПГ", "ПРИЧ_СУЩ",
                "ПОДЛ", "ОБОРОТ", "ПОДКЛАУЗА",
                "ПРИЛОЖЕНИЕ", "СУЩ_ОБС_ПРИЛ", "КЛВ",
                "РАЗРЫВ_СОЮЗ", "ОТСОЮЗ", "НАР_ПРЕДИК",
                "ПРИЛ_ПОСТПОС", "ПРИДАТ_ОПР", "АНАТ_СРАВН",
                "ВВОДН_КЛАУЗА", "ДОЛЖ_ФИО", "ИНОСТР_ЛЕКС",
                "ОТСРАВН", "ПРЯМ_ДОП_РЕВ", "ИНСТР_ДОП"

        };


CRusSyntaxOpt::CRusSyntaxOpt(MorphLanguageEnum langua) : CSyntaxOpt(langua) {
    m_piGramTab = new CRusGramTab();
    m_lPradigmID_NECHEGO = -1;
    m_SimilarNPGroupType = SIMILAR_NOUN_GROUPS;
    m_PluralMask = _QM(rPlural);
    m_SingularMask = _QM(rSingular);
    m_IndeclinableMask = _QM(rIndeclinable);
    m_GenNounGroupType = GEN_NOUN;
    m_PrepNounGroupType = PREP_NOUN;
    m_DirObjGroupType = DIR_OBJ;
    m_NPGroupType = NOUN_GR;
    m_NounAdjGroupType = NOUN_ADJUNCT;
    m_NameGroupType = NAMES;
    m_DisruptConjRelation = DISRUPT_CONJ_RELATION;
    m_DisruptConjGroupType = DISRUPT_CONJ;
    m_OborotGroupType = OBOROTS;
    m_WWWGroupType = WEB_ADDR;
    m_KEYBGroupType = KEYB;
    m_SubjRelation = SUBJ;
    m_RusParenthesis = INP;
    m_Preposition = PREP;
    m_Conjunction = CONJ;
    m_InfintiveClauseType = INFINITIVE_T;

    m_SyntaxGroupTypes.clear();
    for (size_t i = 0; i < rSyntaxGroupTypesCount; i++)
        m_SyntaxGroupTypes.push_back(rSyntaxGroupTypes[i]);

}

CSentence *CRusSyntaxOpt::NewSentence() const {
    return new CRusSentence(this);
};

CLemmatizer *CRusSyntaxOpt::NewLemmatizer() const {
    return new CLemmatizerRussian();
};

COborDic *CRusSyntaxOpt::NewOborDic(const CSyntaxOpt *opt) {
    return new CRusOborDic(opt);
};

CThesaurusForSyntax *CRusSyntaxOpt::NewThesaurus(const CSyntaxOpt *opt) {
    return new CRusThesaurusForSyntax(opt);
};

void CRusSyntaxOpt::DestroyOptions() {
    CSyntaxOpt::DestroyOptions();
};


static void BuildArticleFromUtf8String(CDictionary *piRossDict, std::string s, CTempArticle &A1) {
    A1.ReadFromUtf8String(s.c_str());
};


void CRusSyntaxOpt::LoadFromRoss(CDictionary *piRossDict) {
    int i;

    try {
        int iSize = piRossDict->GetUnitsSize();

        m_AdvAdj.set_poses(_QM(ADV));
        m_SynDependOnAdv.set_poses(_QM(ADV));
        m_SynDependOnAdj.set_poses(_QM(ADV));

        CTempArticle A1(piRossDict);
        A1.ReadFromUtf8String("GF = * НАР:нар_опр");

        CTempArticle A2(piRossDict);
        A2.ReadFromUtf8String("GF1 = * X!:НАР");

        CTempArticle A3(piRossDict);
        A3.ReadFromUtf8String("GF1 = * X!:ПРИЛ");


        CTempArticle A(piRossDict);

        for (i = 0; i < iSize; i++) {
            A.ReadFromDictionary(i, false, true);
            const std::string& word = piRossDict->m_Units[i].GetEntryStr();
            if (A1.IsPartOf(&A, true)) {
                m_AdvAdj.add_lemma(word);
            }

            if (A2.IsPartOf(&A, true)) {
                m_SynDependOnAdv.add_lemma(word);
            }

            if (A3.IsPartOf(&A, true)) {
                m_SynDependOnAdj.add_lemma(word);
            }
        }
    }
    catch (...) {
        std::string m = "Problems with ROSS, word : " + piRossDict->m_Units[i].GetEntryStr();
        OutputErrorString(m);
    }
}


const char g_strRegRossDicPath[] = "Software\\Dialing\\Ross\\DictPath";

void CRusSyntaxOpt::InitOptionsLanguageSpecific() {
    m_piGramTab->LoadFromRegistry();

    //loading ross
    CDictionary piRossDict;
    std::string strPath = GetRegistryString(g_strRegRossDicPath);
    piRossDict.Load(strPath.c_str());
    LoadFromRoss(&piRossDict);
    auto rml = fs::path(GetRmlVariable());
    std::string Path = ( rml / "Dicts" / "SynAn").string();

    m_CompAdvList.read_from_file(MakePath(Path, "comp_adv.dat"));
    m_NounNumList.read_from_file(MakePath(Path, "noun_num.dat"));
    m_NumberAdverbsList.read_from_file(MakePath(Path,"num_pr.dat"));
    m_VerbsThatCanSubdueInfinitive.set_poses( _QM(VERB) | _QM(INFINITIVE) | _QM(ADVERB_PARTICIPLE) | _QM(PARTICIPLE_SHORT) | _QM(PARTICIPLE));
    m_VerbsThatCanSubdueInfinitive.read_from_file(MakePath(Path, "verbs_with_inf.txt"));

    m_pVerbsWithInstrObj.set_poses(_QM(VERB) | _QM(INFINITIVE) | _QM(ADVERB_PARTICIPLE) | _QM(PARTICIPLE_SHORT) | _QM(PARTICIPLE));
    m_pVerbsWithInstrObj.read_from_file(MakePath(Path,"verbs_with_inf.txt"));
    if (m_Professions.is_empty_list()) {
        // read it from file if thesaurus is disabled
        m_Professions.read_from_file(MakePath(Path, "profes.txt"));
    }

    std::vector<CFormInfo> Paradigms;
    GetLemmatizer()->CreateParadigmCollection(true, _R("нечего"), false, false, Paradigms);

    for (auto p: Paradigms) {
        BYTE POS = GetGramTab()->GetPartOfSpeech(p.GetAncode(0).c_str());
        if (POS == PRONOUN_PREDK) {
            m_lPradigmID_NECHEGO = p.GetParadigmId();
            m_Gramcode_NECHEGO = p.GetAncode(0);
            break;
        };
    };

    auto grs = _QM(rMasculinum)|_QM(rSingular)|_QM(rNominativ);
    m_MasSingNomNounGramCode = GetGramTab()->GetAllGramCodes(NOUN, grs, GrammemsEqu);
    assert(m_MasSingNomNounGramCode.length() == 2);
}


bool CRusSyntaxOpt::is_firm_group(int GroupType) const {
    return ((GroupType == WEB_ADDR)
            || (GroupType == NUMERALS)
            || (GroupType == C_NUMERALS)
            || (GroupType == NAMES)
            || (GroupType == KEYB)
            || (GroupType == FOREIGN_TERM)
            || (GroupType == OBOROTS)
    );
}


bool CRusSyntaxOpt::IsGroupWithoutWeight(int GroupType, const char *cause) const {
    //	группу ПРЯМ_ДОП не будем учитывать при подсчете веса, поскольку
    //	иначе на след. примерах синтаксис работает неверно:
    //		СПС и "Яблоко" не дали Немцову побыть акционером "Эха"
    //	для фразы "Я считал его великим учителем." специльно
    //	прописывается  частота омонима "его".


    if (GroupType == DIR_OBJ) return true;

    //  Если это ПГ, у которого главное слово - прилагательное, тогда
    //  нужно взять ее первую подгруппу (если она есть) и считать ее вес
    //  например: "у самого ленивого" - здесь будет считаться  вес группы
    //  "самого ленивого"

    if (cause
        && !strcmp(cause, "PP + adjective")
            )
        return true;


    //	группу ОТР_ФОРМА не будем учитывать при подсчете веса, поскольку
    //	иначе на след. примерах синтаксис работает неверно:
    //		"не дело тебе ходить в лес"
    if (GroupType == NEG_VERB) return true;


    // пример для группы НАР_ГЛАГ "Было приятно ощущать тепло разогретого солнцем камня."
    if (GroupType == ADV_VERB) return true;

    // "Куплю масла"
    if (GroupType == GEN_NOUN) return true;


    // "Как он раньше жил?"
    if (GroupType == FROM_COMPAR) return true;

    // пока не знаю примеров для:
    //if (GroupType == INSTR_OBJ) return  true;

    // "Серый рваный дым цеплялся за ветви елей , прижатый токами воздуха"
    if (GroupType == NOUN_PARTICIPLE) return true;

    return false;

};

bool CRusSyntaxOpt::IsSimilarGroup(int type) const {
    return (type == SIMILAR_ADJS)
           || (type == SIMILAR_NOUN_GROUPS)
           || (type == SIMILAR_INFS)
           || (type == SIMILAR_ADVS)
           || (type == SIMILAR_NUMERALS);
};
