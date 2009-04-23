// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#ifndef thesaurus_for_syntax_h
 #define thesaurus_for_syntax_h

#include "stdafx.h"
#include "Group.h"


typedef vector<const char*> CVectorOfStrings;
typedef pair<int,int>   CIntPair;
typedef vector<	int	>     CIntVector;
typedef map< int, CIntVector > CInt2Vector;
typedef set<int> CSetOfInt;
typedef pair<int const, CIntVector> CInt2IntVectorPair;

class CGroups;

class CTermin : public CVectorOfStrings
{
public:
	const char*		m_strTermin; 
	const CGroups*	m_pModel;
	int				m_TerminId;
	EThesType		m_ThesType;

	CTermin() 
	{
		m_pModel = 0;
	};

	bool operator<(const CTermin& term) const
	{ 
		assert( size() > 0);
		return strcmp((*this)[0], term[0]) < 0;
	}

	string GetTerminStr() const
	{
		string res;
		for(int i = 0 ; i < size() ; i++)
		{
			res += (*this)[i];
			res += " ";
		}
		return res;

	}
};


typedef vector<CTermin> CVectorOfTermins;



class CTerminSort_less : public binary_function<int, int, bool>
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
		int size = m_pTermins->size();
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



inline EThesType GetThesTypeByStr(const string& strDBName)
{
	if (strDBName == "RML_THES_LOC") return LocThes;
	if (strDBName == "RML_THES_FIN") return FinThes;
	if (strDBName == "RML_THES_OMNI") return OmniThes;
	if (strDBName == "RML_THES_COMP") return CompThes;
	return	NoneThes;
}

class CSyntaxOpt;

class CThesaurusForSyntax
{
public:
	CSyntaxOpt*								m_pSyntaxOptions;
	const CSyntaxOpt* GetOpt() const 	{		return m_pSyntaxOptions;	};

	CInt2Vector m_TerminsGrouppedByLength; //здесь хранятся индексы из m_Termins

	CThesaurusForSyntax(CSyntaxOpt* Opt);
	virtual ~CThesaurusForSyntax();

	CThesaurus* LoadThesaurus(const char* ThesName) const;

	bool ReadThesaurusForSyntax(const char* ThesName, const CThesaurus* Thes, StringVector& p_vectorAccost);
	bool ReadTermins(const CThesaurus* piThes, EThesType eThesType);
	bool ReadModels(const CThesaurus& piThes, EThesType eThesType);
	void SortIndexes();
	const CTermin& GetTermin(int i) const
	{ return m_Termins[i]; }

	const CVectorOfTermins* GetTermins() const 
	{ return &m_Termins; }


protected:
	
	CVectorOfTermins					m_Termins; 	
	map<EThesType, vector<CGroups> >	m_AllThesModels;	
	CAgramtab*							m_pEngGramTab;

	virtual void AssignMainGroupsToModel(CGroups& model, const CInnerModel& piModel) = 0;

};




#endif
