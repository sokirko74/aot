// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "StdSynan.h"
#include "RusSentence.h"
#include "RusFormatCaller.h"
#include "RusWord.h"

CSentence *NewSentenceRussian(const CSyntaxOpt *pSyntaxOptions) {
    return new CRusSentence(pSyntaxOptions);
}

CFormatCaller *CRusSentence::GetNewFormatCaller() const {
    return new CRusFormatCaller(GetOpt());
};

CRusSentence::CRusSentence(const CSyntaxOpt *pSyntaxOptions) : CSentence(pSyntaxOptions) {
    InitClauseVectorRules();

    m_KOTORYI_INDEX = GetOpt()->GetOborDic()->FindSubConj(_R("КОТОРЫЙ"));
    m_CHEI_INDEX = GetOpt()->GetOborDic()->FindSubConj(_R("ЧЕЙ"));

}

CRusSentence::~CRusSentence() {
    m_vectorPrimitiveRules.clear();
}

void CRusSentence::ReadNextFromPlmLinesLanguageSpecific() {
    TryBuildVerbLemmaWithKa();
}

void CRusSentence::ChooseClauseType(const std::vector<SClauseType> &vectorTypes, CMorphVariant &V) {
    int empty_type = -1;

    for (int i = 0; i < vectorTypes.size(); i++) {
        if (!vectorTypes[i].m_Root.IsEmpty()) {
            int node = vectorTypes[i].m_Root.m_WordNo;
            int hom = vectorTypes[i].m_Root.m_HomonymNo;

            int iUnit = V.UnitNoByWordNo(node);
            assert (iUnit != -1);
            if (V.m_SynUnits[iUnit].m_iHomonymNum == hom) {
                V.m_ClauseTypeNo = i;
                return;
            }
        } else
            empty_type = i;

    }
    V.m_ClauseTypeNo = empty_type;

}

bool CRusSentence::RunSyntaxInClauses(ESynRulesSet type) {
    try {
        if (m_pSyntaxOptions == NULL)
            return false;

        CRusFormatCaller FormatCaller(GetOpt());
        bool bRebuildAllGroups = false;
        switch (type) {
            case SimpleSimilarRules : {
                FormatCaller.AddSimpleSimilarRules();
                break;
            }
            case RulesBeforeSimClauses: {
                FormatCaller.AddRulesBeforeSimClauses();
                break;
            }
            case RulesAfterSimClauses : {
                FormatCaller.AddRulesAfterSimClauses();
                break;
            }
            case AllRules              : {
                FormatCaller.AddAllRules();
                bRebuildAllGroups = true;

                break;
            }

            case GroupRulesForClause              : {
                FormatCaller.AddGroupRulesForClause();
                break;
            }

            default:
                return false;
        }


        //я бы с удовольствием здесь for_each запустил, да, блин, передавать надо 4 параметра
        for (int i = 0; i < GetClausesCount(); i++) {

            GetClause(i).BuildGroups(FormatCaller, bRebuildAllGroups);
            FormatCaller.Reset();
        }


        return true;
    }
    catch (...) {
        OutputErrorString("Failed RunSyntaxInClause");
        return false;
    }
}


void TryBuildVerbLemmaWithKa(CSynWord &W) {
    if (W.m_strWord.find(_R("-ка")) == std::string::npos) return;

    for (int i = 0; i < W.m_Homonyms.size(); i++) {
        if (W.m_Homonyms[i].HasPos(VERB))
            if ((W.m_Homonyms[i].HasGrammem(rPlural) &&
                 W.m_Homonyms[i].HasGrammem(rPastTense)) ||
                ((W.m_Homonyms[i].HasGrammem(rFirstPerson) ||
                  W.m_Homonyms[i].HasGrammem(rSecondPerson)) &&
                 W.m_Homonyms[i].HasGrammem(rImperative))) {
                CSynHomonym GoodHom = W.m_Homonyms[i];
                W.m_Homonyms.clear();
                W.m_Homonyms.push_back(GoodHom);
                return;
            }
    }
}

void CRusSentence::TryBuildVerbLemmaWithKa() {
    for (int i = 0; i < m_Words.size(); i++)
        ::TryBuildVerbLemmaWithKa(m_Words[i]);
}

