#include "http_server.h"

#ifdef WIN32
#include <process.h>
#include <signal.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/signal.h>	
#endif

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

    string log_path = GetRegistryString("Software\\Dialing\\Logs\\Main");
    if (!DirExists(log_path.c_str())) {
        fprintf (stderr, "log dir \"%s\" does not exist; http-server must write logs to some folder\n");
        exit(1);
    }

	return true;
}

DaemonLogModeEnum ParseDaemonLogMode(string strMode) {
	if (strMode == "quiet")
		return dlmQuiet;
	if (strMode == "normal")
		return dlmNormal;
	if (strMode == "debug")
		return dlmDebug;
	throw CExpc("bad daemon log mode, could be quiet, normal or debug");
}


TRMLHttpServer::TRMLHttpServer(std::uint16_t srvPort, TLogFunction logFunction, DaemonLogModeEnum logMode):
	Server(evhttp_start("127.0.0.1", srvPort), &evhttp_free),
    LogFunction (logFunction),
	LogMode(logMode){
	evhttp_set_default_content_type(Server.get(), "application/json; charset=utf8");
};


static void OnHttpRequestStatic(evhttp_request *req, void* httpServer) {
	static_cast<TRMLHttpServer*>(httpServer)->OnHttpRequest(req);
}

bool TRMLHttpServer::Start() {
	if (!Server) {
		std::cerr << "Failed to init http server." << std::endl;
		return false;
	}
	evhttp_set_gencb(Server.get(), OnHttpRequestStatic, this);
	if (event_dispatch() == -1)
	{
		std::cerr << "Failed to run messahe loop." << std::endl;
		return false;
	}
	return true;
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
		LogFunction(uri);
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
		std::string inputQuery = convert_from_utf(query, langua);
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
		string error = Format("Error: %s, Request: %s\n", e.m_strCause.c_str(), uri);
		LogFunction(error.c_str());
		SendReply(req, HTTP_BADREQUEST, nullptr);
		return;
	}
	catch (std::invalid_argument e) {
		string error = Format("Error: string to number conversion failed, Request: %s\n", uri);
		LogFunction(error.c_str());
		SendReply(req, HTTP_BADREQUEST, nullptr);
		return;
	}
	
	//print_method(req);
};

int GetPID() {
    #ifdef WIN32
	   return _getpid();
    #else
	   return getpid();
    #endif
}

void SetSigTermHandler(void(*termination_handler)(int)) {
	if (signal(SIGTERM, termination_handler) == SIG_IGN)
		signal(SIGTERM, SIG_IGN);
}
