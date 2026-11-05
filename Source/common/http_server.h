#pragma once

#include "nlp_base/common/utilit.h"

#include <memory>
#include <mutex>
#include <string>

namespace httplib {
	class Server;
	struct Request;
	struct Response;
}

struct TDaemonParsedRequest {
	const httplib::Request& Request;
	std::string Action;
	MorphLanguageEnum Langua;
	std::string Query;

	bool HasParam(const char* name) const;
	// throws CExpc if the parameter is absent
	std::string GetParam(const char* name) const;
};

// A http server based on cpp-httplib. Connections are served by a thread pool,
// but OnParsedRequest calls are serialized (the RML holders are not thread-safe).
class TRMLHttpServer {
	std::unique_ptr<httplib::Server> Server;
	std::mutex RequestMutex;
	std::string Host;
	uint16_t Port = 0;
public:
	TRMLHttpServer();
	virtual ~TRMLHttpServer();
	void Initialize(std::string host, uint16_t port);
	void Start();
	void OnHttpRequest(const httplib::Request& req, httplib::Response& res);
	virtual std::string OnParsedRequest(TDaemonParsedRequest& request) = 0;
};


extern void DealWithLockFile(const std::string fileName);
extern std::string GetLogPath(std::string filename);
