// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef __SENTENCESCOLLECTION_H_
#define __SENTENCESCOLLECTION_H_


#include "Sentence.h"
#include "../common/utilit.h"


class CSentencesCollection  
{
public:
	CSentencesCollection();
	~CSentencesCollection();
	vector<CSentence*>						m_vectorSents;
	bool									m_bEnableProgressBar;
	bool									m_bLogProcessedSentence;

	bool CreateOptions(MorphLanguageEnum langua);
	bool InitializeProcesser();
	bool ProcessData(const CPlmLineCollection* piPlmLine);
	void ClearSentences();
	void ClearOptions();
	void put_SilentMode(bool newVal);
	//void SetThesaurus(UINT ThesId, const CThesaurus* Thes);
	void SetLemmatizer(const CLemmatizer* R);
	void SetOborDic(const CDictionary* D);
	void SetEnableAllThesauri(bool Value);
	const CSyntaxOpt* GetOpt() const 
	{
		return m_pSyntaxOptions;
	};


protected:
	
	CSyntaxOpt*								m_pSyntaxOptions;



	/*
	 слот m_bDeleteEqualPrimitiveClauseVariants  дл€ семантики должен быть равен true,
	 а  дл€ VisualSynan - false.
	 ≈сли он истина, тогда удал€ютс€  все морфологические варианты, 
	 равные  с точностью до омонимов слов(!). Ќапример, если в клаузе есть два 
	 юнита ’1 и ’2,	 ’1 - клауза с двум€ вариантами, ’2 - слово с одном вариантом,
	 тогда из двух вариантов останетс€ один (любой).
	 Ёто нужно, чтобы семантике не подавались  одинаковые варианты.
	*/
	bool	m_bDeleteEqualPrimitiveClauseVariants;

	bool ReadAndProcessSentences(const CPlmLineCollection* piPLMLinePtr);
};

#endif //__SENTENCESCOLLECTION_H_
