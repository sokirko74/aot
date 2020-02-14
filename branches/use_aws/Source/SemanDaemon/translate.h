#pragma once

#include <SemanLib/SemStructureBuilder.h>

#include <common/http_server.h>

class TSemanHttpServer : public TRMLHttpServer {
	CSemStructureBuilder SemBuilder;
	string Translate(const string& russian, const string &po);
	string BuildRusGraph(const string& russian, const string &po);

public:
	TSemanHttpServer() : TRMLHttpServer() {};
	virtual ~TSemanHttpServer() {};
	string OnParsedRequest(TDaemonParsedRequest&) override;
	void Load();
};



extern bool LoadTranslator(); 
extern bool UnloadTranslator();
