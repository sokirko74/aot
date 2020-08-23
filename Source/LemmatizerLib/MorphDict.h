// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef AhoKorasickMorph_h
#define AhoKorasickMorph_h


#include "MorphAutomat.h"
#include "../MorphWizardLib/FormInfo.h"


struct CLemmaInfoAndLemma 
{
	int			m_LemmaStrNo;
	CLemmaInfo	m_LemmaInfo;

	bool operator < (const CLemmaInfoAndLemma& X) const 
	{
		if (m_LemmaInfo.m_FlexiaModelNo != X.m_LemmaInfo.m_FlexiaModelNo)
			return m_LemmaInfo.m_FlexiaModelNo < X.m_LemmaInfo.m_FlexiaModelNo;

		return m_LemmaStrNo < X.m_LemmaStrNo;
	};

};




class CMorphDict : public CMorphWizardBase
{
	
protected:
	
	CMorphAutomat*				m_pFormAutomat;

	std::vector<int>					m_ModelsIndex;
	
	

	struct IsLessMorphInterp 
	{
		const CShortStringHolder& m_SearchInfos;

		IsLessMorphInterp(const CShortStringHolder& SearchInfos) : m_SearchInfos(SearchInfos) {};

		bool	operator () (const CLemmaInfoAndLemma& _X1, const char* _X2) const
		{
			const char* base1 = m_SearchInfos[_X1.m_LemmaStrNo].GetString();

			return strcmp(base1, _X2) < 0;
		};
		bool	operator () (const char* _X1, const CLemmaInfoAndLemma& _X2) const
		{
			const char* base2 = m_SearchInfos[_X2.m_LemmaStrNo].GetString();

			return strcmp(_X1, base2) < 0;
		};
		bool	operator () (const CLemmaInfoAndLemma& _X1, const CLemmaInfoAndLemma& _X2) const
		{
			const char* base1 = m_SearchInfos[_X1.m_LemmaStrNo].GetString();
			const char* base2 = m_SearchInfos[_X2.m_LemmaStrNo].GetString();

			return strcmp(base1, base2) < 0;
		};


	};
	IsLessMorphInterp m_SearchInfoLess;
	void	GetLemmaInfos (const std::string& Text, size_t TextPos, std::vector<CAutomAnnotationInner>& Infos) const;
	void	CreateModelsIndex();

public:
	CShortStringHolder			m_Bases;
	std::vector<CLemmaInfoAndLemma>	m_LemmaInfos;
	StringVector				m_Prefixes;
	std::vector<BYTE>				m_NPSs;

	CMorphDict(MorphLanguageEnum Language);
	virtual ~CMorphDict();
	void	InitAutomat(CMorphAutomat*	pFormAutomat);

	//  loading & saving
	bool	Load(std::string GrammarFileName);
	bool	Save(std::string GrammarFileName) const;
	
	
	void	PredictBySuffix (const std::string& Text, size_t& TextOffset, size_t MinimalPredictSuffixlen, std::vector<CAutomAnnotationInner>& Infos) const;
	std::string	GetAllMorphInterpsStr (const std::string& Text,const size_t TextPos, bool bFullInterp) const;
};



#endif
