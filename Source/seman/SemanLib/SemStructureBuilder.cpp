#include "stdafx.h"
#include "SemStructureBuilder.h"
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Initializers/ConsoleInitializer.h>


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


void  CSemStructureBuilder::FindSituations(std::string utf8text, CSemOptions opts)
{
	m_RusStr.SetOptions(opts);
	m_RusStr.m_ClauseCombinationVariantsCount = 0;
	m_CurrentSentence = 0;
	
	if (!m_RusStr.TryTestTree(utf8text))
	{
		Trim(utf8text);
			
		if (!utf8text.empty())
		{
			if (!m_RusStr.m_pData->MakeSyntaxStr(utf8text.c_str(), m_GlobalSpan))
			{
				throw CExpc("Syntax has crushed");
			};
			m_GlobalSpan.StartTimer("Semantics",0);
			m_RusStr.FindSituations(m_CurrentSentence);
			m_GlobalSpan.EndTimer("Semantics");
			if (m_RusStr.m_bShouldBeStopped) return;
		}	
		else
		{
			m_RusStr.m_Relations.clear();
			m_RusStr.m_Nodes.clear();
		}

	};


	m_RusStr.CopyDopRelationsExceptAnaphor();
	m_CurrentSentence ++;
}



void CSemStructureBuilder::TranslateToEnglish(CSemOptions)
{
	m_GlobalSpan.StartTimer("Transfer",0);
	m_EngStr.m_pData->InitializeIndices();
	m_EngStr.TranslateToEnglish();
	m_GlobalSpan.EndTimer("Transfer");
}


std::string CSemStructureBuilder::TranslateRussianText(const std::string& russian, const std::string& po) {
	std::string graphStr;
	try {
		LOGI << "FindSituations " << russian;
		CSemOptions opts;
		opts.m_Domain = po;
		FindSituations(russian, opts);
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
			LOGI << "TranslateToEnglish";
			TranslateToEnglish();

			LOGI<< "BuildSentence";
			std::string TranslatedSent;
			if (!BuildSentence(TranslatedSent))
			{
				LOGE << "Error in Synthesis";
				return "Unexpected Error";
			}
			res += TranslatedSent;

			LOGI << "FindSituationsForNextSentence";
			if (!FindSituationsForNextSentence()) break;
		}
		//остаются недопереведенные слова
		return convert_to_utf8(res.c_str(), morphRussian);
	}
	catch (CExpc c) {
		LOGE << c.what();
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


long CSemStructureBuilder::GetScrollMax() const
{
	return m_RusStr.m_ClauseCombinationVariantsCount;
}

long  CSemStructureBuilder::GetScrollCurrent() const
{
	return m_RusStr.m_ClauseVariantsCombinationNo;
}


class MyFormatter
{
public:
	static plog::util::nstring header()
	{
		return plog::util::nstring();
	}

	static plog::util::nstring format(const plog::Record& record)
	{
		plog::util::nostringstream ss;
		ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
		ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
		ss << record.getMessage() << PLOG_NSTR("\n");
		auto mess = ss.str();
		//if (record.getSeverity() != plog::Severity::verbose) {
		//	std::cerr << mess << "\n";
		//}
		return mess;
	}
};


void init_plog_seman(plog::Severity severity, std::string filename) {
	if (std::filesystem::exists(filename)) {
		std::filesystem::remove(filename);
	}
	plog::init<MyFormatter>(severity, filename.c_str());
}

