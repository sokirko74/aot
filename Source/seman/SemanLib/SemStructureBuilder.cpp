#include "stdafx.h"
#include "SemStructureBuilder.h"

CSemStructureBuilder::CSemStructureBuilder() : m_RusStr(), m_EngStr(m_RusStr) 
{
	m_RusStr.m_pData = new CSemanticsHolder();
	m_EngStr.m_pData = m_RusStr.m_pData;  
	m_CurrentSentence = 0;
	m_bShouldBuildTclGraph = true;
}

CSemStructureBuilder::~CSemStructureBuilder()
{
	delete m_RusStr.m_pData;
};

void CSemStructureBuilder::InitDicts() {
	PLOGD << "initialize presemantic dictionaries...";
	m_RusStr.m_pData->Init();
	m_RusStr.m_pData->Initialize();
	m_RusStr.m_pData->InitializeIndices();
	m_bShouldBuildTclGraph = false;
	m_RusStr.m_pData->GetSynan()->SetKillHomonymsMode(CoverageKillHomonyms);

}

bool CSemStructureBuilder::FindSituationsForNextSentence()
{
	if (m_CurrentSentence >=  m_RusStr.m_pData->GetSynan()->m_vectorSents.size())
	{
		return false;
	};
	m_RusStr.FindSituations(m_CurrentSentence);
	m_RusStr.CopyDopRelationsExceptAnaphor();
	m_CurrentSentence++;
	return true;
}

bool  CSemStructureBuilder::FindSituations(std::string utf8text, long UserTreeVariantNo, std::string PO, long PanicTreeVariantCount, long UserClauseVariantsCombinationNo, std::string AllowableLexVars, std::string& Graph)
{
	try {
		
		m_RusStr.m_ClauseCombinationVariantsCount = 0;
		m_CurrentSentence = 0;
		m_RusStr.m_PO = PO;
		Graph = "";
		m_RusStr.m_PanicTreeVariantCount = PanicTreeVariantCount;
		m_RusStr.m_UserTreeVariantNo = UserTreeVariantNo;
				


		m_RusStr.ProcessAllowableLexVars(AllowableLexVars.c_str());

		if (!m_RusStr.TryTestTree(utf8text))
		{
			Trim(utf8text);
			
			if (!utf8text.empty())
			{
				if (!m_RusStr.m_pData->MakeSyntaxStr(utf8text.c_str(), m_GlobalSpan))
				{
					ErrorMessage("Syntax has crushed");
					return false;
				};
				m_GlobalSpan.StartTimer("Semantics",0);
				m_RusStr.FindSituations(m_CurrentSentence, UserClauseVariantsCombinationNo);
				m_GlobalSpan.EndTimer("Semantics");
				if (m_RusStr.m_bShouldBeStopped) return true;
			}	
			else
			{
				m_RusStr.m_Relations.clear();
				m_RusStr.m_Nodes.clear();
			}

		};
		if (m_bShouldBuildTclGraph) 
		{
			Graph =  m_RusStr.GetTclGraph (false, true);
			Graph += "if { [catch SetLayout] != 0 } {  $GT($main,graph) draw  }\1";
			Graph += m_RusStr.GetOtherRelations ();
			Graph += m_RusStr.GetClauseTreeForTcl();
		}
		std::ofstream outp;
		outp.open("graph_debug.txt");
		auto d = Graph;
		std::replace(d.begin(), d.end(), '\1', '\n');
		outp << d;
		outp.close();

		m_RusStr.CopyDopRelationsExceptAnaphor();
		m_CurrentSentence ++;

		return true;
	}
	catch (...)
	{
		ErrorMessage("General error");
		return false;
	};
}



bool CSemStructureBuilder::TranslateToEnglish(std::string& Graph)
{
	try 
	{
		Graph = "";
		m_GlobalSpan.StartTimer("Transfer",0);
		m_EngStr.m_pData->InitializeIndices();
		
		m_EngStr.TranslateToEnglish();
		m_GlobalSpan.EndTimer("Transfer");
        if (m_bShouldBuildTclGraph) 
		{
			Graph =  m_EngStr.GetTclGraph (false, true);
			Graph += "if { [catch SetLayout] != 0 } {  $GT($main,graph) draw  }\1";
			Graph += m_EngStr.GetOtherRelations ();
			
		}
		
		return true;
	}
	catch (...)
	{
		return false;
	};
}


std::string CSemStructureBuilder::TranslateRussianText(const std::string& russian, const std::string& po, void(*logger)(const char*)) {
	std::string graphStr;
	try {
		if (logger) logger(Format("  FindSituations (po=%s) %s\n", po.c_str(), russian.c_str()).c_str());
		FindSituations(russian.c_str(), 0, po.c_str(), 20000, -1, "", graphStr);
	}
	catch (CExpc c) {
		throw;
	}
	catch (...)
	{
		std::string s = m_RusStr.m_pData->m_LastError;
		if (s.empty())
			s = "unknown error";
		throw CExpc(s);
	}

	try {
		std::string res;
		for (;;) {
			if (logger) logger("  TranslateToEnglish\n");
			if (!TranslateToEnglish(graphStr)) {
				if (logger) logger("Error in TranslateToEnglish\n");
				return "Unexpected Error";
			}

			if (logger) logger("  BuildSentence\n");
			std::string TranslatedSent;
			if (!BuildSentence(TranslatedSent))
			{
				if (logger) logger("Error in Synthesis\n");
				return "Unexpected Error";
			}
			res += TranslatedSent;

			if (logger) logger("  FindSituationsForNextSentence\n");
			if (!FindSituationsForNextSentence()) break;
		}
		//остаются недопереведенные слова
		return convert_to_utf8(res.c_str(), morphRussian);
	}
	catch (CExpc c) {
		throw;
	}
	catch (...)
	{
		throw CExpc("Error in TranslateToEnglish or Synthesis");
	}
}

bool  CSemStructureBuilder::BuildSentence(std::string& Sentence)
{
	m_GlobalSpan.StartTimer("Synthesis",0);	
	m_RusStr.m_pData->InitializeIndices();
	Sentence =  m_EngStr.BuildSentence();
	m_GlobalSpan.EndTimer("Synthesis");
	return true;
}

bool CSemStructureBuilder::SyntRusSentence(std::string& Sentence)
{
	m_GlobalSpan.StartTimer("Russian Synthesis",0);
	m_RusStr.m_pData->InitializeIndices();
	if (!m_RusStr.RussianSynthesis(Sentence)) return false;
	m_GlobalSpan.EndTimer("Russian Synthesis");
	return true;
}

void CSemStructureBuilder::SaveThisSentence(void)
{
	m_SavedSentences.push_back(m_RusStr);
}

void  CSemStructureBuilder::ClearSavedSentences(void)
{
	m_SavedSentences.clear();
}

std::string  CSemStructureBuilder::Answer()
{
	std::string Result;

	if (!m_RusStr.SemanticAnswer(Result, m_SavedSentences))
		Result = "I do not know";
		
	return Result;
}


long CSemStructureBuilder::GetScrollMax() const
{
	return m_RusStr.m_ClauseCombinationVariantsCount;
}

long  CSemStructureBuilder::GetScrollCurrent() const
{
	return m_RusStr.m_ClauseVariantsCombinationNo;
}

