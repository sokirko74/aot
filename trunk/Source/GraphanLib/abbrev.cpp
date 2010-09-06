// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphmatFile.h"
#include "abbrev.h"
#include "GraphanDicts.h"
#include "../common/util_classes.h"

struct CStrToCompare {
	MorphLanguageEnum	m_Language;
	const char*			m_Str;
	size_t				m_StrLen;

	CStrToCompare(MorphLanguageEnum	 Language, const char* Str, size_t StrLen)
	{
		m_Str = Str;
		m_StrLen = StrLen;
		m_Language = Language;
	};

	CStrToCompare(const CGraphmatFile* pParent, size_t LineNo, bool bCaseSensitive)
	{
		if (bCaseSensitive)
			m_Str = pParent->GetUnits()[LineNo].GetToken();
		else
			m_Str = pParent->GetUppercaseToken(LineNo);
			
		
		m_StrLen = pParent->GetUnits()[LineNo].GetTokenLength();
		m_Language = pParent->m_Language;
	};

};

bool CAbbrevItem::operator == (const CAbbrevItem X) const
{
	return		(m_ItemStr == X.m_ItemStr);
};


bool CAbbrevItem::operator < (const CAbbrevItem X) const
{
	return m_ItemStr < X.m_ItemStr;
};

inline bool AbbrevIsEqualToString (const CAbbrevItem& X, const CStrToCompare& Str) 
{
	switch (X.m_Type)
	{
		case abString: return (X.m_ItemStr.length() == Str.m_StrLen) && !strncmp(X.m_ItemStr.c_str(), Str.m_Str, Str.m_StrLen);
		case abNumber: return (Str.m_StrLen > 0)&& isdigit((BYTE)Str.m_Str[0]);
		case abUpperCase: return (Str.m_StrLen > 0)&& (is_upper_alpha((BYTE)Str.m_Str[0], Str.m_Language) || is_upper_alpha((BYTE)Str.m_Str[0], Str.m_Language));
		case abAny: return (Str.m_StrLen > 0);
	};
	assert (false);
	return true;
};

bool AbbrevIsGreaterThanString (const CAbbrevItem& X, const CStrToCompare& Str) 
{
	size_t min_len = (X.m_ItemStr.length()<Str.m_StrLen) ? X.m_ItemStr.length() :  Str.m_StrLen;

	int t  = strncmp(X.m_ItemStr.c_str(), Str.m_Str, min_len);

	return		(t > 0) 
			||	(		(t == 0)
					&&	(X.m_ItemStr.length() > Str.m_StrLen)
				);
};


const char* NumberPlace = "/:D";

static bool ReadAbbrevationsFromOneFile(string FileName, vector<CAbbrev>& V, MorphLanguageEnum langua)
{
    FILE* fp = fopen(FileName.c_str(), "rb");
	if (!fp) return false;

    char buffer[AbbrevSize];
	while (fgets (buffer, AbbrevSize, fp))
    {   
		string s = buffer;	
        
		{
			int index = s.find("//");
			if (index != string::npos)
				s.erase(index);
		};
		Trim(s);
		if (s.empty()) continue;
		RmlMakeUpper(s, langua);

		CAbbrev Abbrev;

		StringTokenizer tok(s.c_str(), " ");
		while (tok())
		{
			CAbbrevItem item;

			item.m_ItemStr = tok.val();;
			
			if (item.m_ItemStr ==  NumberPlace) 
				item.m_Type = abNumber;
			else
			if (item.m_ItemStr ==  "/:U") 
				item.m_Type = abUpperCase;
			else
			if (item.m_ItemStr ==  "/:A") 
				item.m_Type = abAny;
			else
				item.m_Type = abString;

			Abbrev.push_back(item);
		};
	    V.push_back (Abbrev);
    };   
    fclose (fp);
	return true;
};



bool CGraphanDicts::ReadAbbrevations()
{
	string FileName = GetRegistryString("Software\\Dialing\\Graphan\\AbbrFile");

	m_Abbrevs.clear();

	// English abbreviation are common part
	ReadAbbrevationsFromOneFile(MakeFName(FileName,"eng"),m_Abbrevs, m_Language);

	if (m_Language == morphGerman)
		ReadAbbrevationsFromOneFile(MakeFName(FileName,"ger"),m_Abbrevs, m_Language);
	else
		ReadAbbrevationsFromOneFile(MakeFName(FileName,"rus"),m_Abbrevs, m_Language);

	

	//  sorting in the reverse order, in orger  to larger sequences
	// go to the top, for example:
	// a b c
	// a b 
	// a
	{
		sort (m_Abbrevs.begin(),m_Abbrevs.end());
		vector<CAbbrev>::iterator it = unique(m_Abbrevs.begin(), m_Abbrevs.end());
		m_Abbrevs.erase(it, m_Abbrevs.end());
		reverse(m_Abbrevs.begin(),m_Abbrevs.end());
	}


	


//	FILE * fp = fopen("abbrev.log","w");
//	if (fp)
//	{
//		for (size_t i=0; i < m_Abbrevs.size(); i++)
//		{
//			for (CAbbrev::const_iterator it = m_Abbrevs[i].begin(); it != m_Abbrevs[i].end(); it++)
//			{
//				fprintf(fp, "%s ", it->m_ItemStr.c_str());
//			};
//			fprintf(fp, "\n");
//		};
//		fclose (fp);
//	}
	    
	return true;
};


