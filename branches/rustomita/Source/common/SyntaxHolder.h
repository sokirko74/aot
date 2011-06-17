#ifndef SyntaxHolder_H
#define SyntaxHolder_H

#include "utilit.h"
#include "MorphologyHolder.h"
#include "../SynCommonLib/SentencesCollection.h"
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

#endif
