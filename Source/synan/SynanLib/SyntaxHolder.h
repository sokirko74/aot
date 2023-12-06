#pragma once

#include "morph_dict/common/utilit.h"
#include "morphan/LemmatizerLib/LemTextCreator.h"
#include "SentencesCollection.h"
#include "synan/MAPostLib/PostMorphInterface.h"


class CSyntaxHolder : public  CLemTextCreator
{
public:
	CSentencesCollection			m_Synan;
	CPostMorphInteface*				m_pPostMorph;


	CSyntaxHolder();
	~CSyntaxHolder();

	void LoadSyntax(MorphLanguageEnum langua);
	void ClearHolder();
	bool GetSentencesFromSynAn(std::string str, bool bFile);
	
	std::string  GetClauseTypeDescr(const CClause& C, int ClauseTypeNo) const;
};