bool CGraphanDicts::ReadKeyboard(string FileName)
{
  m_Keys.clear();
  m_KeyModifiers.clear();
  FILE* fp = fopen((const char*)FileName.c_str(), "r");

  if (fp ==0 ) return true;
  char s[100];

  fgets (s, 100, fp);
  rtrim(s);
  const char* q = s+strspn(s, " \t");
  if (strcmp (q, "[modifiers]")) return false;

  bool ModifiersSect = true;
  while (fgets (s, 100, fp))
  {   
           rtrim (s);
           if (s[0] == 0) continue;
		   

		   char* q = s + strspn(s, " \t");
		   if (!strcmp (q, "[keys]") )
		   {
			   ModifiersSect = false;
			   continue;
		   };
		   
		   if (ModifiersSect) 
		   {
			   m_KeyModifiers.push_back(q);
		   }
		   else
		   {
			   RmlMakeUpper(s, m_Language);
			   m_Keys.push_back(q);
		   };
   };   
  fclose (fp);
  return true;
};


bool CGraphanDicts::ReadExtensions(string FileName)
{
  m_Extensions.clear();
  FILE* fp = fopen((const char*)FileName.c_str(), "r");

  if (fp ==0 ) return false;
  char s[100];
  while (fgets (s, 100, fp))
  {   
           rtrim (s);
           if (s[0] == 0) continue;
		   char* q = s + strspn(s, " \t");
		   EngRusMakeUpper(q);
		   m_Extensions.push_back(q);
   };   
  fclose (fp);
  return true;
};



typedef vector<CAbbrev>::const_iterator abbrev_t;

abbrev_t	abbrev_lower_bound(abbrev_t _First, abbrev_t _Last, const CStrToCompare& _Val)
{	
	size_t _Count = _Last - _First;
	for (; 0 < _Count; )
	{	
		size_t _Count2 = _Count / 2;
		abbrev_t _Mid = _First;
		std::advance(_Mid, _Count2);
		if (AbbrevIsGreaterThanString(*_Mid->begin(), _Val))
		{
			_Mid++;
			_First = _Mid;
			_Count -= _Count2 + 1;
		}
		else
			_Count = _Count2;
	}
	return _First;
}

bool CGraphmatFile::DealAbbrev (size_t  StartPos, size_t EndPos)
{
	//  no intersection  of abbreviations is possible, since 
	//  there is no way to distinct abbrevations aftrewards using fork graphematical 
	//  descriptors, if they have an intersection

	if (GetUnits()[StartPos].IsAbbreviation()) return false;
	if (GetUnits()[StartPos].IsSoft()) return false;

	// search for the first word of the abbreviation
	vector<CAbbrev>::const_iterator AbbrevIt;

	CStrToCompare FirstLine(this, StartPos, false);
	
	if (HasDescr(StartPos, ODigits))
	{
		// only numbers and strings  can be at the beginning of an abbreviations
		FirstLine.m_Str  = NumberPlace;
		FirstLine.m_StrLen = strlen(NumberPlace);
	};

	AbbrevIt = abbrev_lower_bound (m_pDicts->m_Abbrevs.begin(), m_pDicts->m_Abbrevs.end(), FirstLine);

	if (		(AbbrevIt == m_pDicts->m_Abbrevs.end())
			||	!AbbrevIsEqualToString(*AbbrevIt->begin(),  CStrToCompare (this, StartPos, false))
		)
		return false;

	vector<CAbbrev>::const_iterator StartAbbrevIt = AbbrevIt;

	while	(		AbbrevIt != m_pDicts->m_Abbrevs.end() 
				&& (*AbbrevIt->begin() == *StartAbbrevIt->begin())
			)
	{
		// the first word was checked
		size_t EndAbbrevPos = StartPos + 1;

		// check current abbreviation (AbbrevIt)
		CAbbrev::const_iterator it = AbbrevIt->begin();
		for (it++; it != AbbrevIt->end(); it++)
		{
			EndAbbrevPos = PSoft(EndAbbrevPos, EndPos);

			if ( EndAbbrevPos == EndPos ) break;
			// saving the last hard part of the abbreviation

			// checking current abbreviation item 
			if 	(!AbbrevIsEqualToString(*it,  CStrToCompare(this, EndAbbrevPos, it->m_Type==abUpperCase)))
				break;
			EndAbbrevPos++;
		};

		if (it  == AbbrevIt->end())
		{
			// found an abbreviation
			SetDes (StartPos, OAbbr1);
			SetDes (EndAbbrevPos-1, OAbbr2);
			SetState(StartPos,EndAbbrevPos,stAbbreviation);
			return true;
		}

		AbbrevIt++;
	};
	return false;
};