void CRusSentence::TryToAddComparativeTypeToClause() {
    int i, j, k;
    for (i = 0; i < GetClausesCount(); i++) {
        if (GetClause(i).HasUnambiguousStrongRootWithoutWeakHomonyms()) continue;

        for (j = GetClause(i).m_iFirstWord; j <= GetClause(i).m_iLastWord; j++)
            for (k = 0; k < m_Words[j].GetHomonymsCount(); k++)
                if (((m_Words[j].GetSynHomonym(k).HasPos(ADJ_FULL))
                     || (m_Words[j].GetSynHomonym(k).HasPos(NUMERAL)) // больше, меньше
                    )
                    &&
                    (m_Words[j].GetSynHomonym(k).HasGrammem(rComparative))
                        ) {
                    SClauseType NewTyp(COMPARATIVE_T, j, k);
                    GetClause(i).m_vectorTypes.push_back(NewTyp);
                }

    }
}


void CRusSentence::DeleteHomOneToThousand() {
    for (int i = 0; i < m_Words.size(); i++) {
        for (int j = 0; j < m_Words[i].GetHomonymsCount(); j++) {
            CSynHomonym &HomF = m_Words[i].GetSynHomonym(j);
            for (int k = 0; k < m_Words[i].GetHomonymsCount(); k++) {
                CSynHomonym &HomS = m_Words[i].GetSynHomonym(k);
                if (HomF.m_lFreqHom > 0 && HomS.m_lFreqHom > 0) {
                    int iFrq = HomF.m_lFreqHom / HomS.m_lFreqHom;
                    if (iFrq >= 1000) HomS.m_bDelete = true;
                }
            }
        }

        m_Words[i].DeleteMarkedHomonymsBeforeClauses();
    }
}

void CRusSentence::AddWeightForSynVariantsInClauses() {
    for (int i = 0; i < GetClausesCount(); i++) {
        CClause &pClause = GetClause(i);

        pClause.AddVariantWeightWithHomOneToFifty();
    }
}


void CreateHomonymFor_NECHEGO(CSynHomonym &H, long plPardigmID, std::string psAncode, long plOldGrammems) {
    std::string s;
    H.m_lPradigmID = plPardigmID;

    H.m_lFreqHom = 1;
    s = _R("НЕЧЕГО");
    H.SetLemma(s);
    H.m_iCmpnLen = 6;
    H.m_LemSign = '+';
    H.m_CommonGramCode = "??";
    const CAgramtab *Agramtab = H.GetOpt()->GetGramTab();
    H.m_iGrammems = Agramtab->GetAllGrammems(psAncode.c_str());
    H.m_iPoses = (1 << PRONOUN_PREDK);


    H.m_iGrammems &= ~_QM(rGenitiv);
    int lGramm = H.m_iGrammems;
    std::vector<int> v_gramm;

    if ((plOldGrammems & _QM(rGenitiv)) != 0) {
        H.m_iGrammems |= _QM(rGenitiv);
        lGramm |= _QM(rGenitiv);
        v_gramm.push_back(lGramm);
        lGramm &= ~_QM(rGenitiv);
    }

    if ((plOldGrammems & _QM(rDativ)) != 0) {
        H.m_iGrammems |= _QM(rDativ);
        lGramm |= _QM(rDativ);
        v_gramm.push_back(lGramm);
        lGramm &= ~_QM(rDativ);
    }

    if ((plOldGrammems & _QM(rAccusativ)) != 0) {
        H.m_iGrammems |= _QM(rAccusativ);
        lGramm |= _QM(rAccusativ);
        v_gramm.push_back(lGramm);
        lGramm &= ~_QM(rAccusativ);
    }

    if ((plOldGrammems & _QM(rInstrumentalis)) != 0) {
        H.m_iGrammems |= _QM(rInstrumentalis);
        lGramm |= _QM(rInstrumentalis);
        v_gramm.push_back(lGramm);
        lGramm &= ~_QM(rInstrumentalis);
    }

    if ((plOldGrammems & _QM(rLocativ)) != 0) {
        H.m_iGrammems |= _QM(rLocativ);
        lGramm |= _QM(rLocativ);
        v_gramm.push_back(lGramm);
        lGramm &= ~_QM(rLocativ);
    }

    for (int i = 0; i < v_gramm.size(); i++) {
        std::string strPos = H.GetPartOfSpeechStr();
        std::string strHelpGr;
        strHelpGr = Agramtab->GrammemsToStr(v_gramm[i]);

        std::string n_code;
        BYTE iPos;
        QWORD iGrm;
        strPos += " ";
        strPos += strHelpGr;
        bool Result = Agramtab->ProcessPOSAndGrammems(strPos.c_str(), iPos, iGrm);
        assert (Result);
        Result = Agramtab->GetGramCodeByGrammemsAndPartofSpeechIfCan(iPos, iGrm, n_code);
        assert (Result);

        H.SetGramCodes(H.GetGramCodes() + n_code);
    }


}


