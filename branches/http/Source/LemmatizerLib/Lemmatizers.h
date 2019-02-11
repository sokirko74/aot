// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#ifndef __LEMMATIZERS_H_
#define __LEMMATIZERS_H_

#pragma warning (disable : 4786)

#include "MorphDict.h"
#include "Paradigm.h"
#include "Statistic.h"
#include "Predict.h"

#pragma warning (disable : 4250)

class CGraphmatFile;

typedef enum {	subjFinance = 1, subjComputer = 2, subjLiterature = 4	} SubjectEnum;


class CLemmatizer  : public CMorphDict
{
protected:
	string				m_Registry;
	
    // Postfixes-particles, that do not change the meaning 
	set<string>     	m_HyphenPostfixes;
    // productive prefixes
    set<string> 		m_HyphenPrefixes;

	CStatistic			m_Statistic;
	CPredictBase		m_Predict;
	set<string>			m_PrefixesSet;


	virtual void			FilterSrc(string& src) const = 0;

	string					GetRegistryString()  const	{return m_Registry;	};
	string					GetPath()  const;
	void					ReadOptions(string FileName);
		
	
	bool					LemmatizeWord(string& InputWordStr, const bool cap, const bool predict, vector<CAutomAnnotationInner>& results, bool bGetLemmaInfos) const;
	void					AssignWeightIfNeed(vector<CAutomAnnotationInner>& FindResults) const;

	// prediction by suffix
	bool					CheckAbbreviation(string InputWordStr,vector<CAutomAnnotationInner>& FindResults, bool is_cap) const;
	CAutomAnnotationInner	ConvertPredictTupleToAnnot(const CPredictTuple& input) const;
	void					PredictByDataBase(string InputWordStr,  vector<CAutomAnnotationInner>&  results,bool is_cap) const;
	bool					IsPrefix(const string& Prefix) const;
	

public:
	bool					m_bLoaded;
	bool					m_bMaximalPrediction;
	bool					m_bUseStatistic;
	bool					m_bAllowRussianJo;

	
	
	
	CLemmatizer(MorphLanguageEnum Language);
	virtual ~CLemmatizer();

	//  basic methods
	MorphLanguageEnum GetLanguage()  const {return m_pFormAutomat->m_Language; };
	const CStatistic& GetStatistic() const;
	bool	CheckABC(const string& WordForm) const;
	bool	IsHyphenPostfix(const string& Postfix) const;
    bool    IsHyphenPrefix(const string& Prefix) const;

	
	//  loading
	bool	LoadDictionariesRegistry(string& strError) ;
	bool	LoadStatisticRegistry(SubjectEnum subj);

	//  main interfaces
	bool	CreateParadigmCollection(bool bNorm, string& WordStr, bool capital, bool bUsePrediction, vector<CFormInfo>& Result) const;
	void	GetAllAncodesQuick(const BYTE* WordForm, bool capital, BYTE* OutBuffer, bool bUsePrediction) const;
	//string	GetAllAncodesAndLemmasQuick(string& InputWordStr, bool capital) const;
	bool	GetAllAncodesAndLemmasQuick(string& InputWordStr, bool capital, char* OutBuffer, size_t MaxBufferSize, bool bUsePrediction) const;
	bool	CreateParadigmFromID(DWORD id, CFormInfo& Result) const;
	bool	ProcessHyphenWords(CGraphmatFile* piGraphmatFile) const;



};





/////////////////////////////////////////////////////////////////////////////
class CLemmatizerRussian : public CLemmatizer
{
public:
	CLemmatizerRussian();
	virtual ~CLemmatizerRussian() {};
	void FilterSrc(string& src) const;
};


class  CLemmatizerEnglish : public CLemmatizer
{
	void FilterSrc(string& src) const;
public:
	CLemmatizerEnglish();
	virtual ~CLemmatizerEnglish() {};
	
};



/////////////////////////////////////////////////////////////////////////////

class  CLemmatizerGerman: public CLemmatizer
{
	void FilterSrc(string& src) const;
public:
	CLemmatizerGerman();
	virtual ~CLemmatizerGerman() {};
	
};





#endif //__LEMMATIZERS_H_
