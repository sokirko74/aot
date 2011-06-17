#include "StdGramtab.h"
#include "../common/util_classes.h"
// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko


#include "agramtab_.h"
//#include "assert.h"


static BYTE GetTagFromStr(const CAgramtab& A, const char* tab_str) 
{
    
    
    for (BYTE i = 0; i < A.GetPartOfSpeechesCount(); i++)
			if  (!strcmp(tab_str, A.GetPartOfSpeechStr(i)))
		  	 return i;
     
    return UnknownPartOfSpeech;
}



CAgramtabLine :: CAgramtabLine (size_t SourceLineNo) 
{
	m_SourceLineNo = SourceLineNo;
};


bool CAgramtab::GetGrammems(const char* gram_code, QWORD& grammems)  const
{
	grammems = 0;
	if (gram_code == 0) return false;
	if (!*gram_code) return false;
    if (gram_code[0] == '?') return false;

	const CAgramtabLine* L =  GetLine(s2i(gram_code));

	if (L == NULL)
		return 
        false;

	grammems = L->m_Grammems;
	return  true;
};

string   CAgramtab::GrammemsToStr(QWORD grammems) const 
{
	char szGrammems[64*5];
	grammems_to_str(grammems, szGrammems);
	return szGrammems;
}

//#pragma optimize( "", off )

bool CAgramtab :: ProcessPOSAndGrammems (const char* line_in_gramtab, BYTE& PartOfSpeech, QWORD& grammems)  const
{
	if (strlen(line_in_gramtab) > 300) return false;

	StringTokenizer tok(line_in_gramtab," ,\t\r\n");
	const char* strPos = tok();
	if  (!strPos) 
	{
		//printf ("unknown pos");		
		return false;
	};
		

	//  getting the part of speech
	if( strcmp("*", strPos) )
	{
		PartOfSpeech = GetTagFromStr(*this, strPos);
		if (PartOfSpeech == UnknownPartOfSpeech)
			return false;
	}
	else
		PartOfSpeech = UnknownPartOfSpeech;

	
	//  getting grammems
	grammems = 0; 
	while ( tok() )
	{
		
		size_t Count = GetGrammemsCount();
		const char* grm = tok.val();
		

		size_t  i = 0;
		for (; i < Count; i++)
			if (!strcmp(grm, GetGrammemStr(i)) )
			{
				grammems |=  _QM(i);
				break;
			};

		if  (i == Count) 
			return false;
	};

	return true;
};

//#pragma optimize( "", on )

bool  CAgramtab::ProcessPOSAndGrammemsIfCan (const char* tab_str, BYTE* PartOfSpeech,  QWORD* grammems) const
{
	return ProcessPOSAndGrammems(tab_str, *PartOfSpeech, *grammems);
};
   

static bool  ProcessAgramtabLine (CAgramtab& A, const char* tab_str,  size_t LineNo)
{
	const char* s = tab_str+strspn (tab_str, " ");
	s += strcspn (s, " ");
	s += strspn (s, " ");
	s += strcspn (s, " ");
	s += strspn (s, " ");;
	return A.ProcessPOSAndGrammems(s, A.GetLine(LineNo)->m_PartOfSpeech, A.GetLine(LineNo)->m_Grammems);
};

CAgramtab :: CAgramtab () 
{ 
	m_bInited = false;
};



bool CAgramtab :: Read (const char * FileName) 
{
	if (FileName ==  NULL) return true;

	for (size_t i=0; i<GetMaxGrmCount(); i++) 
		GetLine(i) = 0;


	FILE* fp = fopen (FileName,"r");

	assert (fp);

	if( !fp)
		return false;


	char buff[300];
	size_t LineNo = 0;;
	while (fgets (buff,300,fp))
	{
		LineNo++;
		char * s = buff;
		for (; isspace ( (unsigned char) *s); s++);
		if (!*s) continue;
		if (!strncmp (s,"//",2)) continue;

		CAgramtabLine* pAgramtabLine = new CAgramtabLine(LineNo);

		// a double can  occur
		if (GetLine(s2i(s))) delete GetLine(s2i(s));

		GetLine(s2i(s)) = pAgramtabLine;
		bool bResult = ProcessAgramtabLine(*this, buff,s2i(s));
	}

	fclose (fp);

	m_bInited = true;
	return true;
};


