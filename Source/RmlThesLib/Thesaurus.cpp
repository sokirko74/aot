// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#include "StdRmlThes.h"
#pragma warning (disable:4786)
#include "Thesaurus.h"

/////////////////////////////////////////////////////////////////////////////
// CThesaurus
template<class _II, class _Ty> inline
bool _find(_II It, const _Ty& _V)
{
	  return !(find(It.begin(), It.end(), _V) ==  It.end());
}


CThesaurus::CThesaurus()
{
	m_pOborDic = 0;
	m_pMainGramTab = 0;
	m_pEngGramTab = 0;
	m_MainLanguage = morphUnknown;
}

CThesaurus::~CThesaurus()
{

}

bool CThesaurus::ReadThesaurusFromDisk()
{
	if (m_bDontLoad) return true;

	if (!m_pMainGramTab) return false;
	if (!m_pEngGramTab) return false;
	if (m_MainLanguage == morphUnknown) return false;
	
	if (!LoadModels (m_Directory+"/StatThes/FreqCollocTypes.txt"))
	{
		ErrorMessage ("Cannot Load models " +m_Directory+"/StatThes/FreqCollocTypes.txt");
		return false;
	};
	
	string OborotsFileName = (m_MainLanguage == morphGerman) ?  
				m_Directory+"/StatThes/GerOborots.txt" : m_Directory+"/StatThes/RusOborots.txt";
	if (!LoadOborots (OborotsFileName))
	{
		ErrorMessage ("Cannot Load oborots " + OborotsFileName);
		return false;

	};


	if (!LoadTermins (m_Directory+"/StatThes/TextEntr.txt"))
	{
		ErrorMessage ("Cannot Load text entries " +m_Directory+"/StatThes/TextEntr.txt");
		return false;

	};

	if (!LoadSynItems (m_Directory+"/StatThes/Lemmas.txt"))
	{
		ErrorMessage ("Cannot Load lemmas " +m_Directory+"/StatThes/Lemmas.txt");
		return false;

	};

	if (!LoadSynonyms (m_Directory+"/StatThes/Synonyms.txt"))
	{
		ErrorMessage ("Cannot Load text entries " +m_Directory+"/StatThes/Synonyms.txt");
		return false;

	};


	return true;
}




bool CThesaurus::ReadRelationsFromDisk()
{
	if (m_bDontLoad) return true;

	if (!LoadConcepts (m_Directory+"/StatThes/Concepts.txt"))
	{
		ErrorMessage ("Cannot Load Concepts " +m_Directory+"/StatThes/Concepts.txt");
		return false;

	};
	if (!LoadRelats (m_Directory+"/StatThes/Relats.txt"))
	{
		ErrorMessage ("Cannot Load Relats " +m_Directory+"/StatThes/Relats.txt");
		return false;

	};
	return true;
}

bool CThesaurus::SetDatabase(string DatabaseName)
{
	m_bDontLoad = false;
	m_Name = DatabaseName;
	string KeyName = "Software\\Dialing\\"+m_Name+"\\DictPath";
	try {
	  m_Directory = GetRegistryString(KeyName);
	}
	catch (...)
	{
		ErrorMessage ("Cannot open" + m_Name);
        return false;
	};

	try {
	  string KeyName = "Software\\Dialing\\Thesauri\\IgnoreThesauri";
	  if (CanGetRegistryString(KeyName))
	  {
		  string Value =  GetRegistryString(KeyName);
		  m_bDontLoad =   (Value == "yes");
	  }
	}
	catch (...)
	{
	};
	
	return true;
}




struct IsLessByConcept1{
	
	bool  operator() (const CRelat& _X1, const int& ConceptNo) const
	{
			return _X1.m_Concept1No < ConceptNo;
	};
	bool  operator() (const int& ConceptNo, const CRelat& _X1 ) const
	{
			return ConceptNo < _X1.m_Concept1No;
	};
	bool  operator() (const CRelat& _X1, const CRelat& _X2) const
	{
		return _X1.m_Concept1No < _X2.m_Concept1No;
	};
};



static long GetRelationsCount(const CThesaurus& T, int ConceptNo, RelationEnum RelationId) 
{
	long Result = 0;
	for (
		vector<CRelat>::const_iterator It = lower_bound(T.m_Relats.begin(), T.m_Relats.end(),ConceptNo, IsLessByConcept1());
	  !(It == T.m_Relats.end()) && (It->m_Concept1No==ConceptNo);
	  It++
	 )
	 if (It->m_RelationType == RelationId)
		 Result++;
	 return Result;

};