void CRusSentence::DisruptPronounPredik() {
    for (size_t i = 0; i + 2 < m_Words.size(); i++) {
        if (m_Words[i + 0].HasOborot1() != m_Words[i + 0].HasOborot2()) continue;
        if (m_Words[i + 1].HasOborot1() != m_Words[i + 1].HasOborot2()) continue;
        if (m_Words[i + 2].HasOborot1() != m_Words[i + 2].HasOborot2()) continue;

        int ihom = m_Words[i + 2].GetHomonymByPOS(PRONOUN);
        if (m_Words[i].FindLemma(_R("НЕ"))
            && m_Words[i + 1].GetHomonymByPOS(PREP) != -1
            && m_Words[i + 2].FindLemma(_R("ЧТО"))
            && ihom != -1
                ) {
            QWORD lOldGrammems = m_Words[i + 2].m_Homonyms[ihom].m_iGrammems;
            m_Words[i + 2].m_Homonyms.clear();
            CSynHomonym Hom_nechego(this);
            CreateHomonymFor_NECHEGO(Hom_nechego, GetOpt()->m_lPradigmID_NECHEGO, GetOpt()->m_Gramcode_NECHEGO,
                                     lOldGrammems);
            m_Words[i + 2].m_Homonyms.push_back(Hom_nechego);
            MarkWordAsDeleted(i);
        }

    }


}


const long CommonNounPrefixesCount = 2;
const std::string CommonNounPrefixes[CommonNounPrefixesCount] = {_R("ВИЦЕ-"), _R("ЭКС-")};


void CreateHomonymFor_EksVice(CSynHomonym &H, long plPardigmID, std::string psAncode, std::string sLem, std::string TypeAncode) {
    const CAgramtab *Agramtab = H.GetOpt()->GetGramTab();
    H.m_lPradigmID = plPardigmID;
    H.SetGramCodes(psAncode);
    H.m_lFreqHom = 1;
    H.SetLemma(sLem);
    H.m_iCmpnLen = sLem.length();
    H.m_LemSign = '+';
    H.m_CommonGramCode = TypeAncode;
    H.m_iGrammems = Agramtab->GetAllGrammems(psAncode.c_str());
    H.m_iPoses = (1 << Agramtab->GetPartOfSpeech(psAncode.c_str()));

}

void CRusSentence::CutPrefixEksAndVize() {

    for (int ii = 0; ii < m_Words.size(); ii++) {
        std::string Word = m_Words[ii].m_strUpperWord;
        std::vector<int> Prefixes;
        for (long i = 0; i < CommonNounPrefixesCount; i++)
            if (CommonNounPrefixes[i] == Word.substr(0, CommonNounPrefixes[i].length())) {
                Prefixes.push_back(i);
                Word.erase(0, CommonNounPrefixes[i].length());
                i = -1;
                if (Word.empty()) break;
            };
        if (Prefixes.empty()) continue;


        std::vector<CFormInfo> Paradigms;
        GetOpt()->GetLemmatizer()->CreateParadigmCollection(false, Word, false, false, Paradigms);

        if (Paradigms.size() < 1) continue;

        std::vector<CSynHomonym> vec_Homonyms;
        for (long k = 0; k < Paradigms.size(); k++) {
            long lParadigm = Paradigms[k].GetParadigmId();
            std::string TypeAncode = Paradigms[k].GetCommonAncode();
            std::string sGramatCode = Paradigms[k].GetSrcAncode();
            std::string sLemma = Paradigms[k].GetWordForm(0);
            if (!binary_search(GetOpt()->m_pProfessions->m_vectorDatItems.begin(),
                               GetOpt()->m_pProfessions->m_vectorDatItems.end(), sLemma))
                continue;
            CSynHomonym NewHom(this);
            CreateHomonymFor_EksVice(NewHom, lParadigm, sGramatCode, sLemma, TypeAncode);
            vec_Homonyms.push_back(NewHom);
        }

        if (vec_Homonyms.size() > 0) {
            for (long i = 0; i < Prefixes.size(); i++) {
                m_Words[ii].m_UnparsedGraphemDescriptorsStr += " " + CommonNounPrefixes[Prefixes[i]];
            };

            m_Words[ii].m_Homonyms.clear();
            for (int j = 0; j < vec_Homonyms.size(); j++)
                m_Words[ii].m_Homonyms.push_back(vec_Homonyms[j]);
        }

    }
}