bool CAgramtab :: ReadAndCheck (const char * FileName) 
{
	for (size_t i=0; i<GetMaxGrmCount(); i++) 
		GetLine(i) = 0;


	FILE* fp = fopen (FileName,"r");
	if (!fp)
	{
		printf ("cannot open gram table %s", FileName);
		return false;
	};


	char buff[300];
	size_t LineNo = 0;
	while (fgets (buff,300,fp))
	{
		LineNo++;
		char * s = buff;
		for (; isspace ( (unsigned char) *s); s++);
		if (!*s) continue;
		if (!strncmp (s,"//",2)) continue;

		CAgramtabLine* pAgramtabLine = new CAgramtabLine(LineNo);

		char debug[200];
		strcpy (debug, s);
		if (GetLine(s2i(s)))  
		{
			printf ("a double found %s", debug);
			return false;
		};


		GetLine(s2i(s)) = pAgramtabLine;
		bool bResult = ProcessAgramtabLine(*this, buff,s2i(s));

		if (!bResult)
		{
			printf ("cannot process %s", debug);
			return false;
		};


		for (WORD i=0; i<GetMaxGrmCount(); i++) 
		if ( (GetLine(i) != NULL)  && (s2i(debug) != i))
		{
			QWORD g1 = GetLine(i)->m_Grammems;
			QWORD g2 = GetLine(s2i(debug))->m_Grammems;
			if( (g1 == g2) && (GetLine(i)->m_PartOfSpeech == GetLine(s2i(debug))->m_PartOfSpeech) )
			{
				printf ("a double found %s (%s)", debug, i2s(i).c_str());
				return false;
			};
		}

	}

	fclose (fp);
	return true;
};


bool CAgramtab ::GetPartOfSpeechAndGrammems(const BYTE* AnCodes, DWORD& Poses, QWORD& Grammems) const
{
	size_t len = strlen((const char*)AnCodes);
	if (len == 0) return false;

	// grammems
	Grammems = 0;
	Poses = 0;
	for (size_t l=0; l<len; l+=2)
	{
		const CAgramtabLine* L =   GetLine(s2i((const char*)AnCodes+l));

		if (L == 0) return false;

		Poses |=  (1<< L->m_PartOfSpeech);
		Grammems |= L->m_Grammems;
	};

	return true;
}


CAgramtab :: ~CAgramtab ()
 {

	
 };

int CAgramtab :: AreEqualPartOfSpeech (char *grm1, char* grm2) 
 {
   if ((grm1==0) && (grm2==0)) return 1;
   if ((grm1==0) && (grm2!=0)) return 0;
   if ((grm2==0) && (grm1!=0)) return 0;
   if (((unsigned char)grm1[0] == '?') || ((unsigned char)grm2[0] == '?')) return 0;
   return GetLine(s2i(grm1))->m_PartOfSpeech == GetLine(s2i(grm2))->m_PartOfSpeech;
 }



char* CAgramtab :: grammems_to_str (QWORD grammems, char* out_buf) const
{
	out_buf[0] = 0;
	size_t GrammemsCount = GetGrammemsCount();
	for (int i = GrammemsCount-1; i >=0; i--)
	 if (_QM(i) & grammems)
	 {
		 strcat (out_buf, GetGrammemStr(i));
		 strcat (out_buf, ",");
	 };	 
	return out_buf;
};


bool CAgramtab :: FindGrammems (const char* gram_codes, QWORD grammems) const
{
  for (size_t l=0; l<strlen(gram_codes); l+=2)
		if ( (GetLine(s2i(gram_codes+l))->m_Grammems & grammems) == grammems)
			return true;

  return false;	
};



