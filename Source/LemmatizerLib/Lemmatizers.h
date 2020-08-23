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
	std::string				m_Registry;
	
    // Postfixes-particles, that do not change the meaning 
	std::set<std::string>     	m_HyphenPostfixes;
    // productive prefixes
    std::set<std::string> 		m_HyphenPrefixes;

	CStatistic			m_Statistic;
	CPredictBase		m_Predict;
	std::set<std::string>			m_PrefixesSet;


	virtual void			FilterSrc(std::string& src) const = 0;

	std::string					GetRegistryString()  const	{return m_Registry;	};
	std::string					GetPath()  const;
	void					ReadOptions(std::string FileName);
		
	
	bool					LemmatizeWord(std::string& InputWordStr, const bool cap, const bool predict, std::vector<CAutomAnnotationInner>& results, bool bGetLemmaInfos) const;
	void					AssignWeightIfNeed(std::vector<CAutomAnnotationInner>& FindResults) const;

	// prediction by suffix
	bool					CheckAbbreviation(std::string InputWordStr,std::vector<CAutomAnnotationInner>& FindResults, bool is_cap) const;
	CAutomAnnotationInner	ConvertPredictTupleToAnnot(const CPredictTuple& input) const;
	void					PredictByDataBase(std::string InputWordStr,  std::vector<CAutomAnnotationInner>&  results,bool is_cap) const;
	bool					IsPrefix(const std::string& Prefix) const;
	

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
	bool	CheckABC(const std::string& WordForm) const;
	bool	IsHyphenPostfix(const std::string& Postfix) const;
    bool    IsHyphenPrefix(const std::string& Prefix) const;

	
	//  loading
	bool	LoadDictionariesRegistry(std::string& strError) ;
	bool	LoadStatisticRegistry(SubjectEnum subj);

	//  main interfaces
	bool	CreateParadigmCollection(bool bNorm, std::string& WordStr, bool capital, bool bUsePrediction, std::vector<CFormInfo>& Result) const;
	void	GetAllAncodesQuick(const BYTE* WordForm, bool capital, BYTE* OutBuffer, bool bUsePrediction) const;
	//std::string	GetAllAncodesAndLemmasQuick(std::string& InputWordStr, bool capital) const;
	bool	GetAllAncodesAndLemmasQuick(std::string& InputWordStr, bool capital, char* OutBuffer, size_t MaxBufferSize, bool bUsePrediction) const;
	bool	CreateParadigmFromID(DWORD id, CFormInfo& Result) const;
	bool	ProcessHyphenWords(CGraphmatFile* piGraphmatFile) const;



};





/////////////////////////////////////////////////////////////////////////////
class CLemmatizerRussian : public CLemmatizer
{
public:
	CLemmatizerRussian();
	virtual ~CLemmatizerRussian() {};
	void FilterSrc(std::string& src) const;
};


class  CLemmatizerEnglish : public CLemmatizer
{
	void FilterSrc(std::string& src) const;
public:
	CLemmatizerEnglish();
	virtual ~CLemmatizerEnglish() {};
	
};



/////////////////////////////////////////////////////////////////////////////

class  CLemmatizerGerman: public CLemmatizer
{
	void FilterSrc(std::string& src) const;
public:
	CLemmatizerGerman();
	virtual ~CLemmatizerGerman() {};
	
};





#endif //__LEMMATIZERS_H_