//менять граммемы подлежащего и группы, в которую он входит только после того,
//как все клаузы уже построены (после операций объединения и вложения клауз), т.е.
//когда отработают все правила фрагментации.
//раньше чистка граммем происходила в ф-ции CFormatCaller::find_subj_for_verb
//пример: "Стол, что за нами пристально следит, преследовал меня весь день"
//при первоначальной фрагментации подлежащим становился "день", а после объединения клауз "стол",
//поэтому терялся вн. у "день".    
void ChangeSubjAndItsGroupGrammemsInClause(CClause &C, SVI pSynVar) {
    if (pSynVar->m_Subjects.size() != 1) return;


    int iSbj = pSynVar->GetUnitLastWord(pSynVar->GetFirstSubject());
    assert(iSbj != -1);
    int iPrdk = pSynVar->GetUnitLastWord(pSynVar->m_iPredk);
    assert(iPrdk != -1);

    CGroup *GrpWithSubj = const_cast<CGroup *>( C.GetLastHost(iSbj, pSynVar));
    const CGroup *GrpWithPredk = C.GetLastHost(iPrdk, pSynVar);

    QWORD ipredk_gram = 0;
    if (!GrpWithPredk)
        ipredk_gram = pSynVar->m_SynUnits[pSynVar->m_iPredk].m_iGrammems;
    else
        ipredk_gram = GrpWithPredk->GetGrammems();

    QWORD isubj_gram = 0;
    if (!GrpWithSubj)
        isubj_gram = pSynVar->m_SynUnits[pSynVar->GetFirstSubject()].m_iGrammems;
    else
        isubj_gram = GrpWithSubj->GetGrammems();

    std::string debug = C.GetOpt()->GetGramTab()->GrammemsToStr(isubj_gram);
    debug = C.GetOpt()->GetGramTab()->GrammemsToStr(ipredk_gram);
    //if( isubj_gram & rAllGenders & ipredk_gram == 0) return; //разный род
    QWORD new_subj_grammems = (ipredk_gram & isubj_gram & rAllNumbers) | _QM(rNominativ);
    debug = C.GetOpt()->GetGramTab()->GrammemsToStr(new_subj_grammems);
    if (!(new_subj_grammems & rAllNumbers)) new_subj_grammems |= rAllNumbers;
    int GT = GrpWithSubj ? GrpWithSubj->m_GroupType : -1;
    bool numeral = (GT == NUMERAL_NOUN || GT == NOUN_NUMERAL || GT == NUMERAL_ADVERB || GT == NUMERALS ||
                    GT == NOUN_NUMERAL_APPROX);
    if (numeral)
        isubj_gram |=
                _QM(rSingular) | _QM(rNeutrum); //группа, вводимая количественным наречием: В углу стояло 5 стульев
    if (((CRusGramTab *) C.GetOpt()->GetGramTab())->ConflictGrammems(isubj_gram, ipredk_gram,
                                                                     rAllGenders | rAllNumbers))  //разный род или число
        return;

    // setting grammems  for group and relation
    if (GrpWithSubj) {
        size_t breaks = rAllCases | rAllNumbers;
        GrpWithSubj->SetGrammems(isubj_gram & (new_subj_grammems | ~breaks));// GrpWithSubj->GetGrammems()
        for (int i = 0; i < pSynVar->m_vectorGroups.m_Relations.size(); i++) {
            if (GrpWithSubj->m_MainWordNo == pSynVar->m_vectorGroups.m_Relations[i].m_iFirstWord &&
                !((CRusGramTab *) C.GetOpt()->GetGramTab())->ConflictGrammems(
                        pSynVar->m_vectorGroups.m_Relations[i].m_iGrammems, GrpWithSubj->GetGrammems(), breaks)) {
                pSynVar->m_vectorGroups.m_Relations[i].m_iGrammems = GrpWithSubj->GetGrammems();
                if (GrpWithSubj->m_GroupType == NOUN_ADJ) {
                    int AdjUnitNo = pSynVar->UnitNoByWordNo(pSynVar->m_vectorGroups.m_Relations[i].m_iLastWord);
                    assert (AdjUnitNo < pSynVar->m_SynUnits.size());
                    pSynVar->m_SynUnits[AdjUnitNo].ModifyGrammems(new_subj_grammems);
                };
            }
        }
    }
    rml_TRACE("ChangeSubjAndItsGroupGrammemsInClause %d\n", pSynVar->GetFirstSubject());
    pSynVar->m_SynUnits[pSynVar->GetFirstSubject()].ModifyGrammems(new_subj_grammems);
}

