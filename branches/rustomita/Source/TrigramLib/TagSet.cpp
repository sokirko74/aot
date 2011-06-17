// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "TagSet.h"
#ifdef  USE_TRIGRAM_LEMMATIZER

#include "../AgramtabLib/RusGramTab.h"



// ==================   struct CTag  =================

CTag::CTag() {
	m_Pos = rPartOfSpeechCount;
	m_Grammems = 0;
};

bool CTag::operator < (const CTag& T) const {
	if (m_Pos != T.m_Pos)
		return m_Pos < T.m_Pos;

	if (m_Grammems != T.m_Grammems)
		return m_Grammems < T.m_Grammems;

	return m_Lemmas < T.m_Lemmas;
};

bool CTag::IsEmptyTag() const
{
	return		m_Lemmas.empty() 
			&&	m_Pos == rPartOfSpeechCount 
			&&	m_Grammems == 0;
};


// ==================   class CTagSet  =================

CTagSet::CTagSet(void)
{
}

CTagSet::~CTagSet(void)
{

}



// m_Name is a mystem description 
// m_Pos and m_Grammems- Dialing description 
// 
/*
bool CTagSet::ReadMyStemTagSet(string FileName, const CAgramtab* pAgramtab)
{
	FILE * fp = fopen(FileName.c_str(), "r");
	if (!fp)
	{
		fprintf (stderr, "cannot open %s\n", FileName.c_str());
		return false;
	}
	
	char buffer[1000];
	while (fgets(buffer, 1000, fp))
	{
		string line = buffer;
		Trim(line);
		if (line.empty()) continue;
		
		StringTokenizer tok(line.c_str(), "_");
		string Pos;
		string Grammems;
		while (tok())
		{
			string s = tok.val();
			string pos = POS_MyStem2Dialing(s);
			if (!pos.empty())
			{
				Pos  = pos;
			}
			else
			{
				if (!IsMyStemSpecificGrammem(s))
				{
					string g = Grammem_MyStem2Dialing(s);
					if (!g.empty())
						Grammems += g + ",";
				}
			}

		}
		string pattern = Pos + " " + Grammems;
		CTag T;
		T.m_Name = line;
		if (!pAgramtab->ProcessPOSAndGrammems(pattern.c_str(), T.m_Pos, T.m_Grammems))
		{
			if (ispunct(T.m_Name[0]) )
			{
				m_Tags[T.m_Name] = T;
				continue;
			}
			else
			{
				fprintf (stderr, "cannot parse %s\n", line.c_str());
				continue;
			}
		}
		if (T.IsEmptyTag())
		{
				fprintf (stderr, "empty tag in %s\n", line.c_str());
				return false;
		};
		size_t i_size = m_Tags.size();
		m_Tags[T.m_Name] = T;
		if (m_Tags.size() == i_size)
		{
			fprintf (stderr, "a dublicate tag name \"%s\" is found\n", line.c_str());
			return false;
		};

	}
	fclose (fp);
	return true;
}
*/


bool CTagSet::ReadTagSet(string FileName, const CAgramtab* pAgramtab)
{
	m_Tags.clear();
	FILE * fp = fopen(FileName.c_str(), "r");
	if (!fp)
	{
		string RmlFileName = GetRmlVariable();
		if (		!RmlFileName.empty()  )
		{
				if (		( RmlFileName[RmlFileName.length() - 1] != '/')
						&&	( RmlFileName[RmlFileName.length() - 1] != '\\')
					)
					RmlFileName += "/Dicts/Trigram/";
				RmlFileName += FileName;
				fp = fopen(RmlFileName.c_str(), "r");
		}
		if (!fp)
		{
			fprintf (stderr, "cannot open %s\n", FileName.c_str());
			return false;
		}
	}
	
	char buffer[1000];
    // <tagname>  /lemma1,lemma2.../? <morphological featutes>
    // пример:
    // tag1  /красивый,новый/ КР_ПРИЧАСТИЕ мр
    // tag2  КР_ПРИЧАСТИЕ мр
	while (fgets(buffer, 1000, fp))
	{
		string s = buffer;
		Trim(s);
		if (s.empty()) continue;
		CTag T;
		StringTokenizer tok(s.c_str(), "\t \n\r");
		tok();
		T.m_Name = tok.val();
		s = tok.get_rest();
		if (s[0] == '/')
		{
			int i = (int)s.find('/',1);
			if (i == string::npos)
			{
				fprintf (stderr, "cannot find the second '/' in  %s\n", s.c_str());
				return false;
			};
			string q = s.substr(1, i-1);
			s.erase(0, i+1);
			StringTokenizer tok(q.c_str(), ", ");
			while (tok())
			{
				string lemma  = tok.val();
				EngRusMakeLower(lemma);
				T.m_Lemmas.push_back(lemma);
			};
			if (T.m_Lemmas.empty())
			{
				fprintf (stderr, "No lemmas in %s\n", s.c_str());
				return false;
			};
			Trim(s);
		};
		

		if (!s.empty())
		{

			if (!pAgramtab->ProcessPOSAndGrammems(s.c_str(), T.m_Pos, T.m_Grammems))
			{
				fprintf (stderr, "cannot parse %s\n", s.c_str());
				return false;
			}
		}
		else
		{

		};

		if (T.IsEmptyTag())
		{
				fprintf (stderr, "empty tag in %s\n", s.c_str());
				return false;
		};
		size_t i_size = m_Tags.size();      
		m_Tags[T.m_Name] = T;
		if (m_Tags.size() == i_size)
		{
			fprintf (stderr, "a dublicate tag name \"%s\" is found\n", s.c_str());
			return false;
		};
	};

	fclose (fp);
	return true;
}