bool CAgramtab::GetGramCodeByGrammemsAndPartofSpeechIfCan(BYTE Pos, QWORD grammems, string& gramcodes) const
{


	 for (WORD i=0; i<GetMaxGrmCount(); i++) 
	  if (GetLine(i) != NULL) 
	  {
		if( (GetLine(i)->m_Grammems == grammems) && (GetLine(i)->m_PartOfSpeech == Pos) )
		{
			gramcodes = i2s(i);
			return true;
		}
	  }
	return false; 
};


BYTE CAgramtab::GetPartOfSpeech(const char* gram_code) const
{
	if (gram_code == 0) return UnknownPartOfSpeech;
    if (*gram_code == 0) return UnknownPartOfSpeech;
	if (*gram_code == '?') return UnknownPartOfSpeech; 

	const CAgramtabLine* L =  GetLine(s2i(gram_code));

	if (L == NULL)
		return UnknownPartOfSpeech;

	return L->m_PartOfSpeech;
}

size_t CAgramtab::GetSourceLineNo(const char* gram_code) const
{
	if (gram_code == 0) return 0;

	if (!strcmp(gram_code, "??")) return 0; 

	const CAgramtabLine* L =  GetLine(s2i(gram_code));

	if (L == NULL)
		return 0;

	return L->m_SourceLineNo;
}


QWORD CAgramtab::GetAllGrammems(const char *gram_code) const
{
	if (gram_code == 0) return 0;
	if (!strcmp(gram_code, "??")) return 0; 

	size_t len = strlen (gram_code);
    
	QWORD grammems = 0;

	for (size_t l=0; l<len; l+=2)
	{
		QWORD G =   GetLine(s2i(gram_code+l))->m_Grammems;
		grammems |= G;
	};

	return grammems;
}





bool CAgramtab::LoadFromRegistry ()
{
	try
	{
		Read(::GetRegistryString(GetRegistryString()).c_str());
		return true;
	}
	catch (...)
	{
		return false;
	};
};

bool CAgramtab::LoadFromRegistryAndCheck ()
{
	try
	{
		return ReadAndCheck(::GetRegistryString(GetRegistryString()).c_str());
	}
	catch (...)
	{
		return false;
	};
};


BYTE CAgramtab::GetFirstPartOfSpeech(const DWORD poses) const
{
	BYTE Count = GetPartOfSpeechesCount();
	for (BYTE i = 0; i < Count; i++)
		if (( poses & (1 <<i )) != 0)
			return i;

	return Count;
};




string	CAgramtab::GetAllPossibleAncodes(BYTE pos, QWORD grammems)const
{
	string Result;
	for (WORD i=0; i<GetMaxGrmCount(); i++) 
		if (GetLine(i) != 0)
		{
			const CAgramtabLine* L =  GetLine(i);
			if (			(L->m_PartOfSpeech == pos)
					&&		((grammems & L->m_Grammems) == grammems)
				)
				Result += i2s(i);
		};
	return Result;
};