void CRusSentence::ChangeSubjAndItsGroupGrammems() {
    for (int i = 0; i < GetClausesCount(); i++) {
        CClause &PrCl = GetClause(i);
        for (SVI pSynVar = PrCl.m_SynVariants.begin(); pSynVar != PrCl.m_SynVariants.end(); pSynVar++) {
            if (pSynVar->HasSubj())
                ChangeSubjAndItsGroupGrammemsInClause(PrCl, pSynVar);
        }
    }
}

bool CRusSentence::IsRelativSentencePronoun(int ClauseStartWordNo, int WordNo, int &HomonymNo) const {
    int SubordConjNo = m_Words[WordNo].m_SubordinateConjNo;
    HomonymNo = 0;

    // "кое-чьи" не является релативным словом, а морфология  его не знает, поэтому возникает омоним "чей"
    if (m_Words[WordNo].m_strWord.find('-') != std::string::npos) return false;


    if (m_Words[WordNo].m_Homonyms.empty()) return false;
    return (SubordConjNo == m_KOTORYI_INDEX)
           || (SubordConjNo == m_CHEI_INDEX);
};


EClauseType CRusSentence::GetClauseTypeByAncodePattern(const CAncodePattern &Pattern) const {

    if (Pattern.HasPos(VERB))
        return VERB_PERS_T;
    else if (Pattern.HasPos(ADVERB_PARTICIPLE))
        return ADVERB_PARTICIPLE_T;
    else if (Pattern.HasPos(PARTICIPLE_SHORT))
        return PARTICIPLE_SHORT_T;
    else if (Pattern.HasPos(ADJ_SHORT))
        return ADJ_SHORT_T;
    else if (Pattern.HasPos(PREDK))
        return PREDK_T;
    else if (Pattern.HasPos(PARTICIPLE))
        return PARTICIPLE_T;
    else if (Pattern.HasPos(INFINITIVE))
        return INFINITIVE_T;
    else if (Pattern.HasPos(INP))
        return INP_T;
    else if ((Pattern.HasPos(ADJ_FULL) || Pattern.HasPos(NUMERAL)) && Pattern.HasGrammem(rComparative))
        return COMPARATIVE_T;

    //возвращаем  несуществующую часть речи
    return UnknownPartOfSpeech;
};



//функция  идет по всем словам предложение, если слово вошло в графематическую вилку  ФИ1-ФИ2,
//тогда это слово не считается  союзом
//Например, "И" в "И.А.Кирилловой"  не будет союзом.

void CRusSentence::ProcessFio1Fio2() {
    bool bInFIO = false;
    for (int i = 0; i < m_Words.size(); i++) {
        if (m_Words[i].HasDes(OFAM1))
            bInFIO = true;

        if (bInFIO)
            for (int k = 0; k < m_Words[i].m_Homonyms.size(); k++)
                m_Words[i].m_Homonyms[k].m_CoordConjNo = -1;

        if (m_Words[i].HasDes(OFAM2))
            bInFIO = false;
    };


};


