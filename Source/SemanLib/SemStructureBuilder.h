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
	bool	FindSituations(string text, long UserTreeVariantNo, string PO, long PanicTreeVariantCount, long UserClauseVariantsCombinationNo, string AllowableLexVars, string& Graph);
	bool	TranslateToEnglish(string& Graph);
	bool	BuildSentence(string& Sentence);
	bool	SyntRusSentence(string& Sentence);
	void	SaveThisSentence(void);
	void	ClearSavedSentences(void);
	string  Answer();
	long	GetScrollMax() const;
	long	GetScrollCurrent() const;


};

#endif

