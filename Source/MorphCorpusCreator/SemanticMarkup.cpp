// SemanticMarkup.cpp: implementation of the CSemanticMarkup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SemanticMarkup.h"
#include "std::string_tokenizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSemanticMarkup::CSemanticMarkup()
{

}

CSemanticMarkup::~CSemanticMarkup()
{

}

bool CSemanticMarkup::Open(std::string strName)
{
	FILE* fp = fopen(strName.c_str(), "r");
	if( fp == NULL )
	{
		cout << "Can't read semantic markup file.(" << strName << ")" << endl;
		return false;
	}

	char ss[501];
	
	bool bFirst = true;
	while( !feof(fp) )
	{
		if( !fgets(ss, 500, fp) )
			break;
		if( bFirst )
		{
			ReadHeadLine(ss);
			bFirst = false;
		}
		else
			ReadItems(ss);

	}
	return true;
}

void CSemanticMarkup::ReadHeadLine(const char *ss)
{	
	const char *word;
	//bool bFirst = true;	
	int iPos = 0;
	bool bBreak = false;
	int i = 0;
	while( (word = strchr(ss + iPos,';')) )
	{
		int len = word - (ss + iPos);
		if( i <= 1 )
		{
			//bFirst = false;
			iPos += (len+1);
			i++;
			continue;
		}
		if( len == 0 )
		{
			bBreak = true;
			break;
		}
		else
			m_Titles.push_back(std::string(ss+iPos, len));
		i++;
		iPos += (len+1);
	}
	if( !bBreak )
	{
		int len = strlen(ss) - iPos - 1;
		if( len > 0 )
			m_Titles.push_back(std::string(ss+iPos, len));
	}
}

BYTE CSemanticMarkup::GetPOS(std::string s)
{
	if( s == "П")
		return AGRAMTABLib::ADJ_FULL;
	if( s == "С")
		return AGRAMTABLib::NOUN;
	if( s == "Г")
		return AGRAMTABLib::VERB;
	if( s == "Н")
		return AGRAMTABLib::ADV;
	if( s == "МС")
		return AGRAMTABLib::PRONOUN;
	if( s == "МС-П")
		return AGRAMTABLib::PRONOUN_P;
	if( s == "МС-ПРЕДК")
		return AGRAMTABLib::PRONOUN_PREDK;
	if( s == "ЧИСЛ-П")
		return AGRAMTABLib::NUMERAL_P;
	return UnknownPartOfSpeech;
}

void CSemanticMarkup::ReadItems(const char *ss)
{
	StringTokenizer token(ss, ";");	
	const char *word;
	bool bFirst = true;
	int i = 0;	
	SLemma lemma;
	SSemMeaning semMeaning;
	int iPos = 0;
	bool bBreak = false;
	while( (word = strchr(ss + iPos,';') ) )
	{
		int len = word - (ss + iPos);
		if( i == 0  )
		{
			lemma.m_POS = GetPOS(std::string(ss+iPos, len));
		}
		else if( i == 1 )
		{			
			lemma.m_sLemma = std::string(ss+iPos, len);
			::strupr((char*)lemma.m_sLemma.c_str());
		}
		else
			if( len == 0 )
				semMeaning.m_values.push_back("");	
			else
				semMeaning.m_values.push_back(std::string(ss+iPos, len));
		i++;
		if( i - 2 >= (int)(m_Titles.size()) )
		{
			bBreak = true;
			break;
		}
		iPos += (len+1);
	}	
	if( !bBreak )
	{
		int len = strlen(ss) - iPos - 1;
		if( len > 0 )
			semMeaning.m_values.push_back(std::string(ss+iPos, len));
		else
			semMeaning.m_values.push_back("");	
	}

	map<SLemma,SSemInfo>::iterator it = m_Entries.find(lemma);
	if( it == m_Entries.end() )
		m_Entries[lemma] = SSemInfo(semMeaning);
	else
		it->second.m_Meanings.push_back(semMeaning);
}

std::string CSemanticMarkup::GetSemFeaturesString(std::string& lemma, BYTE POS)
{
	std::string res;
	map<SLemma,SSemInfo>::iterator it = m_Entries.find(SLemma(lemma.c_str(),POS));
	if( it == m_Entries.end() )
		return "";
	for( int i = 0 ; i < it->second.m_Meanings.size() ; i++ )
	{	
		for( int j = 0 ; j < it->second.m_Meanings[i].m_values.size() ; j++ )
		{
			if( it->second.m_Meanings[i].m_values[j].length() == 0 )
				continue;
			res += m_Titles[j];
			res += "=";
			res += '\"';
			res += it->second.m_Meanings[i].m_values[j];
			res += "\" ";
		}
		if( i < it->second.m_Meanings.size() - 1)
			res += "| ";
	}
	return res;
}