// если в клаузе нашлись "который" или "чeй", тогда  омонимы вершины  на причастие или на деепричастие надо удалить
void CRusSentence::DeleteSomeTypesInRelativeClauses() {
    for (size_t ClauseNo = 0; ClauseNo < m_Clauses.size(); ClauseNo++) {
        CClause &C = m_Clauses[ClauseNo];
        if (C.IsRelative()) {
            for (int i = C.m_vectorTypes.size() - 1; i >= 0; i--)
                if ((C.m_vectorTypes[i].m_Type == PARTICIPLE_T)
                    || (C.m_vectorTypes[i].m_Type == ADVERB_PARTICIPLE_T)
                        )
                    if (m_Words[C.m_vectorTypes[i].m_Root.m_WordNo].m_Homonyms.size() > 1)
                        C.DeleteHomonym(C.m_vectorTypes[i].m_Root.m_WordNo, C.m_vectorTypes[i].m_Root.m_HomonymNo);
        };
    };
};

bool CRusSentence::IsProfession(const CSynHomonym &H) const {
    return H.CompareWithPredefinedWords(*GetOpt()->m_pProfessions);
};

bool CRusSentence::BuildClauses() {
    ProcessFio1Fio2();

    SolveAmbiguityUsingRuleForShortAdj();
    DeleteHomonymsIfTooManyPredictedWords();
    m_bPanicMode = IsPanicSentence();
    if (m_bPanicMode) {
        rml_TRACE("!!!!!!!!!!  Entering panic mode !!!!!!!!!!!1\n");
    };

    CRusSentence SavedSentence(GetOpt());
    SavedSentence = *this;
    m_bShouldUseTwoPotentialRule = !m_bPanicMode;
    TryWithoutTwoPotentialRule:
    bool bRes = true;


    assert (GetClausesCount() == 0);

    // соединение предикатива нечего: "Вам не о чем волноваться"
    DisruptPronounPredik();


    // удаление префиксов ВИЦЕ- и ЭКС-
    CutPrefixEksAndVize();


    //	удаление омонимов вводных слов, если эти слова не выделены запятыми и пр.

    DetermineParenthesis();

    //	удаление омонимов, частоты которых соотносятся 1/1000

    DeleteHomOneToThousand();

    FindGraPairs();

    CloneHomonymsForOborots();

    AscribeSimplePrepositionInterpretations(PREP);

    FindAllTermins();

    bool SecondTryOfCoverageKillHomonyms = false;
    std::vector<CSynWord> SaveWords = m_Words;
    std::vector<SFoundTermin> SaveFoundTermins = m_vectorTermins;


    BuildInitialClausesLabel:

    TraceClauses();

    if (!BuildInitialClauses()) {
        return false;

    };

    TraceClauses();

    DeleteSomeTypesInRelativeClauses();


    TraceClauses();

    rml_TRACE("BuildAnalyticalVerbForms");
    BuildAnalyticalVerbForms();


    rml_TRACE("TryToAddComparativeTypeToClause");
    TryToAddComparativeTypeToClause();

    rml_TRACE("TryToRebuildDashInClause");
    TraceClauses();
    TryToRebuildDashInClause();
    TraceClauses();

    rml_TRACE("RunSyntaxInClause(RulesBeforeSimClauses)");
    RunSyntaxInClauses(RulesBeforeSimClauses);

    // после первого вызова RunSyntaxInClause нужно удалить омонимы, которые противоречат найденным терминам
    rml_TRACE("DeleteMarkedHomonymsWithClauses");
    DeleteMarkedHomonymsWithClauses(CPeriod(0, (int) m_Words.size() - 1));

    rml_TRACE("InitWordsInTermins");
    InitWordsInTermins();


    assert (IsValid());

    if (!m_bPanicMode) {
        rml_TRACE("OneRunOfClauseRules(m_vectorPrimitiveRules)\n");
        OneRunOfClauseRules(m_vectorPrimitiveRules);
        assert (IsValid());

        rml_TRACE("RunSyntaxInClause(RulesAfterSimClauses)\n");
        RunSyntaxInClauses(RulesAfterSimClauses);


        rml_TRACE("OneRunOfClauseRules(m_vectorMainEncloseRules)\n");
        OneRunOfClauseRules(m_vectorMainEncloseRules);
        assert (IsValid());

        rml_TRACE("RunSyntaxInClause(GroupRulesForClause)\n");
        RunSyntaxInClauses(GroupRulesForClause);

        rml_TRACE("OneRunOfClauseRules(m_vectorDisruptRules)\n");
        OneRunOfClauseRules(m_vectorDisruptRules);
        assert (IsValid());
    };

    rml_TRACE("RunSyntaxInClause(AllRules)\n");
    RunSyntaxInClauses(AllRules);

    // чтобы  собиралась группа ПРИДАТ_ОПР в примере "баба, которая много пьет" нужно еще раз запустить
    // GroupRulesForClause, поскольку OneRunOfClauseRules(m_vectorDisruptRules), в котором эта клауза  вложилась, только что отработало.
    rml_TRACE("RunSyntaxInClause(GroupRulesForClause)\n");
    RunSyntaxInClauses(GroupRulesForClause);

    rml_TRACE("BuildClauseRelations()\n");
    BuildClauseRelations();
    assert (IsValid());

    rml_TRACE("RecalculateRelationsCoordinates()\n");
    RecalculateRelationsCoordinates();
    AssignClauseNoToWords();
    assert (IsValid());

    rml_TRACE("AddWeightForSynVariantsInClauses()\n");
    AddWeightForSynVariantsInClauses();
    assert (IsValid());



    //менять граммемы подлежащего и группы, в которую оно входит, нужно  только после того,
    //как все клаузы уже построены (после операций объединения и вложения клауз), т.е.
    //когда отработают все правила фрагментации.
    //Раньше чистка граммем происходила в ф-ции CFormatCaller::find_subj_for_verb
    //пример: "Стол, что за нами пристально следит, преследовал меня весь день"
    //при первоначальной фрагментации подлежащим становился "день", а после объединения клауз "стол",
    //поэтому терялся вн. у "день".
    ChangeSubjAndItsGroupGrammems();


    if (!m_bPanicMode) {
        for (int tt = 0; tt < GetClausesCount(); tt++)
            if (0 == GetClause(tt).GetSynVariantsCount())
                if (m_bShouldUseTwoPotentialRule) {
                    *this = SavedSentence;
                    m_bShouldUseTwoPotentialRule = false;
                    rml_TRACE("goto TryWithoutTwoPotentialRule\n");
                    goto TryWithoutTwoPotentialRule;
                }


        if (m_pSyntaxOptions->m_KillHomonymsMode == CoverageKillHomonyms) {

            //	если  были удалены омонимы и программа организует еще один проход  (только один!)

            rml_TRACE("KillHomonymsInAllSentence\n");
            if (KillHomonymsInAllSentence()
                && !SecondTryOfCoverageKillHomonyms
                    ) {
                IsValid();
                for (int ClauseNo = 0; ClauseNo < GetClausesCount(); ClauseNo++) {
                    const CClause &Clause = GetClause(ClauseNo);

                    //	если остались ПУСТЫХи или клаузы  c пустым кол-вом вариантов

                    if (Clause.m_vectorTypes.empty()
                        //||		Clause.m_SynVariants.empty()
                            ) {
                        SecondTryOfCoverageKillHomonyms = true;

                        //Мы не можем здесь оставить вектор слов(m_Words) так, как он есть,
                        //поскольку будем заново вызывать функцию BuildAnalyticalVerbForms, которая  будет строить
                        //анал. формы,т.е. удалять и менять  сами слова.
                        //Мы так же не можем  сделать m_Words = SaveWords,
                        //поскольку тогда  восстановятся все омонимы,которые были удалены KillHomonymsInAllSentence.
                        //Поэтому мы делаем следующее.

                        std::vector<CSynWord> p;
                        for (int i = 0; i < SaveWords.size(); i++) {
                            int j = 0;
                            for (; j < m_Words.size(); j++)
                                if ((SaveWords[i].m_GraphematicalUnitOffset == m_Words[j].m_GraphematicalUnitOffset)
                                    && (SaveWords[i].m_strWord == m_Words[j].m_strWord)
                                        )
                                    break;
                            if (j < m_Words.size()) {
                                p.push_back(m_Words[j]);
                                p.back().m_MainVerbs = SaveWords[i].m_MainVerbs;
                                p.back().m_TrennbarePraefixWordNo = SaveWords[i].m_TrennbarePraefixWordNo;
                            } else
                                p.push_back(SaveWords[i]);

                        };
                        m_Words = p;
                        m_vectorTermins = SaveFoundTermins;
                        rml_TRACE("goto  BuildInitialClausesLabel\n");
                        goto BuildInitialClausesLabel;
                    };
                };
            };


        };
    };


    assert (IsValid());


    return true;

}

