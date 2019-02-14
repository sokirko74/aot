#include <common/http_server.h>

class TSynanHttpServer : public TRMLHttpServer {
	string ProcessMorphology(TDaemonParsedRequest& request);
	string ProcessBigrams(TDaemonParsedRequest& request);
	string ProcessSyntax(TDaemonParsedRequest& request);
public:
	TSynanHttpServer() : TRMLHttpServer() {};
	virtual ~TSynanHttpServer() {};
	string OnParsedRequest(TDaemonParsedRequest&) override;
	static void LoadSynan(bool loadBigrams);
};

