// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "stdafx.h"
#include "../../common/cortege.h"
#include "oborot.h"
#include "SyntaxInit.h"


void COborotForSyntax::Init() 
{ 
	m_Poses = 0;
	m_AllPossibleDependCases = 0;
	m_DependCases = 0;
	m_ConjType = no_type;
	m_bConjWithInfinitiveSubord = false;
	m_bParenthesis = false;
		
};
COborotForSyntax::COborotForSyntax () 
{
	Init();
};


void COborotForSyntax::AddPartOfSpeech(BYTE POS)
{
	m_Poses |= (1<<POS);
};

bool COborotForSyntax::HasPartOfSpeech(BYTE POS) const 
{
	return (m_Poses & (1<<POS)) > 0;
};

bool COborotForSyntax::HasOnlyPartOfSpeech(BYTE POS) const 
{
	return m_Poses ==  (1<<POS);
};



COborDic::COborDic(const CSyntaxOpt* Opt)
{
	m_pSyntaxOptions = Opt;
};

const CSyntaxOpt* COborDic::GetOpt() const
{
	return m_pSyntaxOptions;
};

const char OborotDels[] = " \t";

int COborDic::FindSubConj(const std::string& word_upper) const
{
	const StringVector& V = GetSubConjs();
	for (size_t i = 0; i < V.size(); ++i) {
		if (V[i] == word_upper) {
			return i;
		}
	}
	return -1;
}


bool COborDic::ReadOborots(const CDictionary* piOborDic)
{
	if( ! ReadOborDic(piOborDic) )
		return false;
	MergeCasesOfSimililarSimplePreps();		
	return true;
};


std::vector<int> COborDic::FindAllArticlesForSimplePrep(std::string strPrep) const
{
	std::vector<int> v;
	if (strPrep.empty()) return v;
	RmlMakeLower(strPrep, GetOpt()->m_Language);

	CSimplePrepToArticles::const_iterator it = m_mapSimplePrep.find(strPrep);

	assert (!m_mapSimplePrep.empty());

	if( it != m_mapSimplePrep.end() )	
	{
		assert(it->second.size() > 0 );
		return it->second;
	}
	
	return v;
}


int COborDic::FindSimplePrep(std::string strPrep) const
{
	RmlMakeLower(strPrep, GetOpt()->m_Language);

	CSimplePrepToArticles::const_iterator it = m_mapSimplePrep.find(strPrep);
	if( it != m_mapSimplePrep.end() )	
	{
		assert(it->second.size() > 0 );
		return it->second[0];
	}
	

	return -1;
}


void  COborDic::WriteSimplePrep(std::string s, int OborotNo)
{
	std::string strPrep = s;
	size_t ii = strPrep.find_first_of(" ");

	StringTokenizer token(strPrep.c_str(), " ");	
	const char *word;

	word = token();
	if( !word )
		return;
	strPrep = word;
	
	if( token() )
		return;	

	CSimplePrepToArticles::iterator it;
	it = m_mapSimplePrep.find(strPrep);
	if( it == m_mapSimplePrep.end() )
	{
		std::vector<int> v;
		v.push_back(OborotNo);
		m_mapSimplePrep[strPrep] = v;
	}
	else
	{
		const std::string&  debug  = it->first;
		if( find(it->second.begin() , it->second.end(), OborotNo) == it->second.end())
			it->second.push_back(OborotNo);
	}
}

void  COborDic::MergeCasesOfSimililarSimplePreps()
{
	/*	для всех сложных оборотов все возможные падежи равны тем, которые записаны в статье,
		поскольку  для сложных  оборотов омонимия не предусморена.
	*/
	for (int ArtNo=0; ArtNo < m_Entries.size(); ArtNo++)
		m_Entries[ArtNo].m_AllPossibleDependCases = m_Entries[ArtNo].m_DependCases;

	CSimplePrepToArticles::iterator it;
	int i = 0;
	for(it = m_mapSimplePrep.begin() ; it != m_mapSimplePrep.end() ; it++ )

	{
		//const std::string&  debug  = it->first;
		std::vector<int>& article_nums = it->second;
		uint64_t _cases = 0;
		
		for(i=0; i < article_nums.size() ; i++)		
			_cases |= m_Entries[article_nums[i]].m_DependCases;

		for(i = 0; i < article_nums.size() ; i++)		
			m_Entries[article_nums[i]].m_AllPossibleDependCases = _cases;

	}
}

void COborDic::BuildOborots(int iUnit, const CDictionary* piOborDic, WriteOborFuncType WriteOborFunc)
{
	for (size_t k = piOborDic->GetUnitStartPos(iUnit); k <= piOborDic->GetUnitLastPos(iUnit); k++)
		  if (piOborDic->GetCortegeFieldNo(k) == piOborDic->GetFieldNoByFieldStr("CONTENT"))
		  {
			  std::string q = piOborDic->GetDomItemStr(k, 0);			  
			  BuildOborot(q, iUnit, WriteOborFunc);
		  };
}


void COborDic::TokenizeDoubleConj(std::string s, int OborotNo)
{
	StringTokenizer token(s.c_str(), " ");
	SDoubleConj DoubleConj;
	const char *word;
	bool InFirstPart = true;
	while( word = token() )
	{
		SDoubleConj::string30 s;
		assert (strlen(word) < MaxConjSize - 1);
		strcpy (s.m_item, word);
		RmlMakeUpper (s.m_item, GetOpt()->m_Language);

		if( !strcmp(word, "..." ) )
			InFirstPart = false;
		else
			if (InFirstPart)
				DoubleConj.m_FirstPart.push_back(s);
			else
				DoubleConj.m_SecondPart.push_back(s);
	}	

	DoubleConj.m_iOborNum = OborotNo;
	DoubleConj.m_bRepeating = (DoubleConj.m_FirstPart == DoubleConj.m_SecondPart);
	std::vector<SDoubleConj>::iterator it = lower_bound (m_DisruptConj.begin(), m_DisruptConj.end(), DoubleConj);
	m_DisruptConj.insert(it, DoubleConj);
	

}

void  COborDic::WriteSimpleSubConj(std::string s, int OborotNo)
{
	RmlMakeUpper (s, GetOpt()->m_Language);
	m_SimpleSubConj.push_back(s);
}


void COborDic::BuildOborot(std::string s, int OborotNo, WriteOborFuncType WriteOborFunc)
{
	size_t i = s.find("(");
	if (i == s.npos) 
	{
		char q[200];
		strcpy (q, s.c_str());
		char* t = q + strspn (q,OborotDels);

		if (s.find("+") == s.npos)		
			((*this).*(WriteOborFunc))(t, OborotNo);
	}
	else
	{
		size_t k = s.find (")");
		if (k == s.npos)
		{
			char Error[100];
			sprintf (Error, "Error in parenthesis  in oborot %s", s.c_str());
			ErrorMessage("Synan", Error);
		};
		size_t last_j = i;
        	for (size_t j =i+1; j <= k; j++)
		{
          		if  (  	  	(j == k)
				    ||		(s[j] == '|'))
		  {
	            std::string q;
			if (i >0) q += s.substr(0,i-1);
			q += " ";
			q += s.substr(last_j+1, j-last_j-1);
			q += " ";
			if (k-1 < s.length()) q += s.substr(k+1);
			BuildOborot(q, OborotNo, WriteOborFunc);
			last_j = j;
		  };

		};

	};
}



