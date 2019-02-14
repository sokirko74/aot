#include "VisualGraph.h"
#include "translate.h"

#include <common/json.h>
#include <assert.h>

void TSemanHttpServer::Load() {
	try {
		LogMessage("Init Preseman\n");
		SemBuilder.m_RusStr.m_pData->Init();

		LogMessage("Init Seman\n");
		SemBuilder.m_RusStr.m_pData->Initialize();

		LogMessage("InitializeIndices\n");
		SemBuilder.m_RusStr.m_pData->InitializeIndices();

	} 
	catch (...)
	{
		LogMessage(Format("Error in Constructor: %s\n",(const char*)SemBuilder.m_RusStr.m_pData->m_LastError.c_str()));
		throw;
	}
}


string TSemanHttpServer::Translate(const string& russian, const string &po) {
	string graphStr;
	try {
		LogMessage(Format("  FindSituations (po=%s) %s\n", po.c_str(), russian.c_str()));
		SemBuilder.FindSituations(russian.c_str(), 0, po.c_str(), 20000, -1, "", graphStr);
	}
	catch (CExpc c) {
		throw;
	}
	catch (...)
	{
		string s = SemBuilder.m_RusStr.m_pData->m_LastError;
		if (s.empty())
			s = "unknown error";
		throw CExpc(s);
	}

	try {
		string res;
		for(;;) {
			LogMessage("  TranslateToEnglish\n");
			if (!SemBuilder.TranslateToEnglish(graphStr)) {
				LogMessage("Error in TranslateToEnglish\n");
				return "Unexpected Error";
			}

			LogMessage("  BuildSentence\n");
			string TranslatedSent;
			if (!SemBuilder.BuildSentence(TranslatedSent))
			{
				LogMessage("Error in Synthesis\n");
				return "Unexpected Error";
			}
			res += TranslatedSent;
			
			LogMessage("  FindSituationsForNextSentence\n");
			if (!SemBuilder.FindSituationsForNextSentence()) break;
		}

		return Format("{\"translation\": \"%s\"}", res.c_str());
	} 
	catch (CExpc c) {
		throw;
	}
	catch (...)
	{
		throw CExpc("Error in TranslateToEnglish or Synthesis");
	}
}

string TSemanHttpServer::BuildRusGraph(const string& russian, const string &po)
{
	try {
		LogMessage("Build Graph: " + russian);
		
		string res;
		string graphStr;
		if (!SemBuilder.FindSituations(russian.c_str(), 0, po.c_str(), 20000, -1, "", graphStr)) {
			throw CExpc("Error in SemBuilder\n");
		}
		CVisualSemGraph Graph;
		Graph.InitFromSemantics(SemBuilder);
		Graph.SetGraphLayout();
		return Graph.GetResultStr();
	}
	catch (CExpc c) {
		throw;
	}
	catch (...) {
		throw CExpc("Error in Seman\n");
	}

};

string TSemanHttpServer::OnParsedRequest(TDaemonParsedRequest& req) {
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

