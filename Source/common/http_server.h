#include "utilit.h"

#include <evhttp.h>

#include <memory>

typedef void (*TLogFunction)(const char*);

enum DaemonLogModeEnum {
	dlmQuiet = 0,
	dlmNormal = 1,
	dlmDebug = 2,
};

struct TDaemonParsedRequest {
	evhttp_request *Request;
	const char* Uri;
	evkeyvalq& headers;
	std::string Action;
	MorphLanguageEnum Langua;
	std::string Query;
};

class TRMLHttpServer {
	using TInnerServer = std::unique_ptr<evhttp, decltype(&evhttp_free)>;
	TInnerServer Server;
	static DaemonLogModeEnum LogMode;
	static std::string LogFileName;
    int SrvPort;
public:
	TRMLHttpServer();
	virtual ~TRMLHttpServer() {};
	void Initialize(std::uint16_t srvPort, DaemonLogModeEnum logMode, const std::string logFile);
	void Start();
	void OnHttpRequest(evhttp_request *req);
	virtual std::string OnParsedRequest(TDaemonParsedRequest& request) = 0;

	static void LogMessage(const char* t);
};


DaemonLogModeEnum ParseDaemonLogMode(std::string strMode);
void DealWithLockFile(const std::string fileName);
