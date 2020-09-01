#include "JVisualSynAnParamBuilder.h"
#include "SynanDmn.h"

#include "../SynanLib/SyntaxHolder.h"
#include "../LemmatizerLib/Morphan.h"
#include "../common/BigramsReader.h"
#include "../common/json.h"


CSyntaxHolder RussianSyntaxHolder;
CSyntaxHolder GermanSyntaxHolder;
CMorphologyHolder EnglishMorphHolder;


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

    return GetConnectedWords(request.Query, minBigramsFreq, directBigrams, sortMode, request.Langua);
}

const CMorphologyHolder *GetMorphHolder(MorphLanguageEnum l) {
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
    const CMorphologyHolder *Holder = GetMorphHolder(request.Langua);
    return LemmatizeJson(request.Query, Holder, withParadigms);
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
    if (!RussianSyntaxHolder.LoadSyntax(morphRussian)) {
        throw CExpc("cannot load Russian Syntax");
    };

    TRMLHttpServer::LogMessage("Loading German Syntax\n");
    if (!GermanSyntaxHolder.LoadSyntax(morphGerman)) {
        throw CExpc("cannot load German Syntax");
    };

    TRMLHttpServer::LogMessage("Loading English Morphology\n");
    if (!EnglishMorphHolder.LoadGraphanAndLemmatizer(morphEnglish)) {
        throw CExpc("cannot load English Morphology\n");
    };

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



