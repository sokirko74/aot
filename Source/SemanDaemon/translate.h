#pragma once

#include <SemanLib/SemStructureBuilder.h>

#include <common/http_server.h>

class TSemanHttpServer : public TRMLHttpServer {
	CSemStructureBuilder SemBuilder;
	std::string Translate(const std::string& russian, const std::string &po);
	std::string BuildRusGraph(const std::string& russian, const std::string &po);

public:
	TSemanHttpServer() : TRMLHttpServer() {};
	virtual ~TSemanHttpServer() {};
	std::string OnParsedRequest(TDaemonParsedRequest&) override;
	void Load();
};



extern bool LoadTranslator(); 
extern bool UnloadTranslator();
