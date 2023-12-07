#include <stdio.h>
#include "synan/SynCommonLib/RelationsIterator.h"
#include "../SynanLib/SyntaxHolder.h"
#include "morph_dict/common/util_classes.h"
#include "morph_dict/common/argparse.h"
#include <filesystem>

void GetAnanlytForms(const CSentence &Sentence, CJsonObject& out) {
    CJsonObject arr(out.get_doc(), rapidjson::kArrayType);
    for (auto& w: Sentence.m_Words) {
        if (!w.m_MainVerbs.empty()) {
            std::string form = w.m_strWord;
            for (auto& m: w.m_MainVerbs) {
                form += std::string(" ") + Sentence.m_Words[m].m_strWord;
                for (auto j: Sentence.m_Words[m].m_MainVerbs)
                    form += std::string(" ") + Sentence.m_Words[j].m_strWord;
            };
            rapidjson::Value s;
            s.SetString(form.c_str(), out.get_allocator());
            arr.push_back(s);
        }
    }
    out.add_member("analytical", arr.get_value());
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

void GetGroups(const CSentence &Sentence, const CAgramtab &A, CJsonObject& out) {
    int nClausesCount = Sentence.GetClausesCount();
    CJsonObject arr(out.get_doc(), rapidjson::kArrayType);

    for (int ClauseNo = 0; ClauseNo < nClausesCount; ClauseNo++) {
        const CClause &Clause = Sentence.GetClause(ClauseNo);
        int nCvar = Clause.m_SynVariants.size();

        if (Clause.m_SynVariants.empty()) continue;
        CJsonObject clause(out.get_doc());
        clause.add_int("start", Clause.m_iFirstWord);
        clause.add_int("last",  Clause.m_iLastWord);
        clause.add_string_copy("words", GetWords(Sentence, Clause));
        if (!Clause.m_RelativeWord.IsEmpty())
        {
            clause.add_string_copy("relative", Sentence.m_Words[Clause.m_RelativeWord.m_WordNo].m_strWord);
        };
        if (Clause.m_AntecedentWordNo != -1)
        {
            clause.add_string_copy("antecedent",  Sentence.m_Words[Clause.m_AntecedentWordNo].m_strWord);
        };

        

        int nVmax = Clause.m_SynVariants.begin()->m_iWeight;
        CJsonObject good_synvars(out.get_doc(), rapidjson::kArrayType);
        for (auto& synVar: Clause.m_SynVariants) {
            if (synVar.m_iWeight < nVmax) break;

            std::string clauseType = "EMPTY";
            {
                int ClauseType = (synVar.m_ClauseTypeNo == -1) ? UnknownSyntaxElement
                                                          : Clause.m_vectorTypes[synVar.m_ClauseTypeNo].m_Type;;
                if (ClauseType != UnknownSyntaxElement)
                    clauseType = A.GetClauseNameByType(ClauseType);
            }
            CJsonObject groups(out.get_doc(), rapidjson::kArrayType);
            for (auto& g: synVar.m_vectorGroups.GetGroups()) {
                CJsonObject jq(out.get_doc());
                jq.add_string_copy("type", Sentence.GetOpt()->GetGroupNameByIndex(g.m_GroupType));
                jq.add_string_copy("words", GetWords(Sentence, g));
                groups.push_back(jq.get_value());
            };           

            CJsonObject syn_units(out.get_doc(), rapidjson::kArrayType);
            for (int unitNo = 0; unitNo < synVar.m_SynUnits.size(); unitNo++) {
                int iWord = synVar.m_SynUnits[unitNo].m_SentPeriod.m_iFirstWord;
                int homIndex = synVar.GetHomNum(unitNo);
                if (homIndex != -1) {
                    const CSynHomonym& hom = Sentence.GetWords()[iWord].GetSynHomonym(homIndex);
                    CJsonObject o(out.get_doc());
                    o.add_string_copy("lemma", hom.GetLemma());
                    o.add_string_copy("morph_info", hom.GetPartOfSpeechStr() + std::string(" ") + hom.GetGrammemsStr());
                    o.add_string_copy("modified_grammems", A.GrammemsToStr(synVar.m_SynUnits[unitNo].m_iGrammems));
                    syn_units.push_back(o.get_value());
                }
                else {
                    //todo: print subclauses
                }
            }
            CJsonObject o(out.get_doc());
            o.add_member("syn_units", syn_units.get_value());
            o.add_member("groups", groups.get_value());
            o.add_string_copy("clause_type", clauseType);
            good_synvars.push_back(o.get_value());
        }
        clause.add_member("good_synvars", good_synvars.get_value());
        arr.push_back(clause);
    }
    out.add_member("groups", arr.get_value());
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
        Lemma = Sentence.GetWords()[WordNo].GetHomonym(U.m_iHomonymNum)->GetLemma();
        return Sentence.GetOpt()->GetGramTab()->GrammemsToStr(U.m_iGrammems | U.m_TypeGrammems);
    }
}

