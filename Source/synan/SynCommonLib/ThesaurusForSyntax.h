#pragma once

#include "stdafx.h"
#include "Group.h"
#include "dicts/RmlThesLib/Thesaurus.h"


typedef std::vector<const char*> CVectorOfStrings;
typedef std::pair<int,int>   CIntPair;
typedef std::vector<	int	>     CIntVector;
typedef std::map< int, CIntVector > CInt2Vector;
typedef std::set<int> CSetOfInt;
typedef std::pair<int const, CIntVector> CInt2IntVectorPair;

class CGroups;

class CTermin : public CVectorOfStrings
{
public:
	const char*		m_strTermin; 
	const CGroups*	m_pModel;
	int				m_TerminId;
	EThesType		m_ThesType;

	CTermin();
	bool operator<(const CTermin& term) const;
	std::string GetTerminStr() const;
};


typedef std::vector<CTermin> CVectorOfTermins;


class CTerminSort_less
{
public:
	CTerminSort_less(const CVectorOfTermins* pTermins)
	{m_pTermins = pTermins;}

	void SetStringToCompare(const char*  str)
	{ 
		m_TerminToCompare.clear();
		m_TerminToCompare.push_back(str);
	}
	bool operator() (int i1, int i2)
	{
		int size = (int)m_pTermins->size();
		assert((i1 < size) && (i2 < size) );

		if( (i1 == -1) && (i2 >= 0) ) 
			return m_TerminToCompare < (*m_pTermins)[i2];

		if( (i2 == -1) && (i1 >=0) )
			return (*m_pTermins)[i1] < m_TerminToCompare;

		if( (i1 == -1) && (i2 == -1) )
			return false;

		return  (*m_pTermins)[i1] < (*m_pTermins)[i2];
	}

protected:
	const CVectorOfTermins* m_pTermins;
	CTermin m_TerminToCompare;

};




class CSyntaxOpt;

class CThesaurusForSyntax
{
	const CSyntaxOpt*								m_pSyntaxOptions;

public:
	const CSyntaxOpt* GetOpt() const 	{		return m_pSyntaxOptions;	};

	CInt2Vector m_TerminsGrouppedByLength; //здесь хранятся индексы из m_Termins

	CThesaurusForSyntax(const CSyntaxOpt* Opt);
	virtual ~CThesaurusForSyntax();

	CThesaurus* LoadThesaurus(std::string ThesName) const;

	void ReadThesaurusForSyntax(std::string ThesName, const CThesaurus* Thes);
	const CTermin& GetTermin(int i) const;
	const CVectorOfTermins* GetTermins() const;
	void SortIndexes();

protected:
	
	CVectorOfTermins					m_Termins; 	
	std::map<EThesType, std::vector<CGroups> >	m_AllThesModels;	
	CAgramtab*							m_pEngGramTab;

	virtual void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel) = 0;
	bool ReadOneTermin(const CThesaurus* piThes, const CInnerTermin& inputTerm, CTermin& outTerm) const;
	void ReadTermins(const CThesaurus* piThes, EThesType eThesType);
	void ReadModels(const CThesaurus& piThes, EThesType eThesType);
};


extern  EThesType GetThesTypeByStr(const std::string& strDBName);