QWORD CAgramtab::Gleiche (GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const
{
	QWORD grammems = 0;
	if (!gram_codes1) return false;
	if (!gram_codes2) return false;
	if (!strcmp(gram_codes1, "??")) return false;
	if (!strcmp(gram_codes2, "??")) return false;
	size_t len1 = strlen(gram_codes1);
	size_t len2 = strlen(gram_codes2);
   	for (size_t l=0; l<len1; l+=2)
		for (size_t m=0; m<len2; m+=2)
		{
            const CAgramtabLine* l1 = GetLine(s2i(gram_codes1+l));
            const CAgramtabLine* l2 = GetLine(s2i(gram_codes2+m));
			if ( CompareFunc  (l1, l2) )  
                grammems |= (l1->m_Grammems & l2->m_Grammems);
		};

 	return grammems;
};




//  uses gleiche to compare ancodes from gram_codes1 with  ancodes gram_codes2
//  returns all ancodes from gram_codes1, which satisfy CompareFunc
string CAgramtab::GleicheAncode1 (GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const
{
	string Result;

	if (!gram_codes1) return "";
	if (!gram_codes2) return "";
	if (!strcmp(gram_codes1, "??")) return gram_codes2;
	if (!strcmp(gram_codes2, "??")) return gram_codes1;
	size_t len1 = strlen(gram_codes1);
	size_t len2 = strlen(gram_codes2);
	for (size_t l=0; l<len1; l+=2)
	{
        const CAgramtabLine* l1 = GetLine(s2i(gram_codes1+l));
   		for (size_t m=0; m<len2; m+=2)	
		{
            const CAgramtabLine* l2 = GetLine(s2i(gram_codes2+m));
			if ( CompareFunc  (l1, l2) )  
			{
				//printf ("%s[%i]=%c\n",gram_codes1,l,gram_codes1[l]);
				Result.append(gram_codes1+l,2);
				//Result += gram_codes1[l+1];
				break;
			};
		};
	};

 	return Result;
};

bool CAgramtab::Compare(GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2, QWORD& grammems) const
{
	grammems = 0;
	if (!gram_codes1) return false;
	if (!gram_codes2) return false;
	if (!strcmp(gram_codes1, "??")) return false;
	if (!strcmp(gram_codes2, "??")) return false;
	size_t len1 = strlen(gram_codes1);
	size_t len2 = strlen(gram_codes2);
   	for (size_t l=0; l<len1; l+=2)
		for (size_t m=0; m<len2; m+=2)
		{
            const CAgramtabLine* l1 = GetLine(s2i(gram_codes1+l));
            const CAgramtabLine* l2 = GetLine(s2i(gram_codes2+m));
			if ( CompareFunc  (l1, l2) )  
                grammems |= (l1->m_Grammems & l2->m_Grammems);
		};

 	return grammems > 0;
};




//  uses gleiche to compare ancodes from gram_codes1 with  ancodes gram_codes2
//  returns all ancodes from gram_codes1, which satisfy CompareFunc
bool CAgramtab::Compare(GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2, string& grammems) const
{
    grammems.clear();

	if (!gram_codes1) return "";
	if (!gram_codes2) return "";
	if (!strcmp(gram_codes1, "??"))
    {
        grammems.append(gram_codes2);
        return true;
    }
	if (!strcmp(gram_codes2, "??"))
    {
        grammems.append(gram_codes2);
        return true;
    }
	size_t len1 = strlen(gram_codes1);
	size_t len2 = strlen(gram_codes2);
	for (size_t l=0; l<len1; l+=2)
	{
        const CAgramtabLine* l1 = GetLine(s2i(gram_codes1+l));
   		for (size_t m=0; m<len2; m+=2)	
		{
            const CAgramtabLine* l2 = GetLine(s2i(gram_codes2+m));
			if ( CompareFunc  (l1, l2) )  
			{
				grammems.append(gram_codes1+l,2);
				break;
			};
		};
	};

    return !grammems.empty();
};

string CommonAncodeAssignFunction(const CAgramtab* pGramTab, const string& s1, const string& s2)
{
	string Result;
	size_t len1 = s1.length();
	size_t len2 = s2.length();
	for (size_t i=0; i<len1; i+=2)
		for (size_t k=0; k<len2; k+=2)
		{
			if	(		(s1[i] == s2[k])
					&&	(s1[i+1] == s2[k+1])
				)
			{
				Result += s1[i];
				Result += s1[i+1];
				break;
			};

		};

	return Result.c_str();
}

string  CAgramtab::GetTabStringByGramCode(const char* gram_code) const
{
    if (!gram_code || gram_code[0] == '?')
        return "";
	BYTE POS = GetPartOfSpeech(gram_code);
	QWORD Grammems;
	GetGrammems(gram_code, Grammems);
	char buffer[256];
	grammems_to_str(Grammems, buffer);
	string POSstr = (POS == UnknownPartOfSpeech ) ? "*" :  GetPartOfSpeechStr(POS);
	return POSstr + string(" ") + buffer;
}