bool CTagSet::BuildDefaultTags(const CAgramtab* pAgramtab)
{
	m_Tags.clear();

	for (BYTE i=0; i <pAgramtab->GetPartOfSpeechesCount(); i++)
	{
		CTag T;
		T.m_Pos = i;
		T.m_Name = pAgramtab->GetPartOfSpeechStr(i);
		m_Tags[T.m_Name] = T;
	};
	return true;
}

BYTE GetUnionSize(const QWORD& d1, const QWORD& d2)
{
	BYTE Result = 0;
	for (long k=0;k < 64; k++)
		if (d1 & d2 & _QM(k))
			Result ++;
	return Result;
};

CTag CTagSet::GetBestSuitedTag (const CTag& T) const 
{
	CTag Best;
	size_t  MaxSize = 0;
	for (map<string, CTag>::const_iterator it = m_Tags.begin(); it != m_Tags.end(); it++)
	{ 
		const CTag&  P = it->second;
		bool bCheckLemma =  !T.m_Lemmas.empty() && find(P.m_Lemmas.begin(), P.m_Lemmas.end(), *T.m_Lemmas.begin()) != P.m_Lemmas.end();
		
		// игнорируем тэги, которые не совпали по частям речи
		if (bCheckLemma || P.m_Lemmas.empty())
		if  (		(P.m_Pos == T.m_Pos) 
				||	(		(P.m_Pos == rPartOfSpeechCount)
						&& !P.m_Lemmas.empty()
					)
			)
			if ( (P.m_Grammems & T.m_Grammems) == P.m_Grammems)
			{
				BYTE curr_size = GetUnionSize(T.m_Grammems, P.m_Grammems);
				if (!P.m_Lemmas.empty()) // если совпало со словоформой, тогда увеличить вес
					curr_size += 100;
				if (curr_size >= MaxSize) 
				{
					MaxSize = curr_size;
					Best = P;
				};
			}
	};
	return Best;
};



vector<CTag> CTagSet::DecipherTagStr (string TagStr, const CAgramtab* pAgramtab) const 
{
	map<string, CTag>::const_iterator tag_set_it = m_Tags.find(TagStr);
	vector<CTag> Tags;
	if (tag_set_it != m_Tags.end())
	{
		//fprintf (stderr, "\t tag %s is found\n",  TagStr.c_str());
		Tags.push_back( tag_set_it->second );
	}
	else
	{
		
		CTag T;
		if (pAgramtab->ProcessPOSAndGrammems(TagStr.c_str(), T.m_Pos, T.m_Grammems))
			// "С им,вн"
			Tags.push_back( T );
		else
		{
			// "МЕЖД,С"
			StringTokenizer tok(TagStr.c_str(), " ;,");
			while (tok())
			{
				CTag T;
				if (pAgramtab->ProcessPOSAndGrammems(tok.val(), T.m_Pos, T.m_Grammems))
					Tags.push_back( T );
			};
		};
	};
	return Tags;
};



 CTag CTagSet::GetTagFromAncode(const CAgramtab* pAgramtab, const char* Ancode, QWORD CommonGrammems, const string& Lemma) const
{
	CTag T;
	pAgramtab->GetGrammems(Ancode, T.m_Grammems);
	T.m_Grammems |= CommonGrammems;
	T.m_Pos = pAgramtab->GetPartOfSpeech(Ancode);
	T.m_Lemmas.push_back(Lemma);
	return GetBestSuitedTag(T);
}

#endif
