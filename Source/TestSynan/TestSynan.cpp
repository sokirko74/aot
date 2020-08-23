#include <stdio.h>
#include "../SynCommonLib/RelationsIterator.h"
#include "../SynanLib/SyntaxHolder.h"
#include "../common/util_classes.h"
#include "../common/argparse.h"

std::vector<std::string> GetAnanlytForms(const CSentence &Sentence) {
    std::vector<std::string>  result;
    for (int WordNo = 0; WordNo < Sentence.m_Words.size(); WordNo++) {
        const CSynWord &W = Sentence.m_Words[WordNo];
        if (!W.m_MainVerbs.empty()) {
            std::string form = W.m_strWord.c_str();
            for (size_t i = 0; i < W.m_MainVerbs.size(); i++) {
                form += std::string(" ") + Sentence.m_Words[W.m_MainVerbs[i]].m_strWord;

                const CSynWord &W_1 = Sentence.m_Words[W.m_MainVerbs[i]];
                for (size_t j = 0; j < W_1.m_MainVerbs.size(); j++)
                    form += std::string(" ") + Sentence.m_Words[W_1.m_MainVerbs[j]].m_strWord;
            };
            result.push_back(form);
        }
    }
    return result;
}

std::string GetWords(const CSentence &Sentence, const CPeriod &P) {
    std::string S;
    for (int WordNo = P.m_iFirstWord; WordNo <= P.m_iLastWord; WordNo++) {
        S += Sentence.m_Words[WordNo].m_strWord;
        if (WordNo < P.m_iLastWord)
            S += " ";
    };
    return S;
}

nlohmann::json GetGroups(const CSentence &Sentence, const CAgramtab &A) {
    int nClausesCount = Sentence.GetClausesCount();
    nlohmann::json  clauses = nlohmann::json::array();

    for (int ClauseNo = 0; ClauseNo < nClausesCount; ClauseNo++) {
        const CClause &Clause = Sentence.GetClause(ClauseNo);
        int nCvar = Clause.m_SynVariants.size();

        if (Clause.m_SynVariants.empty()) continue;
        nlohmann::json clause;
        clause["start"] = Clause.m_iFirstWord;
        clause["last"] = Clause.m_iLastWord;
        clause["words"] =  GetWords(Sentence, Clause);
        clause["good_synvars"] = nlohmann::json::array();


        if (!Clause.m_RelativeWord.IsEmpty())
        {
            clause["relative"] = Sentence.m_Words[Clause.m_RelativeWord.m_WordNo].m_strWord;
        };
        if (Clause.m_AntecedentWordNo != -1)
        {
            clause["antecedent"] = Sentence.m_Words[Clause.m_AntecedentWordNo].m_strWord;
        };


        int nVmax = Clause.m_SynVariants.begin()->m_iWeight;
        for (CSVI pSynVar = Clause.m_SynVariants.begin(); pSynVar != Clause.m_SynVariants.end(); pSynVar++) {
            if (pSynVar->m_iWeight < nVmax) break;

            const CMorphVariant &V = *pSynVar;
            nlohmann::json synvar;
            // print the clause
            {
                int ClauseType = (V.m_ClauseTypeNo == -1) ? UnknownSyntaxElement
                                                          : Clause.m_vectorTypes[V.m_ClauseTypeNo].m_Type;;
                if (ClauseType != UnknownSyntaxElement)
                    synvar["type"] = (const char *) A.GetClauseNameByType(ClauseType);
                else
                    synvar["type"] = "EMPTY";
            }
            nlohmann::json groups = nlohmann::json::array();
            for (size_t GroupNo = 0; GroupNo < V.m_vectorGroups.GetGroups().size(); GroupNo++) {
                const CGroup &G = V.m_vectorGroups.GetGroups()[GroupNo];
                groups.push_back({
                    {"type", Sentence.GetOpt()->GetGroupNameByIndex(G.m_GroupType)},
                    {"words", GetWords(Sentence, G)},
                });

            };
            synvar["groups"] = groups;
            clause["good_synvars"].push_back(synvar);
        }
        clauses.push_back(clause);
    }

    return clauses;
}