bool CThesaurus::GetTree(int ConceptNo, vector<long>& TreeConcepts,  vector<long> CurrPath, RelationEnum RelationType) const
{

	for (long i=0; i <CurrPath.size(); i++)
		if (CurrPath[i] == ConceptNo)  return false;

	CurrPath.push_back(ConceptNo);

	for (
		vector<CRelat>::const_iterator It = lower_bound(m_Relats.begin(), m_Relats.end(),ConceptNo, IsLessByConcept1());
		!(It == m_Relats.end()) && (It->m_Concept1No==ConceptNo);
		It++
		)
		if (It->m_RelationType == RelationType)
		{
			if (!GetTree (It->m_Concept2No, TreeConcepts,CurrPath, RelationType)) return false;
			TreeConcepts.push_back(It->m_Concept2No);
		};

	return true;
};


bool CThesaurus::IsA(DWORD TextEntryId, string ConceptStr) const
{
	long ConceptNo = GetConceptNoByConceptStr(ConceptStr.c_str());	
	if (ConceptNo == -1) return   false;
	long ConceptId = m_Concepts[ConceptNo].m_ConceptId;

	vector<long> ConceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, ConceptsFromTextEntry);
	if (ConceptsFromTextEntry.empty()) return true;

	vector<long> HIGHERConcepts;
	vector<long> Path;
	for (long i=0; i < ConceptsFromTextEntry.size();i++)
	{
		HIGHERConcepts.clear();
		if (!GetTree(GetConceptNoByConceptId(ConceptsFromTextEntry[i]), HIGHERConcepts, Path, HIGHER))
		{
			ErrorMessage ("a cycle found starts from TextEntry " + m_Termins[GetTerminNoByTextEntryId(TextEntryId)].m_TerminStr);
			return false;
		}

		for (long k=0;  k <HIGHERConcepts.size();k++)
			if (m_Concepts[HIGHERConcepts[k]].m_ConceptId == ConceptId) 
				return true;
	};

	return false;
}

bool IsEnglishModel (const CInnerModel& M)
{
  for (long i =0; i <M.m_AtomGroups.size(); i++)
   if (M.m_AtomGroups[i].m_LanguageId != morphEnglish)
     return false;

  return true;

};

void CThesaurus::QueryEnglishTranslations(DWORD TextEntryId, vector<int>& CurrentEnglishTermins) const
{
	CurrentEnglishTermins.clear();
	vector<long> ConceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, ConceptsFromTextEntry);
	if (ConceptsFromTextEntry.empty()) return;

	for (long i=0; i <m_SynonymsByConcept.size();i++)
	{
		bool Found = false;
		long k=0;
		for (; k <ConceptsFromTextEntry.size();k++)
			if (m_SynonymsByConcept[i].m_ConceptId == ConceptsFromTextEntry[k])
			{
				Found = true;
				break;
			};

		if  (Found)
		{
			k = GetTerminNoByTextEntryId(m_SynonymsByConcept[i].m_TextEntryId);
			if (k != -1) 
			{
				if (m_Termins[k].m_ModelNo !=-1)
				if (IsEnglishModel(m_Models[m_Termins[k].m_ModelNo]))
				CurrentEnglishTermins.push_back(k);

				string s = m_Termins[k].m_TerminStr;	
				long j=0;
				for (; j < s.length(); j++)
					if (		!is_english_alpha((BYTE)s[j])
							&&	!isdigit((BYTE)s[j])
							&&	!ispunct((BYTE)s[j])
							&&	!isspace((BYTE)s[j])
					)
					break;

				if (j == s.length())
				if (!_find(CurrentEnglishTermins, k))
				CurrentEnglishTermins.push_back(k);

			};
		};
	}

}



int CThesaurus::GetTerminIdBySingleWord(string WordStr) const
{
	CInnerSynItem I;
    I.m_ItemStr = WordStr;

	EngRusMakeUpper(I.m_ItemStr);

	for (vector<CInnerSynItem>::const_iterator It = lower_bound (m_SynItems.begin(), m_SynItems.end(),I);
	     (It < m_SynItems.end()) && (It->m_ItemStr == I.m_ItemStr);
		 It++)
      if (m_Termins[It->m_TerminNo].m_ModelNo != -1)
		  if (m_Models[m_Termins[It->m_TerminNo].m_ModelNo].m_AtomGroups.size() == 1)
		  {
			  return  m_Termins[It->m_TerminNo].m_TerminId;

		  };

	return -1;
}

