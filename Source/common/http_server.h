#include <evhttp.h>
#include "morph_dict/common/utilit.h"
#include <memory>


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
public:
	TRMLHttpServer();
	virtual ~TRMLHttpServer() {};
	void Initialize(std::string host, uint16_t port);
	void Start();
	void OnHttpRequest(evhttp_request *req);
	virtual std::string OnParsedRequest(TDaemonParsedRequest& request) = 0;
};


extern void DealWithLockFile(const std::string fileName);
extern std::string GetLogPath(std::string filename);