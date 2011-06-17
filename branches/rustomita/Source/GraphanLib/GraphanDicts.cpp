// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphanDicts.h"


#define RD_MODE    (unsigned) 'r'
#define WR_MODE    (unsigned) 'w'

FILE* MOpen (const char *FName, int Mode)
{
	FILE *fp;
	while (isspace (*FName)) FName ++;
	size_t l = strlen (FName);
	if ((l == 0) || (l > 255))
	  throw CExpc ("Cannot read file %s", FName);

	if (Mode == RD_MODE )
	 {
		fp = fopen (FName,"rb");
		if (!fp) throw CExpc ("Cannot read file %s", FName);
		else return (fp);
	 }

	if (Mode == WR_MODE)
	 {
     fp = fopen(FName,"wb");
	 if (!fp) throw CExpc ("Cannot write file %s", FName );
		else return (fp);
     return fp;
    }
  return NULL;
}


CGraphanDicts::CGraphanDicts(MorphLanguageEnum Language)
{
	m_Language = Language;
}

CGraphanDicts::~CGraphanDicts()
{
	m_pOborDic.FreePointer();
};

void CGraphanDicts ::FreeData()
{
	m_pOborDic.FreePointer();
	m_EnglishNames.clear();
	m_Oborottos.clear();
	m_Abbrevs.clear();
};

bool CGraphanDicts:: ReadENames (string FileName)
{
	assert (m_Language != morphUnknown);
   FILE* EnamesFp = MOpen (FileName.c_str(),RD_MODE);
   char s[MaxNameSize];
   while (fgets (s, MaxNameSize, EnamesFp))
   {
	   
	   while (    (strlen (s) > 0)
		       && isspace ((unsigned char)s[strlen(s) -1])
			 )
	   s[strlen(s) -1] = 0;

	   if (strlen (s) == 0) continue;

       CEnglishName N;
	   strcpy  (N.name, s);

	   RmlMakeUpper(N.name, m_Language);

	   m_EnglishNames.push_back(N);
   };

   sort (m_EnglishNames.begin(),m_EnglishNames.end(), EnglishNameLess());
      
   fclose (EnamesFp);  

   return true;
};



bool CGraphanDicts:: ReadIdents (string FileName)
{
	assert (m_Language != morphUnknown);

	for (int i =0; i< 256;  i++)
		m_Idents[i].clear();
	
	FILE* EnamesFp = MOpen (FileName.c_str(),RD_MODE);
	char s[MaxNameSize];

	while (fgets (s, MaxNameSize, EnamesFp))
	{
		string q = s;
		Trim(q);
		if (q.empty()) continue;
		m_Idents[(unsigned char)q[0]].push_back(q);
		m_Idents[(unsigned char)ReverseChar((unsigned char)q[0],m_Language)].push_back(q);
	};
 
	fclose (EnamesFp);  // space.dic

	return true;
};


bool CGraphanDicts:: FindInIdents (const char* s, BYTE& len) const
{
	const StringVector& Idents = m_Idents[(unsigned char)s[0]];

	for (size_t i=0; i  < Idents.size(); i++)
		if (CompareWithoutRegister ((const char*)s+1, Idents[i].c_str()+1, Idents[i].length()-1, morphEnglish) == 0)
		{
			len = Idents[i].length();
			return true;
		};

	return false;
};

bool CGraphanDicts:: ReadSpaces (string FileName)
{
	assert (m_Language != morphUnknown);

	FILE* fp = MOpen (FileName.c_str(), RD_MODE);  

	m_Spaces.clear();


	char s[1000];
	while ( fgets (s, 1000, fp) )
	{
		CSpacedWord S;
		rtrim (s);
		strcpy (S.m_SpacedWord, s + strspn (s, " \t"));
		S.m_SpacedWordLen = strlen (S.m_SpacedWord);
		m_Spaces.push_back(S);
	};

	fclose( fp );

	return true;
}



bool CGraphanDicts:: IsExtension(const char * UpperStr, BYTE Len) const
{
  long i=0;
  for (; i < m_Extensions.size(); i++)
	 if (   (m_Extensions[i].length() == Len)
		 && !strncmp (m_Extensions[i].c_str(), UpperStr,  Len)
		)
		break;

  return  i != m_Extensions.size();
};


bool CGraphanDicts::IsRegisteredKeyModifier(const char* Str, size_t Len) const
{
  for (long i=0; i < m_KeyModifiers.size(); i++)
	 if (   (m_KeyModifiers[i].length() == Len)
		 && !strncmp (m_KeyModifiers[i].c_str(), Str,  Len)
		)
		return true;

  return  false;
};


bool StrSpacingCompare (const char* Sample,  const char* Text, size_t SampleLength, size_t& TextLength, MorphLanguageEnum langua)
{
	int spacing = 1;

	// после первой буквы должна идти  разрядка
	if ( !is_spc_fill((BYTE) Text[spacing] ) ) return false;

	//  разрядка может быть в два пробела 
	if ( is_spc_fill((BYTE) Text[spacing+1] )  ) spacing++;

	// Разрядка не может быть  больше двух пробелов
	if ( is_spc_fill((BYTE) Text[spacing+1] )  ) return false;

	// пошла проверка слов
	for (int i=0; i<SampleLength; i++)
	{ 
		if (!IsSuperEqualChar((BYTE) Sample[i], (BYTE) Text[(spacing+1)*i], langua) )
			return false;
	}

	TextLength = spacing*(SampleLength-1) + 1;

	return true;

};




const char * CGraphanDicts ::SearchSpace (const char *In ,int *len ) const
{

	size_t length;

	for (size_t i=0;i<m_Spaces.size();i++)
		if (StrSpacingCompare (m_Spaces[i].m_SpacedWord,In,m_Spaces[i].m_SpacedWordLen, length, m_Language))
		{
			*len = length;
			return m_Spaces[i].m_SpacedWord;
		};

	return NULL;
}

