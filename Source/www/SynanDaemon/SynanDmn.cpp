#include "JVisualSynAnParamBuilder.h"
#include "SynanDmn.h"

#include "synan/SynanLib/SyntaxHolder.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include "common/BigramsReader.h"
#include "morph_dict/common/json.h"


CSyntaxHolder RussianSyntaxHolder;
CSyntaxHolder GermanSyntaxHolder;
CMorphanHolder EnglishMorphHolder;


std::string TSynanHttpServer::ProcessBigrams(TDaemonParsedRequest &request) {
    auto str = evhttp_find_header(&request.headers, "minBigramsFreq");
    if (!str) {
        throw CExpc("minBigramsFreq is not specified");
    }
    int minBigramsFreq = std::stoi(str);
    bool directBigrams = evhttp_find_header(&request.headers, "direct") != nullptr;
    auto sortMode = evhttp_find_header(&request.headers, "sortMode");
    if (!sortMode) {
        throw CExpc("sortMode is not specified");
    }
    std::string wordForm = request.Query;
    wordForm = convert_from_utf8(wordForm.c_str(), request.Langua);

    return GetConnectedWords(wordForm, minBigramsFreq, directBigrams, sortMode, request.Langua);
}

const CMorphanHolder *GetMorphHolder(MorphLanguageEnum l) {
    switch (l) {
        case morphRussian:
            return &RussianSyntaxHolder;
        case morphGerman:
            return &GermanSyntaxHolder;
        case morphEnglish:
            return &EnglishMorphHolder;
        default:
            return 0;
    }
}

std::string TSynanHttpServer::ProcessMorphology(TDaemonParsedRequest &request) {
    bool withParadigms = evhttp_find_header(&request.headers, "withparadigms") != nullptr;
    const CMorphanHolder *Holder = GetMorphHolder(request.Langua);
    std::string wordForm = request.Query;
    return Holder->LemmatizeJson(wordForm, withParadigms);
};


std::string TSynanHttpServer::ProcessSyntax(TDaemonParsedRequest &request) {
    if (request.Langua == morphEnglish) {
        throw CExpc("unsupported language");
    }
    CSyntaxHolder *P = (request.Langua == morphRussian) ? &RussianSyntaxHolder : &GermanSyntaxHolder;
    return BuildJson(P, request.Query);
};


void TSynanHttpServer::LoadSynan(bool loadBigrams) {
    TRMLHttpServer::LogMessage("Loading Russian Syntax\n");
    RussianSyntaxHolder.LoadSyntax(morphRussian);

    TRMLHttpServer::LogMessage("Loading German Syntax\n");
    GermanSyntaxHolder.LoadSyntax(morphGerman);

    TRMLHttpServer::LogMessage("Loading English Morphology\n");
    EnglishMorphHolder.LoadLemmatizer(morphEnglish);

    if (loadBigrams) {
        std::string fileName = GetRmlVariable() + "/Dicts/Bigrams/bigrams.txt";
        if (!FileExists(fileName.c_str()))
            throw CExpc(Format("cannot find bigrams file: %s", fileName));
        if (!InitializeBigrams(fileName))
            throw CExpc(Format("cannot init bigrams"));
    }
};

std::string TSynanHttpServer::OnParsedRequest(TDaemonParsedRequest &req) {
    try {
        if (req.Action == "morph") {
            return ProcessMorphology(req);
        } else if (req.Action == "bigrams") {
            return ProcessBigrams(req);
        } else if (req.Action == "syntax") {
            return ProcessSyntax(req);
        } else {
            throw CExpc("unknown action");
        }
    }
    catch (nlohmann::json::exception &e) {
        throw CExpc(e.what());
    }

}



