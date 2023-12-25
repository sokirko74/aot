#include "seman/SemanLib/VisualGraph.h"
#include "translate.h"

#include "morph_dict/common/json.h"
#include <assert.h>

void TSemanHttpServer::Load() {
	try {
		LOGI << "Init Preseman";
		SemBuilder.m_RusStr.m_pData->Init();

        LOGI << "Init Seman";
		SemBuilder.m_RusStr.m_pData->Initialize();

        LOGI << "InitializeIndices";
		SemBuilder.m_RusStr.m_pData->InitializeIndices();

	} 
	catch (...)
	{
        LOGE << Format("Error in Constructor: %s\n",(const char*)SemBuilder.m_RusStr.m_pData->m_LastError.c_str()).c_str();
		throw;
	}
}


std::string TSemanHttpServer::Translate(const std::string& russian, const std::string &po) {
	std::string eng = SemBuilder.TranslateRussianText(russian, po); 
	return Format("{\"translation\": \"%s\"}", eng.c_str());
}

std::string TSemanHttpServer::BuildRusGraph(const std::string& russian, const std::string &po)
{
	try {
		LOGI << "Build Graph: " << russian;
		
		std::string res;
        CSemOptions opts;
        opts.m_Domain = po;
        opts.m_PanicTreeVariantCount = 20000;
		SemBuilder.FindSituations(russian, opts);
		CVisualSemGraph graph;
		graph.InitFromSemantics(SemBuilder);
		graph.SetGraphLayout();
		return graph.GetResultStr();
	}
	catch (CExpc c) {
		throw;
	}
	catch (...) {
		throw CExpc("Error in Seman\n");
	}

};

std::string TSemanHttpServer::OnParsedRequest(TDaemonParsedRequest& req) {
	try {
		auto topic = evhttp_find_header(&req.headers, "topic");
		if (topic == nullptr) {
			throw CExpc("cannot find topic");
		}
		if (req.Query.length() > 150) {
			req.Query = req.Query.substr(0, 150) + " ...";
		}
		if (req.Action == "graph") {
			return BuildRusGraph(req.Query, topic);
		}
		else if (req.Action == "translate") {
			return Translate(req.Query, topic);
		}
		else {
			throw CExpc("unknown action");
		}
	}
	catch (nlohmann::json::exception& e) {
		throw CExpc(e.what());
	}

}

