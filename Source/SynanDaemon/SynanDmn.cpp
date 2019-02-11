#include "JVisualSynAnParamBuilder.h"
#include "SynanDmn.h"

#include "../SynanLib/SyntaxHolder.h"
#include "../LemmatizerLib/Morphan.h"
//#include <thread>
#include "../common/string_socket.h"
#include <common/BigramsReader.h>
#include <common/json.h>


CSyntaxHolder RussianSyntaxHolder;
CSyntaxHolder GermanSyntaxHolder;
CMorphologyHolder EnglishMorphHolder;


void synan_daemon_log(const string &t) {

    try {

        string log_path = GetRegistryString("Software\\Dialing\\Logs\\Main");
        struct tm today = RmlGetCurrentTime();
        char tmpbuf[255];
        string FileName = log_path + "synan_dmn.log";

        FILE *fp = fopen(FileName.c_str(), "a");
        strftime(tmpbuf, 255, "%d%B%Y %H:%M:%S", &today);
        fprintf(fp, "%s > %s\n", tmpbuf, t.c_str());
        fclose(fp);

    }
    catch (...) {
    };
}

string TSynanHttpServer::ProcessBigrams(TDaemonParsedRequest& request) {
	auto str = evhttp_find_header(&request.headers, "minBigramsFreq");
	if (!str) {
		throw CExpc("minBigramsFreq is not specified");
	}
	int minBigramsFreq = std::stoi(str);
	bool directBigrams = evhttp_find_header(&request.headers, "direct") != nullptr;
	auto sortMode = evhttp_find_header(&request.headers, "sortMode");
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

string TSynanHttpServer::ProcessMorphology(TDaemonParsedRequest& request) {
	bool withParadigms = evhttp_find_header(&request.headers, "withparadigms") != nullptr;
    const CMorphologyHolder *Holder = GetMorphHolder(request.Langua);
    return LemmatizeJson(request.Query, Holder, withParadigms);
};


string TSynanHttpServer::ProcessSyntax(TDaemonParsedRequest& request) {
	if (request.Langua == morphEnglish) {
		throw CExpc("unsupported language");
	}
    CSyntaxHolder *P = (request.Langua == morphRussian) ? &RussianSyntaxHolder : &GermanSyntaxHolder;
	return BuildJson(P, request.Query);
};


extern void (*GlobalErrorMessage)(const string &);

bool LoadSynan() {
	GlobalErrorMessage = synan_daemon_log;
    synan_daemon_log("Loading Russian Syntax\n");
    if (!RussianSyntaxHolder.LoadSyntax(morphRussian)) {
        synan_daemon_log("  cannot load Russian Syntax\n");
        return false;
    };

    try {
        synan_daemon_log("Loading German Syntax\n");
        if (!GermanSyntaxHolder.LoadSyntax(morphGerman)) {
            synan_daemon_log("  cannot load German Syntax\n");
            return false;
        };
    }
    catch (...) {
        synan_daemon_log("  an exception occurred!\n");
        return false;
    };

    try {
        synan_daemon_log("Loading English Morphology\n");
        if (!EnglishMorphHolder.LoadGraphanAndLemmatizer(morphEnglish)) {
            synan_daemon_log("  cannot load English Morphology\n");
            return false;
        };
    }
    catch (...) {
        synan_daemon_log("  an exception occurred!\n");
        return false;
    };

    try {
        string FileName = GetRmlVariable() + "/Dicts/Bigrams/bigrams.txt";
        if (access(FileName.c_str(), 04) == 0)
            if (!InitializeBigrams(FileName))
                return false;
    }
    catch (CExpc c) {
        synan_daemon_log(c.m_strCause);
        return false;
    }
    catch (...) {
        synan_daemon_log("  an exception occurred!\n");
        return false;
    };


    return true;

};

bool UnloadSynan() {
    synan_daemon_log("Unloading syntax...\n");
    return true;
};

string TSynanHttpServer::OnParsedRequest(TDaemonParsedRequest& req) {
	try {
		if (req.Action == "morph") {
			return ProcessMorphology(req);
		}
		else if (req.Action == "bigrams") {
			return ProcessBigrams(req);
		}
		else if (req.Action == "syntax") {
			return ProcessSyntax(req);
		}
		else {
			throw CExpc("unknown action");
		}
	}
	catch (nlohmann::json::exception& e)	{
		throw CExpc(e.what());
	}
	
}



