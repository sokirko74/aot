#include "common/http_server.h"

class TSynanHttpServer : public TRMLHttpServer {
	std::string ProcessMorphology(TDaemonParsedRequest& request);
	std::string ProcessBigrams(TDaemonParsedRequest& request);
	std::string ProcessSyntax(TDaemonParsedRequest& request);
public:
	TSynanHttpServer() : TRMLHttpServer() {};
	virtual ~TSynanHttpServer() {};
	std::string OnParsedRequest(TDaemonParsedRequest&) override;
	static void LoadSynan(bool loadBigrams);
};

