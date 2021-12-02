#pragma once

#include "morph_dict/common/utilit.h"
#include "morphan/LemmatizerLib/GraphanAndMorphanHolder.h"
#include "SentencesCollection.h"
#include "synan/MAPostLib/PostMorphInterface.h"


class CSyntaxHolder : public  CGraphanAndMorphanHolder
{
public:
	CSentencesCollection			m_Synan;
	CPostMorphInteface*				m_pPostMorph;


	CSyntaxHolder();
	~CSyntaxHolder();

	bool RunMapost(CPlmLineCollection& mapostPlmLines);
	void LoadSyntax(MorphLanguageEnum langua);
	bool GetSentencesFromSynAn(std::string str, bool bFile);
	
	std::string  GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const;
};
