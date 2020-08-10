#ifndef CSemStructureBuilder_H
#define CSemStructureBuilder_H

#include "../SemanLib/SemanticRusStructure.h"
#include "../Transfer/EngSemStructure.h"

class CSemStructureBuilder
{
public:
	CEngSemStructure			m_EngStr;
	CRusSemStructure			m_RusStr;
	long						m_CurrentSentence;
	CMyTimeSpanHolder			m_GlobalSpan; 
	bool						m_bShouldBuildTclGraph;
	vector<CRusSemStructure>	m_SavedSentences;

	CSemStructureBuilder(void);
	~CSemStructureBuilder(void);

	bool	FindSituationsForNextSentence();
	bool	FindSituations(std::string text, long UserTreeVariantNo, std::string PO, long PanicTreeVariantCount, long UserClauseVariantsCombinationNo, std::string AllowableLexVars, std::string& Graph);
	bool	TranslateToEnglish(std::string& Graph);
	bool	BuildSentence(std::string& Sentence);
	bool	SyntRusSentence(std::string& Sentence);
	void	SaveThisSentence(void);
	void	ClearSavedSentences(void);
	std::string  Answer();
	long	GetScrollMax() const;
	long	GetScrollCurrent() const;


};

#endif

