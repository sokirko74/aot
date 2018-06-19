#pragma once

#include "../common/utilit.h"
#include "../LemmatizerLib/MorphologyHolder.h"
#include "../SynanLib/SentencesCollection.h"
#include "../MAPostLib/PostMorphInterface.h" 


class CSyntaxHolder : public  CMorphologyHolder
{
public:
	CSentencesCollection			m_Synan;
	CPostMorphInteface*				m_pPostMorph;


	CSyntaxHolder();
	~CSyntaxHolder();

	bool LoadSyntax(MorphLanguageEnum langua);
	bool GetSentencesFromSynAn(string str, bool bFile);
	
	string  GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const;
};
