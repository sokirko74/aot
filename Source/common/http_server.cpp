#include "http_server.h"

#include "nlp_base/common/utilit.h"
#include "nlp_base/contrib/httplib/httplib.h"

#include <csignal>

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif


static int GetPID() {
#ifdef WIN32
	return _getpid();
#else
	return getpid();
#endif
}

static void termination_handler(int signum) {
	LOGI << "termination_handler daemon";
	exit(1);
};


bool TDaemonParsedRequest::HasParam(const char* name) const {
	return Request.has_param(name);
}

std::string TDaemonParsedRequest::GetParam(const char* name) const {
	if (!Request.has_param(name)) {
		throw CExpc(Format("cannot find %s", name));
	}
	return Request.get_param_value(name);
}


TRMLHttpServer::TRMLHttpServer() : Server(std::make_unique<httplib::Server>()) {
};

TRMLHttpServer::~TRMLHttpServer() = default;

void TRMLHttpServer::Initialize(std::string host, uint16_t port) {
	LOGI << "initialize daemon at " << host << ":" << port;
	Host = host;
	Port = port;
	auto handler = [this](const httplib::Request& req, httplib::Response& res) {
		OnHttpRequest(req, res);
	};
	Server->Get("/.*", handler);
	Server->Post("/.*", handler);
	if (!Server->bind_to_port(Host, Port)) {
		throw CExpc(Format("Failed to bind http server to %s:%i", Host.c_str(), Port));
	}
	if (signal(SIGTERM, termination_handler) == SIG_IGN)
		signal(SIGTERM, SIG_IGN);
};


void TRMLHttpServer::OnHttpRequest(const httplib::Request& req, httplib::Response& res) {
	res.set_header("Access-Control-Allow-Origin", "*");
	LOGI << req.target;
	try {
		if (!req.has_param("action")) {
			throw CExpc("cannot find action");
		}
		if (!req.has_param("langua")) {
			throw CExpc("cannot find language (langua argument)");
		}
		std::string languaStr = req.get_param_value("langua");
		MorphLanguageEnum langua;
		if (!GetLanguageByString(languaStr, langua)) {
			throw CExpc(Format("unsupported language: %s", languaStr.c_str()));
		}
		if (!req.has_param("query")) {
			throw CExpc("cannot find query");
		}
		std::string inputQuery = req.get_param_value("query");
		Trim(inputQuery);
		if (inputQuery.empty()) {
			throw CExpc("Empty input query");
		};
		TDaemonParsedRequest parsedRequest{ req, req.get_param_value("action"), langua, inputQuery };
		std::string result;
		{
			std::lock_guard<std::mutex> lock(RequestMutex);
			result = OnParsedRequest(parsedRequest);
		}
		res.set_content(result, "application/json; charset=utf8");
	}
	catch (std::exception& e) {
		LOGE << "Error: " << e.what() << " Request: " << req.target;
		res.status = httplib::StatusCode::BadRequest_400;
	}
}


void TRMLHttpServer::Start() {
	LOGI << "run message loop for daemon, start listen socket";
	if (!Server->listen_after_bind()) {
		throw CExpc("Failed to run http server.");
	}
}

void DealWithLockFile(const std::string fileName) {
	std::string LockFileName = MakePath(GetRmlVariable(), fileName);

	if (FileExists(LockFileName.c_str())) {
		LOGI << "possible port conflicts..., removing " << LockFileName;
		remove(LockFileName.c_str());
	}
	FILE* fp = fopen(LockFileName.c_str(), "w");
	if (!fp) {
		throw CExpc(Format("Cannot create file %s", LockFileName.c_str()));
	};
	fprintf(fp, "%i", GetPID());
	fclose(fp);
}

std::string GetLogPath(std::string filename) {
    std::string logPath = GetRegistryString("Software\\Dialing\\Logs\\Main");

    if (!fs::exists(logPath.c_str())) {
        throw CExpc(Format("log directory \"%s\" does not exist, please create it", logPath.c_str()));
    };
    auto path = fs::path(logPath) / filename;
    try {
        FILE *fp = fopen(path.c_str(), "a");
        fclose(fp);
    }
    catch (...) {
        throw CExpc("cannot append to log file %s", path.c_str());
    }
    return path.string();
}