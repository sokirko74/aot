#include "../common/http_server.h"

class TSynanHttpServer : public TRMLHttpServer {
	string ProcessMorphology(TDaemonParsedRequest& request);
	string ProcessBigrams(TDaemonParsedRequest& request);
	string ProcessSyntax(TDaemonParsedRequest& request);
public:
	TSynanHttpServer(std::uint16_t srvPort, TLogFunction logFunction, DaemonLogModeEnum logMode) :
		TRMLHttpServer(srvPort, logFunction, logMode) {};
	string OnParsedRequest(TDaemonParsedRequest&) override;
};

bool LoadSynan();
bool UnloadSynan();
void synan_daemon_log(const string&);