std::string GetNodeStr(const CSentence &Sentence, const CRelationsIterator &RelIt, int GroupNo, int WordNo) {
    if (GroupNo != -1)
        return GetWords(Sentence, RelIt.GetFirmGroups()[GroupNo]);
    else
        return Sentence.m_Words[WordNo].m_strWord;
}

std::string GetNodeGrmStr(const CSentence &Sentence, const CRelationsIterator &RelIt, int GroupNo, int WordNo, std::string &Lemma) {
    Lemma = "";
    if (GroupNo != -1)
        return "";
    else {
        size_t ClauseNo = Sentence.GetMinClauseByWordNo(WordNo);
        const CClause &Clause = Sentence.GetClause(ClauseNo);
        const CMorphVariant *pSynVar = &*Clause.GetSynVariantByNo(0);
        int UnitNo = pSynVar->UnitNoByWordNo(WordNo);
        const CSynUnit &U = pSynVar->m_SynUnits[UnitNo];
        Lemma = Sentence.GetWords()[WordNo].GetHomonym(U.m_iHomonymNum)->m_strLemma;
        return Sentence.GetOpt()->GetGramTab()->GrammemsToStr(U.m_iGrammems | U.m_TypeGrammems);
    }
}

nlohmann::json GetRelations(const CSentence &Sentence) {
    CRelationsIterator RelIt;
    RelIt.SetSentence(&Sentence);
    for (int i = 0; i < Sentence.m_vectorPrClauseNo.size(); i++)
        RelIt.AddClauseNoAndVariantNo(Sentence.m_vectorPrClauseNo[i], 0);
    RelIt.BuildRelations();
    nlohmann::json  rels = nlohmann::json::array();

    for (long RelNo = 0; RelNo < RelIt.GetRelations().size(); RelNo++) {
        const CSynOutputRelation &piRel = RelIt.GetRelations()[RelNo];
        std::string RelName = Sentence.GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type);
        std::string Src = GetNodeStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord);
        std::string Trg = GetNodeStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord);
        std::string SrcLemma, TrgLemma;
        std::string SrcGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord, SrcLemma);
        std::string TrgGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord, TrgLemma);
        std::string GramRel = Sentence.GetOpt()->GetGramTab()->GrammemsToStr(piRel.m_Relation.m_iGrammems);
        rels.push_back({
            {"src", Src},
            {"trg", Trg},
            {"name", RelName},
            {"gramrel", GramRel},
            {"src_lemma", SrcLemma},
            {"src_grm", SrcGrm},
            {"trg_lemma", TrgLemma},
            {"trg_grm", TrgGrm},
         });
    }
    return rels;
}

nlohmann::json GetResultBySyntax(const CSentencesCollection &SC, const CAgramtab &A) {
    nlohmann::json sents = nlohmann::json::array();
    for (size_t nSent = 0; nSent < SC.m_vectorSents.size(); nSent++) {
        const CSentence &Sentence = *SC.m_vectorSents[nSent];
        nlohmann::json sent = {
            {"analytical",  GetAnanlytForms(Sentence)},
            {"groups",  GetGroups(Sentence, A)},
            {"relations",  GetRelations(Sentence)},
        };
        sents.push_back(sent);
    }
    ConvertToUtfRecursive(sents, A.m_Language);
    return sents;
};


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.AddOption("--input-is-list-file");
    parser.Parse(argc, argv);
}


int main(int argc, const char **argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);

    CSyntaxHolder H;
    if (!H.LoadSyntax(args.GetLanguage())) {
        std::cerr << "initialization error\n";
        return 1;
    };
    std::cerr << "ok\n";

    try {
        CTestCaseBase base;
        base.read_test_cases(args.GetInputStream());
        for (auto& t : base.TestCases) {
            if (!t.Text.empty()) {
                H.GetSentencesFromSynAn(t.Text, false);
                t.Result = GetResultBySyntax(H.m_Synan, *H.m_pGramTab);
            }
        }
        base.write_test_cases(args.GetOutputStream());
    }
    catch (...) {
        std::cerr << "an exception occurred!\n";
        return 1;
    };

    return 0;
};


