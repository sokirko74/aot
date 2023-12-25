#include "JVisualSynAnParamBuilder.h"
#include "SynanDmn.h"

#include "common/BigramsReader.h"
#include "morph_dict/common/json.h"

TSynanHttpServer::TSynanHttpServer() :
    TRMLHttpServer(),
    RussianSyntaxHolder(morphRussian),
    GermanSyntaxHolder(morphGerman)
{

}


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


std::string TSynanHttpServer::ProcessMorphology(TDaemonParsedRequest &request) {
    bool withParadigms = evhttp_find_header(&request.headers, "withparadigms") != nullptr;
    const CMorphanHolder &h  = GetMHolder(request.Langua);
    std::string wordForm = request.Query;
    return h.LemmatizeJson(wordForm, withParadigms);
};


std::string TSynanHttpServer::ProcessSyntax(TDaemonParsedRequest &request) {
    if (request.Langua == morphEnglish) {
        throw CExpc("unsupported language");
    }
    CSyntaxHolder *P = (request.Langua == morphRussian) ? &RussianSyntaxHolder : &GermanSyntaxHolder;
    return BuildJson(P, request.Query);
};


void TSynanHttpServer::LoadSynan(bool loadBigrams) {
    LOGI <<"Loading Russian Syntax";
    RussianSyntaxHolder.LoadSyntax();

    LOGI <<"Loading German Syntax";
    GermanSyntaxHolder.LoadSyntax();

    LOGI <<"Loading English Morphology";
    EnglishMorphHolder.LoadMorphology(morphEnglish);

    if (loadBigrams) {
        auto path = fs::path(GetRmlVariable()) / "Dicts" / "Bigrams";
        if (!fs::exists(path))
            throw CExpc(Format("cannot find bigrams directory: %s", path.string().c_str()));
        InitializeBigrams(path.string())
    }
};

std::string TSynanHttpServer::OnParsedRequest(TDaemonParsedRequest &req) {
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



