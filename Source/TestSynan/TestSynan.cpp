#include <stdio.h>
#include "../SynCommonLib/RelationsIterator.h"
#include "../SynanLib/SyntaxHolder.h"

#include "../common/argparse.h"

void GetAnanlytForms(const CSentence &Sentence, std::string &Res) {
    for (int WordNo = 0; WordNo < Sentence.m_Words.size(); WordNo++) {
        const CSynWord &W = Sentence.m_Words[WordNo];
        if (!W.m_MainVerbs.empty()) {
            Res += std::string("\t<analyt> ") + W.m_strWord.c_str();
            for (size_t i = 0; i < W.m_MainVerbs.size(); i++) {
                Res += std::string(" ") + Sentence.m_Words[W.m_MainVerbs[i]].m_strWord;

                const CSynWord &W_1 = Sentence.m_Words[W.m_MainVerbs[i]];
                for (size_t j = 0; j < W_1.m_MainVerbs.size(); j++)
                    Res += std::string(" ") + Sentence.m_Words[W_1.m_MainVerbs[j]].m_strWord;
            };
            Res += "</analyt>\n";
        }
    }
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

void GetGroups(const CSentence &Sentence, const CAgramtab &A, std::string &Res) {
    int nClausesCount = Sentence.GetClausesCount();

    for (int ClauseNo = 0; ClauseNo < nClausesCount; ClauseNo++) {
        const CClause &Clause = Sentence.GetClause(ClauseNo);
        int nCvar = Clause.m_SynVariants.size();

        if (Clause.m_SynVariants.empty()) continue;

        int nVmax = Clause.m_SynVariants.begin()->m_iWeight;
        for (CSVI pSynVar = Clause.m_SynVariants.begin(); pSynVar != Clause.m_SynVariants.end(); pSynVar++) {
            if (pSynVar->m_iWeight < nVmax) break;

            const CMorphVariant &V = *pSynVar;
            Res += Format("\t<synvar>\n");

            // print the clause
            {
                int ClauseType = (V.m_ClauseTypeNo == -1) ? UnknownSyntaxElement
                                                          : Clause.m_vectorTypes[V.m_ClauseTypeNo].m_Type;;
                std::string Type;
                if (ClauseType != UnknownSyntaxElement)
                    Type = (const char *) A.GetClauseNameByType(ClauseType);
                else
                    Type = "EMPTY";
                Res += Format("\t\t<clause type=\"%s\">%s</clause>\n", Type.c_str(),
                              GetWords(Sentence, Clause).c_str());
            }


            for (int GroupNo = 0; GroupNo < V.m_vectorGroups.GetGroups().size(); GroupNo++) {
                const CGroup &G = V.m_vectorGroups.GetGroups()[GroupNo];
                Res += Format("\t\t<group type=\"%s\">%s</group>\n",
                              Sentence.GetOpt()->GetGroupNameByIndex(G.m_GroupType),
                              GetWords(Sentence, G).c_str());
            };
            Res += Format("\t</synvar>\n");
        }
    }


}

std::string GetNodeStr(const CSentence &Sentence, const CRelationsIterator &RelIt, int GroupNo, int WordNo) {
    if (GroupNo != -1)
        return GetWords(Sentence, RelIt.GetFirmGroups()[GroupNo]);
    else
        return Sentence.m_Words[WordNo].m_strWord;
}

std::string
GetNodeGrmStr(const CSentence &Sentence, const CRelationsIterator &RelIt, int GroupNo, int WordNo, std::string &Lemma) {
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

void GetRelations(const CSentence &Sentence, std::string &Result) {
    CRelationsIterator RelIt;
    RelIt.SetSentence(&Sentence);
    for (int i = 0; i < Sentence.m_vectorPrClauseNo.size(); i++)
        RelIt.AddClauseNoAndVariantNo(Sentence.m_vectorPrClauseNo[i], 0);
    RelIt.BuildRelations();
    for (long RelNo = 0; RelNo < RelIt.GetRelations().size(); RelNo++) {
        const CSynOutputRelation &piRel = RelIt.GetRelations()[RelNo];
        std::string RelName = Sentence.GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type);
        std::string Src = GetNodeStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord);
        std::string Trg = GetNodeStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord);
        std::string SrcLemma, TrgLemma;
        std::string SrcGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord, SrcLemma);
        std::string TrgGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord, TrgLemma);
        std::string GramRel = Sentence.GetOpt()->GetGramTab()->GrammemsToStr(piRel.m_Relation.m_iGrammems);

        Result += Format(
                "\t<rel name=\"%s\" gramrel=\"%s\" lemmprnt=\"%s\" grmprnt=\"%s\" lemmchld=\"%s\" grmchld=\"%s\" > %s -> %s </rel>\n",
                RelName.c_str(), GramRel.c_str(), SrcLemma.c_str(), SrcGrm.c_str(), TrgLemma.c_str(), TrgGrm.c_str(),
                Src.c_str(), Trg.c_str());
    }
}

std::string GetStringBySyntax(const CSentencesCollection &SC, const CAgramtab &A, std::string input) {
    std::string Result;
    Result += Format("<chunk>\n");
    Result += Format("<input>%s</input>\n", input.c_str());
    for (size_t nSent = 0; nSent < SC.m_vectorSents.size(); nSent++) {
        const CSentence &Sentence = *SC.m_vectorSents[nSent];
        int iWord = 0, iClau = 0, iCvar = 0;
        Result += "<sent>\n";
        GetAnanlytForms(Sentence, Result);
        GetGroups(Sentence, A, Result);
        GetRelations(Sentence, Result);
        Result += "</sent>\n";
    }
    Result += Format("</chunk>\n");

    std::cerr << "sentences count: " <<  SC.m_vectorSents.size() << "\n";
    return Result;
};


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output", "output file");
    parser.AddArgument("--language", "language");
    parser.AddOption("--speed-test");
    parser.AddOption("--input-is-list-file");
    parser.Parse(argc, argv);
}

void checkSpeed(ArgumentParser& args, CSyntaxHolder& H) {
    for (auto f : args.GetInputFiles()) {
        std::cerr << "File " <<  f << "\n";
        H.m_bTimeStatis = true;
        H.GetSentencesFromSynAn(f, true);
        std::string s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab, f.c_str());
        args.GetOutputStream() << s << "\n\n";
    }
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

    if (args.Exists("speed-test")) {
        checkSpeed(args, H);
        return 0;
    }

    try {
        std::string s;
        while (getline(args.GetInputStream(), s)) {
            Trim(s);
            if (s.empty()) continue;

            H.GetSentencesFromSynAn(s, false);
            s = GetStringBySyntax(H.m_Synan, *H.m_pGramTab, s);
            args.GetOutputStream()  << s;
        };
    }
    catch (...) {
        std::cerr << "an exception occurred!\n";
        return 1;
    };

    return 0;
};


