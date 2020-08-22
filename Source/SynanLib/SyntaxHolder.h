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

	bool RunMapost(CPlmLineCollection& mapostPlmLines);
	bool LoadSyntax(MorphLanguageEnum langua);
	bool GetSentencesFromSynAn(std::string str, bool bFile);
	
	std::string  GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const;
};