int CThesaurus::QueryTopConcepts(UINT TextEntryId, vector<int>& CurrentTopConcepts) const
{
  CurrentTopConcepts.clear();
  vector<long> ConceptsFromTextEntry;
  GetConceptsByTextEntryId(TextEntryId, ConceptsFromTextEntry);
  if (ConceptsFromTextEntry.empty()) return 0;

  for (long i=0; i <ConceptsFromTextEntry.size(); i++)
  {
	vector<long> HIGHERConcepts;
	vector<long> Path;

    if (!GetTree(GetConceptNoByConceptId(ConceptsFromTextEntry[i]), HIGHERConcepts, Path, HIGHER))
    {
       ErrorMessage ("a cycle found starts from TextEntry " + m_Termins[GetTerminNoByTextEntryId(TextEntryId)].m_TerminStr);
       return 0;
    };


   for (long k=0; k < HIGHERConcepts.size(); k++)
    if (GetRelationsCount(*this, HIGHERConcepts[k], HIGHER) == 0) 
		if (!_find(CurrentTopConcepts,HIGHERConcepts[k])) 
          CurrentTopConcepts.push_back(HIGHERConcepts[k]);
  };

  return CurrentTopConcepts.size();
}



void  CThesaurus::QueryHigherTermins(UINT TextEntryId, vector<int>& CurrentHigherTermins) const
{
	CurrentHigherTermins.clear();
	long TerminNo = GetTerminNoByTextEntryId(TextEntryId);
	if (TerminNo == -1) return;
	if (m_Termins[TerminNo].m_ModelNo == -1) return;
	bool IsEnglish =   IsEnglishModel(m_Models[m_Termins[TerminNo].m_ModelNo]);
 

	vector<long> ConceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, ConceptsFromTextEntry);
	if (ConceptsFromTextEntry.empty()) return;
	vector<long> HigherConcepts;
	long i=0;
	for (; i <ConceptsFromTextEntry.size(); i++)
	{
		vector<long> HIGHERConcepts;
		vector<long> Path;

		if (!GetTree(GetConceptNoByConceptId(ConceptsFromTextEntry[i]), HIGHERConcepts, Path, HIGHER))
		{
			ErrorMessage ("a cycle found starts from TextEntry " + m_Termins[GetTerminNoByTextEntryId(TextEntryId)].m_TerminStr);
			return;
		};


		for (long k=0; k < HIGHERConcepts.size(); k++)
			if ( !_find(HigherConcepts, HIGHERConcepts[k]) )
				HigherConcepts.push_back(HIGHERConcepts[k]);
	};

	for (i=0; i <HigherConcepts.size(); i++)
	{
	  vector<long> TextEntries;
	  GetTextEntriesByConcept(m_Concepts[HigherConcepts[i]].m_ConceptId, TextEntries);
	  for (long k=0; k < TextEntries.size(); k++)
	  {
		  long TerminNo = GetTerminNoByTextEntryId(TextEntries[k]);
		  if (m_Termins[TerminNo].m_ModelNo == -1) continue;
		  if (IsEnglish != IsEnglishModel(m_Models[m_Termins[TerminNo].m_ModelNo])) continue;
		  if (!_find (CurrentHigherTermins, TerminNo))
			  CurrentHigherTermins.push_back(TerminNo);
	  };
	};

}


DWORD CThesaurus::QueryTerminItem(const string& ItemStr, vector<int>& CurrentTerminItems) const
{
	CurrentTerminItems.clear();
	CInnerSynItem I;
	I.m_ItemStr = ItemStr;
	for (vector<CInnerSynItem>::const_iterator It = lower_bound (m_SynItems.begin(), m_SynItems.end(),I);
	     (It < m_SynItems.end()) && (It->m_ItemStr == I.m_ItemStr);
		 It++)
	     CurrentTerminItems.push_back(It - m_SynItems.begin());

	return  CurrentTerminItems.size();
}



void CThesaurus::QueryLowerTermins(const string& ConceptStr, UINT Language, vector<int>& CurrentLowerTermins) const
{
	CurrentLowerTermins.clear();
	long ConceptNo = GetConceptNoByConceptStr(ConceptStr);	
	if (ConceptNo == -1) return;
	vector<long> LOWERConcepts;
	vector<long> Path;
	if (!GetTree(ConceptNo, LOWERConcepts, Path, LOWER))
		  return;

	bool IsEnglish =  Language == morphEnglish;
	for (long i=0; i <LOWERConcepts.size(); i++)
	{
	  vector<long> TextEntries;
	  GetTextEntriesByConcept(m_Concepts[LOWERConcepts[i]].m_ConceptId, TextEntries);
	  for (long k=0; k < TextEntries.size(); k++)
	  {
		  long TerminNo = GetTerminNoByTextEntryId(TextEntries[k]);
		  if (m_Termins[TerminNo].m_ModelNo == -1) continue;
		  if (IsEnglish != IsEnglishModel(m_Models[m_Termins[TerminNo].m_ModelNo])) continue;
		  if (!_find (CurrentLowerTermins, TerminNo))
			  CurrentLowerTermins.push_back(TerminNo);
	  };
	};

}