//клонирование  оборотов сделано для того, чтобы  возникла омонимия между  оборотом и не оборотом.
//Например, на отрезке "то есть" найден оборот(союз). На этом же отрезке моежт быть построено подлежащее
//и сказуемое. В итоге,  мы должны получить два равноправных варианта: с оборотом и c  подлежащим и сказуемым.

void CRusSentence::CloneHomonymsForOborots() {
    //  идем по всем графематическим вилкам,
    for (int i = 0; i < m_Words.size(); i++) {
        CSynWord &FirstWord = m_Words[i];
        if (!FirstWord.HasOborot1()) continue;

        if (!FirstWord.HasOborot2()) {
            // многословные обороты
            for (int j = i; j < m_Words.size(); j++) {
                //"судя по многочисленным фак
                std::string title = GetOpt()->GetOborDic()->m_Entries[m_Words[j].m_Homonyms[0].m_OborotNo].m_OborotEntryStr;
                EngRusMakeUpper(title);
                int b = title.find(m_Words[j].m_strUpperWord + "(");
                if (b != -1) {
                    b += m_Words[j].m_strUpperWord.length();
                    std::string GramFet = title.substr(b + 1, title.find(")") - b - 1);
                    uint32_t Pose;
                    QWORD Grammems;
                    {
                        Trim(GramFet);
                        QWORD G;
                        BYTE Pos;
                        const CAgramtab *R = GetOpt()->GetGramTab();
                        if (R->ProcessPOSAndGrammemsIfCan(GramFet.c_str(), &Pos, &G)) {
                            Grammems = G;
                            if (Pos == VERB)
                                Pose = (1 << VERB) | (1 << INFINITIVE) | (1 << ADVERB_PARTICIPLE) | (1 << PARTICIPLE) |
                                       (1 << PARTICIPLE_SHORT);
                            else
                                Pose = (1 << (size_t) Pos);
                        } else {
                            GramFet = _R("С ") + GramFet;
                            if (R->ProcessPOSAndGrammemsIfCan(GramFet.c_str(), &Pos, &G)) {
                                Grammems = G;
                                Pose = 0xffffffff;
                            } else {
                                Grammems = 0xffffffff;
                                Pose = 0xffffffff;
                            };

                        };
                    }
                    int k = 0;
                    for (; k < m_Words[j].GetHomonymsCount(); k++) {
                        CSynHomonym &H = m_Words[j].GetSynHomonym(k);
                        if (!(Pose & H.m_iPoses))
                            continue;
                        if (Grammems > 0 && !(Grammems & (H.m_iGrammems | H.m_TypeGrammems)))
                            continue;
                        break;
                    }
                    b = k < m_Words[j].GetHomonymsCount() ? k : -1;
                }
                m_Words[j].CloneHomonymForOborot(b);
                if (m_Words[j].HasOborot2()) break;
            }
        } else {
            // однословные обороты
            for (int k = 0; k < FirstWord.GetHomonymsCount(); k++) {
                CSynHomonym &H = FirstWord.GetSynHomonym(k);
                if (H.m_OborotNo != -1) {
                    const COborotForSyntax &O = GetOpt()->GetOborDic()->m_Entries[H.m_OborotNo];
                    //Проверяем соответствие найденных  графематикой   оборотов и морфологии.
                    //Для омонимов, которые не являются союзом, предлогом, вводным словом или наречием ("GF      = 1  НАР")
                    //интерпретация  в словаре оборотов удаляется. Так, например, для омонима "ПОСОЛ" слова "после"
                    //оборотная интерпретация будет удалена, хотя во входном файле EXPR_NO стоял на всех омонимах.
                    if (!(H.m_iPoses & O.m_Poses))
                        H.DeleteOborotMarks();
                }
            }
        }
    }
};





