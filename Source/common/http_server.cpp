#include "http_server.h"

#include "utilit.h"

#ifdef WIN32
#include <process.h>
#include <signal.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/signal.h>	
#endif

DaemonLogModeEnum TRMLHttpServer::LogMode;
std::string TRMLHttpServer::LogFileName;

#ifdef WIN32
void InitSocketsWindows() {

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		exit(1);
	}
}
#endif

bool InitSockets() {

#ifdef WIN32
	InitSocketsWindows();
#endif

	if (!event_init()) {
		std::cerr << "Failed to init libevent." << std::endl;
		return false;
	}

	return true;
}

DaemonLogModeEnum ParseDaemonLogMode(std::string strMode) {
	if (strMode == "quiet")
		return dlmQuiet;
	if (strMode == "normal")
		return dlmNormal;
	if (strMode == "debug")
		return dlmDebug;
	throw CExpc("bad daemon log mode, could be quiet, normal or debug");
}

int GetPID() {
#ifdef WIN32
	return _getpid();
#else
	return getpid();
#endif
}

bool CheckFileAppendRights(const char* fileName) {
	try {
		FILE *fp = fopen(fileName, "a");
		fclose(fp);
		return true;
	}
	catch (...) {
		return false;
	}
}

void TRMLHttpServer::LogMessage(const char* t) {
	try {
		struct tm today = RmlGetCurrentTime();
		char tmpbuf[255];
		FILE *fp = fopen(TRMLHttpServer::LogFileName.c_str(), "a");
		strftime(tmpbuf, 255, "%d%B%Y %H:%M:%S", &today);
		fprintf(fp, "%s > %s\n", tmpbuf, t);
		fclose(fp);
	}
	catch (...) {
	};
}

void  termination_handler(int signum) {
	TRMLHttpServer::LogMessage("termination_handler daemon");
	exit(1);
};


TRMLHttpServer::TRMLHttpServer() : Server(nullptr, nullptr) {
};

void TRMLHttpServer::Initialize(std::uint16_t srvPort, DaemonLogModeEnum logMode, const std::string logFile) {
	LogMode = logMode;
    SrvPort = srvPort;

	std::string logPath = GetRegistryString("Software\\Dialing\\Logs\\Main");
    std::string myIP = GetRegistryString("Software\\Dialing\\HttpServerIP");
	if (!DirExists(logPath.c_str())) {
		throw CExpc(Format("log dir \"%s\" does not exist; http-server must write logs to some folder\n", logPath.c_str()));
	};
	LogFileName = MakePath(logPath, logFile);
	if (!CheckFileAppendRights(LogFileName.c_str())) {
		throw CExpc(Format("Cannot write to log file \"%s\" \n", LogFileName.c_str()));
	}
	LogMessage(Format("initialize daemon at port %s:%i", myIP.c_str(), SrvPort).c_str());

	InitSockets();
	Server = TInnerServer(evhttp_start(myIP.c_str(), SrvPort), &evhttp_free);
	if (!Server) {
		throw CExpc("Failed to create http server.");
	}

	evhttp_set_default_content_type(Server.get(), "application/json; charset=utf8");
	if (signal(SIGTERM, termination_handler) == SIG_IGN)
		signal(SIGTERM, SIG_IGN);
};


static void OnHttpRequestStatic(evhttp_request *req, void* httpServer) {
	static_cast<TRMLHttpServer*>(httpServer)->OnHttpRequest(req);
}

void TRMLHttpServer::Start() {
	LogMessage("run message loop for daemon, start listen socket");
	std::cerr << "start listen socket at port " << SrvPort << "\n";
	evhttp_set_gencb(Server.get(), OnHttpRequestStatic, this);
	if (event_dispatch() == -1)	{
		throw CExpc ("Failed to run message loop.");
	}
}

void SendReply(evhttp_request *req, int status, struct evbuffer* response) {
	evhttp_add_header(evhttp_request_get_output_headers(req),
		"Access-Control-Allow-Origin", "*");
	evhttp_send_reply(req, status, "", response);
}

void TRMLHttpServer::OnHttpRequest(evhttp_request *req) {
	auto *outBuf = evhttp_request_get_output_buffer(req);
	if (!outBuf)
		return;

	const char* uri = evhttp_request_get_uri(req);
	if (LogMode != dlmQuiet) {
		LogMessage(uri);
	}
	struct evkeyvalq headers {}; // zero initialized with {}
	auto r = evhttp_parse_query(uri, &headers);
	try {
		auto action = evhttp_find_header(&headers, "action");
		if (action == nullptr) {
			throw CExpc("cannot find action");
		}

		auto languaStr = evhttp_find_header(&headers, "langua");
		if (languaStr == nullptr) {
			throw CExpc("cannot find language (langua argument)");
		}
		MorphLanguageEnum langua;
		if (!GetLanguageByString(languaStr, langua)) {
			throw CExpc(Format("unsupported language: %s", languaStr));
		}

		auto query = evhttp_find_header(&headers, "query");
		if (query == nullptr) {
			throw CExpc("cannot find query");
		}
		//std::string inputQuery = convert_from_utf8(query, langua);
        std::string inputQuery = query;
		Trim(inputQuery);
		if (inputQuery.empty()) {
			throw CExpc("Empty input query");
		};
		TDaemonParsedRequest parsedRequest{ req, uri, headers, action, langua, inputQuery};
		auto result = OnParsedRequest(parsedRequest);
		evbuffer_add_printf(outBuf, result.c_str());
		SendReply(req, HTTP_OK, outBuf);
	}
	catch (CExpc e) {
		std::string error = Format("Error: %s, Request: %s\n", e.m_strCause.c_str(), uri);
		TRMLHttpServer::LogMessage(error.c_str());
		SendReply(req, HTTP_BADREQUEST, nullptr);
		return;
	}
	catch (std::invalid_argument e) {
		std::string error = Format("Error: std::string to number conversion failed, Request: %s\n", uri);
		TRMLHttpServer::LogMessage(error.c_str());
		SendReply(req, HTTP_BADREQUEST, nullptr);
		return;
	}
	catch (std::exception e) {
		std::string error = Format("Error: %s, Request: %s\n", e.what(), uri);
		TRMLHttpServer::LogMessage(error.c_str());
		SendReply(req, HTTP_BADREQUEST, nullptr);
		return;
	}

};

void DealWithLockFile(const std::string fileName) {
	std::string LockFileName = MakePath(GetRmlVariable(), fileName);

	if (FileExists(LockFileName.c_str())) {
		std::cerr << "possible port conflicts..., removing " << LockFileName << "\n";
		remove(LockFileName.c_str());
	}
	FILE* fp = fopen(LockFileName.c_str(), "w");
	if (!fp) {
		throw CExpc(Format("Cannot create file %s", LockFileName.c_str()));
	};
	fprintf(fp, "%i", GetPID());
	fclose(fp);
}
