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

bool  CSemStructureBuilder::FindSituations(std::string text, long UserTreeVariantNo, std::string PO, long PanicTreeVariantCount, long UserClauseVariantsCombinationNo, std::string AllowableLexVars, std::string& Graph)
{
	try {
		
		m_RusStr.m_ClauseCombinationVariantsCount = 0;
		m_CurrentSentence = 0;
		m_RusStr.m_PO = PO;
		Graph = "";
		m_RusStr.m_PanicTreeVariantCount = PanicTreeVariantCount;
		m_RusStr.m_UserClauseVariantsCombinationNo = UserClauseVariantsCombinationNo;
		m_RusStr.m_UserTreeVariantNo = UserTreeVariantNo;
		


		m_RusStr.ProcessAllowableLexVars(AllowableLexVars.c_str());

		if (!m_RusStr.TryTestTree(text))
		{
			Trim(text);
			
			if (!text.empty())
			{

				if (!m_RusStr.m_pData->MakeSyntaxStr(text.c_str(), m_GlobalSpan)) 
				{
					ErrorMessage("Syntax has crushed");
					return false;
				};
				m_GlobalSpan.StartTimer(_R("Семантика"),0);
				m_RusStr.FindSituations(m_CurrentSentence);
				m_GlobalSpan.EndTimer(_R("Семантика"));
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
		m_GlobalSpan.StartTimer(_R("Трансфер"),0);
		m_EngStr.m_pData->InitializeIndices();
		
		m_EngStr.TranslateToEnglish();
		m_GlobalSpan.EndTimer(_R("Трансфер"));
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

bool  CSemStructureBuilder::BuildSentence(std::string& Sentence)
{
	m_GlobalSpan.StartTimer(_R("Синтез"),0);	
	m_RusStr.m_pData->InitializeIndices();
	Sentence =  m_EngStr.BuildSentence();
	m_GlobalSpan.EndTimer(_R("Синтез"));	
	return true;
}

bool CSemStructureBuilder::SyntRusSentence(std::string& Sentence)
{
	m_GlobalSpan.StartTimer(_R("Русский Синтез"),0);	
	m_RusStr.m_pData->InitializeIndices();
	if (!m_RusStr.RussianSynthesis(Sentence)) return false;
	m_GlobalSpan.EndTimer(_R("Русский Синтез"));	
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
		Result = _R("Я не знаю");
		
	return Result;
}


long CSemStructureBuilder::GetScrollMax() const
{
	if (m_RusStr.m_AllClausesVariants > 0)
		return m_RusStr.m_AllClausesVariants+m_RusStr.m_ClauseCombinationVariantsCount+1;
	else
		return  0;
}

long  CSemStructureBuilder::GetScrollCurrent() const
{
	return m_RusStr.m_InterfaceClauseNo;
}