int CThesaurus::FindAbbr(const string& str) const
{
	String2Int::const_iterator It = m_AbbrForms2TerminNo.find(str);
	if (It != m_AbbrForms2TerminNo.end())
		return  It->second;
	else
		return  -1;
}


bool CThesaurus::LoadSynItems(string FileName)
{
	m_SynItems.clear();
	FILE* fp = fopen (FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if ( !fgets (buff, 2000, fp) )
		return false;
	string Header = buff;
	Trim(Header);
	if (Header != "TextEntryId;LemmaPos;LemmaStr;IsLemma;IsOb1;IsOb2;OborotId;UpLw;UpUp;IsPlural;")
	{
		fclose(fp);
		return false;
	}

    	long SaveTerminId = -1;
	long SaveTerminNo = -1; 
	while   (fgets(buff, 2000, fp))
	{ 
		int i = 0;
		CInnerSynItem I;
		long TerminId;
		long OborotId;

		for (char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			int len = strlen(s);			
			if (s[0] == '"')
			{
				if (s[len - 1] != '"')
				{
					fclose(fp);
					return false;
				}
			   s[len-1] = ' ';
			   s++;
			   
			};
			string Field =  s;
			if (i==0)
				TerminId = atoi(Field.c_str());
	            else
			if (i==1)
				I.m_ItemPos = atoi(Field.c_str());
      	      else
			if (i==2)
			{
				I.m_ItemStr = s;
				Trim(I.m_ItemStr);
			}
			else
			if (i==3)
				I.m_Flags |= (Field == "TRUE")? siLemma : 0;
			else
			if (i==4)
				I.m_Flags |=   (Field == "TRUE")? siOb1 : 0;
			else
			if (i==5)
			    I.m_Flags |=   (Field == "TRUE")? siOb2 : 0;
			else
			if (i==6)
				OborotId = atoi(Field.c_str());
			else
			if (i==7)
				I.m_Flags |=   (Field == "TRUE")?siUpLw : 0;
			else
			if (i==8)
				I.m_Flags |=   (Field == "TRUE") ? siUpUp : 0;
			else
			if (i==9)
				I.m_Flags |=   (Field == "TRUE") ? siPlural : 0;

	            i++;
		}
		I.m_OborotNo = ErrUnitNo;
		if (OborotId != -1)
		{
			vector<COborot>::const_iterator It = lower_bound(m_Oborots.begin(), m_Oborots.end(),COborot(OborotId));
			if  (  (It != m_Oborots.end())
				 &&(It->m_OborotId == OborotId)
				)
			I.m_OborotNo =  It - m_Oborots.begin();
		};
		long TerminNo ;
		if (TerminId == SaveTerminId)
			TerminNo = SaveTerminNo;
		else
			TerminNo = GetTerminNoByTextEntryId(TerminId);

        if (TerminNo != -1)
		{
		  I.m_TerminNo = TerminNo;
		  m_SynItems.push_back(I);
 		  SaveTerminNo = I.m_TerminNo;
		  SaveTerminId =  TerminId;
		};
	 };
	sort (m_SynItems.begin(), m_SynItems.end());

	long Count = m_SynItems.size();
	long i=0;
	try {
		for (i=0; i < Count; i++)
		{
			CInnerTermin& T = m_Termins[m_SynItems[i].m_TerminNo];
			long k=0;
			for (; k < T.m_Items.size(); k++)
				if (m_SynItems[T.m_Items[k]].m_ItemPos >  m_SynItems[i].m_ItemPos)
					break;
			T.m_Items.insert (T.m_Items.begin() + k, i);
		};
	}
	catch (...)
	{
		return false;
	}	
	fclose(fp);
	return true;
};


void	CThesaurus::SetDicts(MorphLanguageEnum MainLanguage, const CDictionary* OborDic, const CAgramtab* MainGramtab, const CAgramtab* EngGramtab)
{
	m_MainLanguage = MainLanguage;
	m_pOborDic = OborDic;
	m_pMainGramTab = MainGramtab;
	m_pEngGramTab = EngGramtab;
}

