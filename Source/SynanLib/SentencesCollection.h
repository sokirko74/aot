// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#pragma once

#include "../SynCommonLib/Sentence.h"
#include "../common/utilit.h"


class CSentencesCollection  
{
public:
	CSentencesCollection();
	~CSentencesCollection();
	std::vector<CSentence*>						m_vectorSents;
	bool									m_bEnableProgressBar;
	bool									m_bLogProcessedSentence;

	bool CreateOptions(MorphLanguageEnum langua);
	bool InitializeProcesser();
	bool ProcessData(const CPlmLineCollection* piPlmLine);
	void ClearSentences();
	void ClearOptions();
	void put_SilentMode(bool newVal);
	void SetLemmatizer(const CLemmatizer* R);
	void SetOborDic(const CDictionary* D);
	void SetEnableAllThesauri(bool Value);
	const CSyntaxOpt* GetOpt() const 
	{
		return m_pSyntaxOptions;
	};
	void SetKillHomonymsMode(KillHomonymsEnum k){ m_pSyntaxOptions->m_KillHomonymsMode =  k;}

protected:
	
	CSyntaxOpt*								m_pSyntaxOptions;



	/*
	 слот m_bDeleteEqualPrimitiveClauseVariants  для семантики должен быть равен true,
	 а  для VisualSynan - false.
	 Если он истина, тогда удаляются  все морфологические варианты, 
	 равные  с точностью до омонимов слов(!). Например, если в клаузе есть два 
	 юнита Х1 и Х2,	 Х1 - клауза с двумя вариантами, Х2 - слово с одном вариантом,
	 тогда из двух вариантов останется один (любой).
	 Это нужно, чтобы семантике не подавались  одинаковые варианты.
	*/
	bool	m_bDeleteEqualPrimitiveClauseVariants;

	bool ReadAndProcessSentences(const CPlmLineCollection* piPLMLinePtr);
};
