#include <contrib/libevent/include/evhttp.h>
//#include <contrib/libevent/include/event2/http.h>
#include "utilit.h"

typedef void (*TLogFunction)(const string&);

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
	std::unique_ptr<evhttp, decltype(&evhttp_free)> Server;
protected:
	TLogFunction LogFunction;
	DaemonLogModeEnum LogMode;
public:
	TRMLHttpServer(std::uint16_t srvPort, TLogFunction logFunction, DaemonLogModeEnum dlm);
	bool Start();
	void OnHttpRequest(evhttp_request *req);
	virtual string OnParsedRequest(TDaemonParsedRequest& request) = 0;
};


bool InitSockets();
DaemonLogModeEnum ParseDaemonLogMode(string strMode);
int GetPID();
void SetSigTermHandler(void(*termination_handler)(int));
