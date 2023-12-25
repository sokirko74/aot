#include "common/http_server.h"
#include "synan/SynanLib/SyntaxHolder.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

class TSynanHttpServer : public TRMLHttpServer {
    CSyntaxHolder RussianSyntaxHolder;
    CSyntaxHolder GermanSyntaxHolder;
    CMorphanHolder EnglishMorphHolder;

    std::string ProcessMorphology(TDaemonParsedRequest& request);
    std::string ProcessBigrams(TDaemonParsedRequest& request);
    std::string ProcessSyntax(TDaemonParsedRequest& request);


public:
	TSynanHttpServer();
	virtual ~TSynanHttpServer() {};
	std::string OnParsedRequest(TDaemonParsedRequest&) override;
	void LoadSynan(bool loadBigrams);
};