void GetRelations(const CSentence &Sentence, CJsonObject& out) {
    CRelationsIterator RelIt;
    RelIt.SetSentence(&Sentence);
    for (auto& i: Sentence.m_vectorPrClauseNo)
        RelIt.AddClauseNoAndVariantNo(i, 0);
    RelIt.BuildRelations();
    CJsonObject rels(out.get_doc(), rapidjson::kArrayType);

    for (auto& piRel: RelIt.GetRelations()) {
        CJsonObject o(out.get_doc());
        o.add_string_copy("src", GetNodeStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord));
        o.add_string_copy("trg", GetNodeStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord));
        o.add_string_copy("name", Sentence.GetOpt()->GetGroupNameByIndex(piRel.m_Relation.type));
        o.add_string_copy("gramrel", Sentence.GetOpt()->GetGramTab()->GrammemsToStr(piRel.m_Relation.m_iGrammems));
        std::string SrcLemma, TrgLemma;
        std::string SrcGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iSourceGroup, piRel.m_Relation.m_iFirstWord, SrcLemma);
        o.add_string_copy("src_lemma", SrcLemma);
        o.add_string_copy("src_grm", SrcGrm);

        std::string TrgGrm = GetNodeGrmStr(Sentence, RelIt, piRel.m_iTargetGroup, piRel.m_Relation.m_iLastWord, TrgLemma);
        o.add_string_copy("trg_lemma", TrgLemma);
        o.add_string_copy("trg_grm", TrgGrm);

        rels.push_back(o);
    }
    out.add_member("relations", rels.get_value());
}

void GetThesaurusTerms(const CSentence& Sentence, CJsonObject& out) {
    CJsonObject terms(out.get_doc(), rapidjson::kArrayType);
    for (size_t i = 0; i < Sentence.m_Words.size(); i++) {
        if (Sentence.m_Words[i].m_bFirstWordInTermin) {
            for (size_t k = i; k < Sentence.m_Words.size(); ++k) {
                if (Sentence.m_Words[k].m_bLastWordInTermin) {
                    rapidjson::Value o;
                    o.SetString(GetWords(Sentence, CPeriod(i, k)), out.get_allocator());
                    terms.push_back(o);
                    break;
                }
            }
            
        }
    }
    out.add_member("terms", terms.get_value());
}

void GetResultBySyntax(const CSentencesCollection& SC, CJsonObject& out) {
    CJsonObject sents(out.get_doc(), rapidjson::kArrayType);
    const CAgramtab& A = *SC.GetOpt()->GetGramTab();
    for (size_t nSent = 0; nSent < SC.m_vectorSents.size(); nSent++) {
        const CSentence &Sentence = *SC.m_vectorSents[nSent];
        CJsonObject sent(out.get_doc());
        GetAnanlytForms(Sentence, sent);
        GetGroups(Sentence, A, sent);
        GetRelations(Sentence, sent);
        GetThesaurusTerms(Sentence, sent);
        sents.push_back(sent.get_value());
    }
    out.add_member("result",sents.get_value());
};


void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input-file", "input file", true);
    parser.AddArgument("--output-file", "output file", true);
    parser.AddArgument("--input-file-mask", "c:/*.txt", true);
    parser.AddArgument("--output-folder", "", true);
    parser.AddArgument("--language", "language");
    parser.AddArgument("--log-level", "log level", true);

    parser.Parse(argc, argv);
}


int main(int argc, const char** argv) {
    ArgumentParser args;
    initArgParser(argc, argv, args);
    auto langua = args.GetLanguage();
    init_plog(args.GetLogLevel(), "synan_test.log", true);
    GlobalLoadMorphHolder(langua);
    CSyntaxHolder H(langua);
    try {
        H.LoadSyntax();

        std::cerr << "ok\n";
        std::vector <std::pair<std::string, std::string> > file_pairs;

        if (args.Exists("input-file-mask")) {
            auto file_names = list_path_by_file_mask(args.Retrieve("input-file-mask"));
            for (auto filename : file_names) {
                auto outputFilename = filename + ".synan";
                if (args.Exists("output-folder")) {
                    auto base_name = std::filesystem::path(outputFilename).filename();
                    auto p = std::filesystem::path(args.Retrieve("output-folder")) / base_name;
                    //PLOGD <<  "set output file " << p;
                    outputFilename = p.string();
                }
                file_pairs.push_back({filename, outputFilename });
            }
        }
        else {
            file_pairs.push_back({ args.Retrieve("input-file"), args.Retrieve("output-file")});
        }
        for (auto& p : file_pairs) {
            std::cerr << p.first << "\n";
            CTestCaseBase base;
            std::ifstream inp(p.first);
            base.read_test_cases(inp);
            rapidjson::Document d;
            CJsonObject cases(d, rapidjson::kArrayType);

            for (auto& t : base.TestCases) {
                if (!t.Text.empty()) {
                    H.GetSentencesFromSynAn(t.Text, false);
                    CJsonObject j(d);
                    j.add_string("input",  t.Text);
                    j.add_string("comments", t.Comment);
                    GetResultBySyntax(H.m_Synan, j);
                    cases.push_back(j.get_value());
                }
            }
            PLOGD <<  "write output file " << p.second;
            cases.dump_rapidjson_pretty(p.second, 4);
        }
    }
    catch (std::exception& e) {
        PLOGE << e.what();
        return 1;
    }
    catch (...) {
        PLOGE << "an exception occurred!";
        return 1;
    };

    return 0;
};


