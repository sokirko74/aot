// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma warning  (disable : 4530)
#include "StdRoss.h"
#include <stdio.h>
#include "../common/bserialize.h"
#include "Ross.h"
#include "../common/util_classes.h"
#include "LessDomItem.h"
#include "TempArticle.h"


inline size_t get_size_in_bytes (const CStructEntry& t)
{
	BYTE b;

	return get_size_in_bytes(t.m_EntryId) + EntryStrSize + get_size_in_bytes(t.m_MeanNum) +
			get_size_in_bytes(t.m_StartCortegeNo) + get_size_in_bytes(t.m_LastCortegeNo) + get_size_in_bytes(b) +
			get_size_in_bytes(t.__dummy) + AuthorNameSize;
};

inline size_t save_to_bytes(const CStructEntry& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_EntryId, buf);
	memcpy(buf, i.m_EntryStr, EntryStrSize);
	buf += EntryStrSize;
	
	// cast to int to pad to 4 bytes (since we write ints
	// afterwards)
	buf += save_to_bytes((int)i.m_MeanNum, buf);
	
	buf += save_to_bytes(i.m_StartCortegeNo, buf);
	buf += save_to_bytes(i.m_LastCortegeNo, buf);
	buf += save_to_bytes((BYTE)i.m_bSelected, buf);
	buf += save_to_bytes(i.__dummy, buf);
	memcpy(buf, i.m_AuthorStr, AuthorNameSize);
	return get_size_in_bytes(i)+3;
};

inline size_t restore_from_bytes(CStructEntry& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_EntryId, buf);
	memcpy(i.m_EntryStr, buf , EntryStrSize);
	buf += EntryStrSize;
	
	// m_MeanNum is padded to 4 bytes
	int meanNum;
	buf += restore_from_bytes(meanNum, buf);
	i.m_MeanNum = meanNum;
	
	buf += restore_from_bytes(i.m_StartCortegeNo, buf);
	buf += restore_from_bytes(i.m_LastCortegeNo, buf);
	buf += restore_from_bytes((BYTE&)i.m_bSelected, buf);
	buf += restore_from_bytes(i.__dummy, buf);
	memcpy(i.m_AuthorStr, buf, AuthorNameSize);
	return get_size_in_bytes(i)+3;
};

//=========================================================
//============           TCortegeContainer     ============
//=========================================================

TCortegeContainer::TCortegeContainer(BYTE MaxNumDom)
{
	m_MaxNumDom = MaxNumDom;
};

TCortege10*  TCortegeContainer::GetCortege (size_t i) 
{  
	if (m_MaxNumDom == 3)
		return ( TCortege10*) (&m_Corteges3[i]);
	else
	return (TCortege10*) ( &(m_Corteges10[i]) );
};

const TCortege10*  TCortegeContainer::GetCortege (size_t i)  const
{  
	if (m_MaxNumDom == 3)
		return ( TCortege10*) (&m_Corteges3[i]);
	else
		return (TCortege10*) ( &(m_Corteges10[i]) );
};

void  TCortegeContainer::_AddCortege(const TCortege10& C)
{
if (m_MaxNumDom == 3)
{
	TBasicCortege<3> Q;
	Q =  C;
	m_Corteges3.push_back( Q );;
}
else
	m_Corteges10.push_back(C);;
};


size_t  TCortegeContainer::_GetCortegesSize ()  const
{  
	if (m_MaxNumDom == 3)
		return  m_Corteges3.size();
	else
		return  m_Corteges10.size();
};

void TCortegeContainer::ClearCorteges()
{ 
	if (m_MaxNumDom == 3)
		m_Corteges3.clear();
	else
		m_Corteges10.clear();

};

void TCortegeContainer::EraseCorteges (size_t start, size_t last)
{
	if (m_MaxNumDom == 3)
		m_Corteges3.erase (m_Corteges3.begin()+start, m_Corteges3.begin()+ last);
	else
		m_Corteges10.erase (m_Corteges10.begin()+start, m_Corteges10.begin()+last);
};

void TCortegeContainer::WriteCorteges (const char* CortegeFile) const
{
	if (m_MaxNumDom == 3)
		WriteVector<CortegeType3>(CortegeFile, m_Corteges3);
	else
		WriteVector<CortegeType10>(CortegeFile, m_Corteges10);
};

void TCortegeContainer::ReadCorteges (const char* CortegeFile) 
{
	if (m_MaxNumDom == 3)
		{
			ReadVector<CortegeType3>(CortegeFile, m_Corteges3);
		}
	else
		ReadVector<CortegeType10>(CortegeFile, m_Corteges10);
};




//=========================================================
//============           TRoss                 ============
//=========================================================
TRoss::TRoss (BYTE MaxNumDom) : TCortegeContainer(MaxNumDom)
{
	m_bShouldSaveComments = false;
	m_bDontLoadExamples = false;
	m_bRussianFields = false;
	m_MaxMeanNum =	7;

};


TRoss::~TRoss ()
{
	m_Domens.clear();
	ClearFields ();
	ClearUnits();
	ClearCorteges ();
}


bool TRoss::LoadOnlyConstants(const char* _RossPath)
{
	strcpy  (RossPath, _RossPath);
	if(!MakePath (RossPath, "config.txt", ConfigFile) )		
	{
		 m_LastError = "cannot find config.txt";
		 return false; 
	};

	if(!MakePath (RossPath, "domitems.txt", DomItemsTextFile) )		
	{
		 m_LastError = "cannot find domitems.txt";
		 return false; 
	};
	
	if(!MakePath (RossPath, "items.txt", ItemsFile) )		
	{
		 m_LastError = "cannot find items.txt";
		 return false; 
	};
	

	if(!MakePath (RossPath, "domens.txt", DomensFile) )
	{
		 m_LastError = "cannot find domens.txt";
		 return false; 
	};
	if(!MakePath (RossPath, "fields.txt", FieldsFile))
	{
		 m_LastError = "cannot find fields.txt";
		 return false; 
	};

	if (!ReadConfig())
	{
		m_LastError = " Cannot parse config ";
		return false;
	};

	{
		char LastReadLine[1000];
		if(!BuildDomens (LastReadLine) )
		{
			m_LastError = Format (" Cannot build domens: the last read line=%s",LastReadLine);
			return false;
		}
	};

	if(!BuildDomItems ())  
	{	
        m_LastError = "Cannot build domitems";
        return false; 
    };

	if(!BuildFields (m_MaxNumDom))
	{
        return false; 
	};
	MakePath (RossPath, "cortege.bin", CortegeFile);
	MakePath (RossPath, "units.bin", UnitsFile);
 	MakePath (RossPath, "comments.bin", UnitCommentsFile);

	return true;
};

bool TRoss::FullLoad(const char* _RossPath)
{
	if (!LoadOnlyConstants(_RossPath))
		return false;

	if(!MakePath (RossPath, "cortege.bin", CortegeFile))
	{
		 m_LastError = "cannot find cortege.bin";
		 return false; 
	};
	if(!MakePath (RossPath, "units.bin", UnitsFile) )
	{
		 m_LastError = "cannot find units.bin";
		 return false; 
	};

	BuildUnits ();

	if(!BuildCorteges ())
	{
        m_LastError = "Cannot build corteges";
        return false; 
    }

	return true;
}

bool  TRoss::Save ()
{
	if (m_bShouldSaveComments)
		WriteVector<TUnitComment>(UnitCommentsFile, m_UnitComments);

	WriteCorteges(CortegeFile);
	WriteVector<CStructEntry>(UnitsFile, m_Units);
	WriteDomItems();
	WriteFields();
	WriteDomens();
	return true;

};

bool	TRoss::ReadConfig()
{
	string Config;
	{
		FILE* fp = fopen (ConfigFile, "rb");
		if (!fp) return false;
		char buffer[1024];
		while (fgets(buffer,1024, fp))
			Config+=buffer;
		fclose (fp);
	};

	StringTokenizer lines(Config.c_str(), "\n\r");
	while (lines())
	{
		string Line = lines.val();
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer Items(Line.c_str(), " \t\n\r");

		string Field = Items.next_token();
		string Value = Items.next_token();
		if (Field.empty() || Value.empty()) return  false;
		if (Field == "MaxNumDom")
		{
			m_MaxNumDom = atoi(Value.c_str()); 
			if (		(m_MaxNumDom != 3) 
					&&	(m_MaxNumDom != 10) 
				)
				return false;
		}
		else
		if (Field == "MaxMeanNum")
		{
			int u = atoi(Value.c_str()); 
			if (		(u < 1) 
					||	(u > 15) 
				)
				return false;
			m_MaxMeanNum = u;
		}
		else
		if (Field == "DictName")
		{
			m_DictName = Value;
		}
		else
			return false;
	};

	return true;
};


const char* TRoss::GetTitleFieldName() const
{
	if (m_bRussianFields)
		return "ЗГЛ";
	else
		return "TITLE";

};


const char* TRoss::GetSenseFieldName() const
{
	if (m_bRussianFields)
		return "ЗНАЧ";
	else
		return "SENSE";

};


const char* TRoss::GetCommFieldName() const
{
	if (m_bRussianFields)
		return "КОММ";
	else
		return "COMM";

};
const char* TRoss::GetAuthorFieldName() const
{
	if (m_bRussianFields)
		return "АВТОР";
	else
		return "AUTHOR";

};
const char* TRoss::GetTimeCreatFieldName() const
{
	if (m_bRussianFields)
		return "ВРЕМЯ_СОЗД";
	else
		return "TIME_CREATE";

};

const char* TRoss::GetRedactFieldName() const
{

	if (m_bRussianFields)
		return "РЕДАКТ";
	else
		return "REDACT";

};

void   TRoss::BuildUnits()
 {
   ClearUnits();

   if (IsBinFile (UnitsFile))
	   ReadVector<CStructEntry>(UnitsFile, m_Units);
 }


bool   TRoss::ClearUnits()
{
	m_Units.clear();
	return true;
}

void   TRoss::ClearUnit(WORD UnitNo)
{
	if (!m_Units[UnitNo].HasEmptyArticle())
		DelCorteges (m_Units[UnitNo].m_StartCortegeNo, m_Units[UnitNo].m_LastCortegeNo + 1);
    m_Units[UnitNo].m_StartCortegeNo = InitialStartPos;
	m_Units[UnitNo].m_LastCortegeNo  = InitialEndPos;
}



void  TRoss::DelUnit(vector<CStructEntry>::iterator It)
{
	if (!It->HasEmptyArticle())
		DelCorteges (It->m_StartCortegeNo, It->m_LastCortegeNo + 1);
	vector<TUnitComment>::iterator C = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(It->m_EntryId));
	assert (C->m_EntryId == It->m_EntryId);
	m_UnitComments.erase (C);
	m_Units.erase (It);
};

WORD TRoss::LocateUnit (const char * EntryStr, int MeanNum) const
{
	CStructEntry T(EntryStr, MeanNum);
	vector<CStructEntry>::const_iterator It = lower_bound(m_Units.begin(), m_Units.end(), T);   
	if (It == m_Units.end()) return ErrUnitNo;
	if (!(T == *It)) return ErrUnitNo;
	return It - m_Units.begin();
};


WORD     TRoss::GetSelectedUnitNo (WORD i) const
{
	i++;

	WORD k=0;

	for (; (i > 0)  && (k < m_Units.size()); k++)
	   if (m_Units[k].m_bSelected)
		   i--;
	return k -1;
};
WORD	TRoss::GetSelectedUnitsSize() const
{							
	WORD  i = 0;
        WORD  k = 0;
	for (; k < m_Units.size(); k++)
		if (m_Units[k].m_bSelected)
		   i++;

	return i;
};


WORD    TRoss::InsertUnit (CStructEntry& T)  
{
	vector<CStructEntry>::iterator It = lower_bound(m_Units.begin() , m_Units.end(), T);
	T.m_EntryId =  (m_UnitComments.size() == 0)? 1 : m_UnitComments[m_UnitComments.size() - 1].m_EntryId +1;
	WORD res = It - m_Units.begin();
	m_Units.insert(It, T); 
	InsertUnitComment(T.m_EntryId);
	return res;
};

WORD   TRoss::InsertUnit(const char* EntryStr, BYTE MeanNum)
{
	CStructEntry T;
    T.m_MeanNum = MeanNum;
    strcpy (T.m_EntryStr, EntryStr); 
	T.m_AuthorStr[0] = 0;
	return TRoss::InsertUnit(T);
}



static void   EstablishOneToOneCorrespondenceBetweenEntriesAndComments(TRoss& R)
{
	assert (!R.m_Units.empty());
	R.m_UnitComments.clear();
	for (size_t i = 0; i < R.m_Units.size(); i++)
	{
		R.m_Units[i].m_EntryId = i;
		R.InsertUnitComment(i);
	} ;
};





inline size_t get_size_in_bytes (const tm& t)
{
	return get_size_in_bytes(t.tm_hour)*9;
};

inline size_t save_to_bytes(const tm& i, BYTE* buf)
{
	buf += save_to_bytes(i.tm_sec, buf);
	buf += save_to_bytes(i.tm_min, buf);
	buf += save_to_bytes(i.tm_hour, buf);

	buf += save_to_bytes(i.tm_mday, buf);
	buf += save_to_bytes(i.tm_mon, buf);
	buf += save_to_bytes(i.tm_year, buf);

	buf += save_to_bytes(i.tm_wday, buf);
	buf += save_to_bytes(i.tm_yday, buf);
	buf += save_to_bytes(i.tm_isdst, buf);

	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(tm& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.tm_sec, buf);
	buf += restore_from_bytes(i.tm_min, buf);
	buf += restore_from_bytes(i.tm_hour, buf);

	buf += restore_from_bytes(i.tm_mday, buf);
	buf += restore_from_bytes(i.tm_mon, buf);
	buf += restore_from_bytes(i.tm_year, buf);

	buf += restore_from_bytes(i.tm_wday, buf);
	buf += restore_from_bytes(i.tm_yday, buf);
	buf += restore_from_bytes(i.tm_isdst, buf);
	return get_size_in_bytes(i);
};

inline size_t get_size_in_bytes (const TUnitComment& t)
{
	return get_size_in_bytes(t.m_EntryId) + AuthorNameSize + UnitCommentSize +
			get_size_in_bytes(t.modif_tm);
};

inline size_t save_to_bytes(const TUnitComment& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_EntryId, buf);
	memcpy(buf, i.Editor, AuthorNameSize);
	buf += AuthorNameSize;

	memcpy(buf, i.Comments, UnitCommentSize);
	buf += UnitCommentSize;

	buf += save_to_bytes(i.modif_tm, buf);
	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(TUnitComment& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_EntryId, buf);
	memcpy(i.Editor, buf , AuthorNameSize);
	buf += AuthorNameSize;

	memcpy(i.Comments, buf , UnitCommentSize);
	buf += UnitCommentSize;

	buf += restore_from_bytes(i.modif_tm, buf);
	return get_size_in_bytes(i);
};


bool   TRoss::ReadUnitComments()
{
    m_UnitComments.clear();
	
	UnitCommentsFile[0] = 0;

 	if (!MakePath (RossPath, "comments.bin", UnitCommentsFile))
	{
		ErrorMessage ("Cannot find comments.bin or comments.txt");
		return false;
	};
	if (!IsBinFile (UnitCommentsFile)) return false;

	
	ReadVector<TUnitComment>(UnitCommentsFile, m_UnitComments);
	sort (m_UnitComments.begin(), m_UnitComments.end());

	//  handle the error with comments in some previous versions
	if (m_UnitComments.size() != m_Units.size()) 
	{
		EstablishOneToOneCorrespondenceBetweenEntriesAndComments(*this);
	};

	assert(m_UnitComments.size() == m_Units.size());

	m_bShouldSaveComments = true;
	return true;
};

WORD    TRoss::InsertUnitComment (WORD EntryId)  
{
 	 try {
		TUnitComment C;
  		C.m_EntryId = EntryId;
		vector<TUnitComment>::iterator Ic = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), C);
		size_t No = Ic - m_UnitComments.begin();
		m_UnitComments.insert(Ic, C); 
		return No;
	  }
	  catch (...)
	  {
		 ErrorMessage("Error in inserting one UnitComment");
		 return 0;
	  };
};

TUnitComment*  TRoss::GetCommentsByUnitId(WORD EntryId)  
{
	vector<TUnitComment>::iterator It = lower_bound(m_UnitComments.begin() , m_UnitComments.end(), TUnitComment(EntryId));
	assert (   (It != m_UnitComments.end()) 
			&& (It->m_EntryId == EntryId) 
		   ) ;
	return &(*It);
};

const TUnitComment*  TRoss::GetCommentsByUnitId(WORD EntryId)   const
{
	vector<TUnitComment>::const_iterator It = lower_bound(m_UnitComments.begin() , m_UnitComments.end(), TUnitComment(EntryId));
	assert (   (It != m_UnitComments.end()) 
			&& (It->m_EntryId == EntryId) 
		   ) ;
	return &(*It);
};


bool   TRoss::BuildCorteges()
{
   ClearCorteges();

   if (IsBinFile(CortegeFile))
	   ReadCorteges (CortegeFile);

   return true;
}

bool TRoss::UpdateSignatsOfTheFieldInCorteges (BYTE FieldNo, vector<CSignat>& Signats) 
{
	for (size_t j=0; j < _GetCortegesSize(); j++)
	 if (GetCortege(j)->m_FieldNo == FieldNo)
	 {
		 vector<CSignat>::iterator it = find(Signats.begin(), Signats.end(), Fields[FieldNo].m_Signats[GetCortege(j)->GetSignatNo()]);

		 GetCortege(j)->SetSignatNo(it - Signats.begin());
		 if  (GetCortege(j)->GetSignatNo() == Fields[FieldNo].m_Signats.size())
			 return false;
	 };
	 return true;
};

void TRoss::DelCorteges (size_t start, size_t last)
{						
	for (size_t i=0; i<m_Units.size(); i++)
     if (  !m_Units[i].HasEmptyArticle()
	     && m_Units[i].m_StartCortegeNo >= last)
	 {
		 m_Units[i].m_StartCortegeNo -= (last - start);
		 m_Units[i].m_LastCortegeNo  -= (last - start);
	 };

	EraseCorteges(start,last);
};



struct TItemStr
{
  char ItemStr[100];
  TItemStr (char* s)
  {
	  strcpy (ItemStr, s);
  };

};


bool   TRoss::ReadFromStrWithOneSignatura (const char* s, TCortege10& C, BYTE SignatNo)
{
	int CurrItemNo = 0;
	CSignat& Sgn = Fields[C.m_FieldNo].m_Signats[SignatNo];

	vector<TItemStr> ItemStrVec;
	const char* q = s;

	int i =0;
	for (i=0; i<Sgn.DomsWithDelims.size(); i++)
	{
		BYTE DomNo = Sgn.DomsWithDelims[i].m_DomNo;
		bool IsMult = Sgn.DomsWithDelims[i].m_IsMult;
		bool IsDelim = m_Domens[DomNo].IsDelim;
		bool FlagLastItem = (i == Sgn.DomsWithDelims.size() - 1);
		char Delim[10];
		Delim[0] = 0;

		bool FlagNextDelim = false;
		if (!FlagLastItem)
			if (   (i < Sgn.DomsWithDelims.size()-1) 
				&& m_Domens[Sgn.DomsWithDelims[i+1].m_DomNo].IsDelim
				&& !m_Domens[Sgn.DomsWithDelims[i+1].m_DomNo].IsEmpty()
				)
				FlagNextDelim = true;

		if (FlagNextDelim)
			strcat (Delim, m_Domens[Sgn.DomsWithDelims[i+1].m_DomNo].m_Items);

		if  (!FlagLastItem  && !FlagNextDelim)
			strcat (Delim, " ");

		while (isspace ((unsigned  char) q[0])) q++;

		char ItemStr[100];
		size_t ItemStrLen = IsDelim ? 1 : strcspn(q, Delim);
		if (ItemStrLen  > 99)  return false;
		strncpy  (ItemStr, q, ItemStrLen);
		ItemStr[ItemStrLen] = 0;
		if (!IsDelim) rtrim (ItemStr);

		/*
			если есть два формата для одного поля: 
				D_ENGL
				D_RLE  
			и пришло русское слово, тогда не будем добавлять его в 
				в D_ENGL
		*/
		if ( IsRussian (ItemStr) )
			if (!strcmp  (m_Domens[DomNo].DomStr, "D_ENGL"))
				return false;


		/*if (DomNo == ActantsDomNo) 
		{
			if (   (strlen (ItemStr) == 2) 
				&& (ItemStr[0] == (unsigned char)'A') 
				&&  isdigit(ItemStr[1]))
				ItemStr[0] = (unsigned char)'А';

			if  (		(strlen (ItemStr) == 1) 
					&&  (ItemStr[0] == (unsigned char)'C')
				)
				ItemStr[0] = (unsigned char)'С';
		};*/

		// #### Получение в строку q остатка строки для дальнейшей обработки
		q += ItemStrLen;

		//если строка прежедвременно закончилалсь,
		// то выйти из процедуры с неудачей
		if (!FlagLastItem && IsEmptyLine(q))  return false;

		/*
		если данный элемент есть повторение некоторых констант их другого домена
		Например, "D_GRAMMEMS+" означает, что на этом месте стоит перечень элементов
		из домена D_GRAMMEMS, разделенных запятыми.
		Нужно проверить, что перечень синтаксически правилен и записать его в домен 
		D_MULT одной константой.
		*/
		if (IsMult)
		{
			StringTokenizer tok(ItemStr," ,");
			while (tok())
				if (GetItemNoByItemStr (tok.val(), DomNo) == -1)
					return false;

			DomNo = GetDomenNoByDomStr("D_MULT");
		};

		// #### Поиск найденной  строки в домене
		if (   (strlen (ItemStr) == 1)     
			&& ((unsigned char)ItemStr[0] == '*') )
		{
			C.SetItem(CurrItemNo,WildCardDomItemNo);
		}
		else
			C.SetItem(CurrItemNo, GetItemNoByItemStr (ItemStr, DomNo));



		if (   (!m_Domens[DomNo].IsFree)   // Домен константный
			&& (DomNo != LexPlusDomNo)
			&& (C.GetItem(CurrItemNo) == -1))
		// Отрицательный результат
		return false;

		if (!IsDelim)
		{
			ItemStrVec.push_back( ItemStr);
			CurrItemNo++;
		};

	};


	if (!IsEmptyLine(q) ||  (i < Sgn.DomsWithDelims.size())) return false;

	for (i=ItemStrVec.size();  i < m_MaxNumDom; i++)
		C.SetItem(i,-1);



	for (i=0; i < ItemStrVec.size(); i++)
		if (C.GetItem(i) == -1) // не определено значение
		{
			int ItemNo;
			
			if (!InsertDomItem (ItemStrVec[i].ItemStr, Sgn.Doms[i], ItemNo))
				return false;

			C.SetItem(i,ItemNo); 
		};

	return true;
};


bool   TRoss::ReadFromStr (const char* s, TCortege10& C)
{
	int i=0;

	for (; i < Fields[C.m_FieldNo].m_Signats.size(); i++)
		if (ReadFromStrWithOneSignatura (s, C, i))
		  {
			  C.SetSignatNo(i);
			  break;
		  };

	return i < Fields[C.m_FieldNo].m_Signats.size();
};


void TRoss::WriteToStr (const int* Items, const char* Frmt, char* OutBuffer) const
{
    
   BYTE Counter  = 0 ;
   *OutBuffer = 0;
   BYTE BufferLen = 0;
   if (Frmt == 0) return;
   BYTE len = strlen(Frmt);


   for (BYTE i=0; i < len; i++)
	 if (    (Frmt[i] == '%') 
		  && (i+1 < len) 
		  && (Frmt[i+1] == 's')
		)
	 {
		 int ItemId = Items[Counter];
		 if (ItemId != -1)
		 {
            BYTE Len =   m_DomItems[ItemId].GetItemStrLen();
			strncpy (OutBuffer+BufferLen, GetDomItemStr(m_DomItems[ItemId]), Len);
			Counter++;
			BufferLen += Len;
		 };
  		 i++;
	 }
	 else
		 OutBuffer[BufferLen++] = Frmt[i];

	 if (Counter == 0)
	 {
		 BufferLen = 0;
	 };

	 OutBuffer[BufferLen] = 0;
}

void TRoss::CortegeToStr (const TCortege10& C, char* OutBuffer) const
{
	const CField& F =  Fields[C.m_FieldNo];
	WriteToStr (C.m_DomItemNos, F.m_Signats[C.GetSignatNo()].sFrmt,OutBuffer);
};


inline bool IsTitle (const char * s) 
{
	if (!s) return false;
	for (int i=0; i < strlen(s); i++)
		if (   isdigit((unsigned char)s[i]) 
		    )
			return false;
 return true;

};

bool   TRoss::InsertDomItem (const char* ItemStr, BYTE DomNo, int& ItemNo)
{
   if (DomNo == TitleDomNo)
    if (!IsTitle (ItemStr))
		  {
			m_LastError = Format("Warning! Cannot add \"%s\" to title domen!",ItemStr);
			return false;
          };


	if (DomNo == LexDomNo)
		  if (!IsStandardRusLexeme (ItemStr))
		  {
			m_LastError = Format("Warning! Cannot add \"%s\" to lexeme domen!",ItemStr);
			return false;
          };

    if (DomNo == LexPlusDomNo)
	  {
		  DomNo = GetDomNoForLePlus (ItemStr);
		  if (DomNo == ErrUChar)
		  {
            m_LastError = Format("Warning! Cannot add \"%s\" to the extended lexeme domen!",ItemStr);
			return false;
		  };
	  };

	TDomItem D;
	D.SetDomNo (DomNo);
	D.SetItemStrLen ( strlen(ItemStr) );
	D.SetItemStrNo  ( m_Domens[D.GetDomNo() ].AddItem(ItemStr, D.GetItemStrLen() ) );
	vector<TDomItem>::iterator It = lower_bound (m_DomItems.begin(), m_DomItems.end(), D, IsLessByItemStrNew(this));
	ItemNo =  (int)(It-m_DomItems.begin());
	if (m_Domens[DomNo].IsEmpty())
	{
		 m_Domens[DomNo].m_StartDomItem = ItemNo;
		 m_Domens[DomNo].m_EndDomItem = ItemNo+1;
	}
	else
		m_Domens[DomNo].m_EndDomItem++;

	size_t i=0;

	for (;   i<m_Domens.size(); i++)
		if (m_Domens[i].m_StartDomItem >  ItemNo)
		{
			m_Domens[i].m_StartDomItem++;
			m_Domens[i].m_EndDomItem++;
		};

	m_DomItems.insert(It, D);
	for (i=0;   i<_GetCortegesSize(); i++)
		for (size_t k=0; k < m_MaxNumDom; k++)
			if (GetCortege(i)->GetItem(k) >= ItemNo)
				GetCortege(i)->SetItem(k, GetCortege(i)->GetItem(k)+1);

	return true;;
};



void TRoss::DelDomItem	(int ItemNo)
{
   // константы системных доменов не могут встречаться в словарных статьях
   if (m_Domens[ m_DomItems[ItemNo].GetDomNo() ].Source != dsSystem)
	for (size_t i=0; i<m_Units.size(); i++)
     if (!m_Units[i].HasEmptyArticle())
	 {
	  for (size_t k =  m_Units[i].m_StartCortegeNo; k <= m_Units[i].m_LastCortegeNo; k++)
	  {
   	    for (size_t j=0; j < m_MaxNumDom; j++)
		  if (GetCortege(k)->GetItem(j) == ItemNo)
		  {
			if (GetCortege(k)->m_LevelId > 0)
			 for (size_t l = m_Units[i].m_StartCortegeNo; l <= m_Units[i].m_LastCortegeNo; l++)
              if (    (GetCortege(l)->m_FieldNo == GetCortege(k)->m_FieldNo)
				   && (GetCortege(l)->m_LeafId == GetCortege(k)->m_LeafId)
				   && (GetCortege(l)->m_LevelId >  GetCortege(k)->m_LevelId)
				 )
				 GetCortege(l)->m_LevelId--;
				
			DelCorteges(k, k+1);
	        if (m_Units[i].m_StartCortegeNo == m_Units[i].m_LastCortegeNo)
			{
		      m_Units[i].m_StartCortegeNo = InitialStartPos;
		      m_Units[i].m_LastCortegeNo = InitialEndPos;
			  goto EmptyArticle;
			};
			 
			 m_Units[i].m_LastCortegeNo--;
			 k--;
			 break;
		  };

	  };  	  
		   
        EmptyArticle:;
	};
   
	
	int ItemStrLen =  m_DomItems[ItemNo].GetItemStrLen();

	m_Domens[m_DomItems[ItemNo].GetDomNo() ].DelItem(m_DomItems[ItemNo].GetItemStrNo(),  ItemStrLen);

	int i=0;

	for (;   i<m_Domens.size(); i++)
		if (m_Domens[i].m_StartDomItem >  ItemNo)
		{
			m_Domens[i].m_StartDomItem--;
			m_Domens[i].m_EndDomItem--;
		};


		
	for (i=0; i<m_DomItems.size(); i++)
		if ( m_DomItems[i].GetDomNo() == m_DomItems[ItemNo].GetDomNo() )
			if ( m_DomItems[i].GetItemStrNo() > m_DomItems[ItemNo].GetItemStrNo() )
			{
				int uu = m_DomItems[i].GetItemStrNo();
				m_DomItems[i].SetItemStrNo(  uu -  (ItemStrLen + 1) );
			};

	for (i=0; i < _GetCortegesSize(); i++)
		for (size_t j=0; j < m_MaxNumDom; j++)
			if (   GetCortege(i)->GetItem(j) != -1
			 	&& GetCortege(i)->GetItem(j) > ItemNo
			   )
			 GetCortege(i)->SetItem(j, GetCortege(i)->GetItem(j) - 1);

	m_DomItems.erase (   m_DomItems.begin() +ItemNo );
};


bool CDictionary::Load(const char* Path)
{
		if (!FullLoad(Path))
		{
			TItemContainer::ErrorMessage(m_LastError);
			return  false;
		}
		else
			return   true;

};


void TRoss::SetUnitCommentStr(WORD UnitNo, const char* Str)
{
    TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	int l = strlen(Str);
	if (l > UnitCommentSize - 1) 
		l = UnitCommentSize - 1;
    strncpy (C->Comments, Str, l);
	C->Comments[l] = 0;
}

void TRoss::SetUnitAuthor(WORD UnitNo, const char* Author)
{
	int l = strlen(Author);
	if (l > AuthorNameSize - 1) 
		l = AuthorNameSize - 1;
    strncpy (m_Units[UnitNo].m_AuthorStr, Author, l);
	m_Units[UnitNo].m_AuthorStr[l] = 0;
}

tm Str2Tm (string TimeStr)
{
	tm output;
	sscanf (TimeStr.c_str(), "%i/%i/%i %i:%i:%i", &output.tm_mday,&output.tm_mon,&output.tm_year, &output.tm_hour, &output.tm_min, &output.tm_sec);
	output.tm_mon --;
	return output;
};


void TRoss::SetUnitModifTimeStr(WORD UnitNo, const char* TimeStr)
{
	try {
		TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		C->modif_tm = Str2Tm(TimeStr);
	}
	catch (...)
	{
		::ErrorMessage ("StructDict", "Error in setting UnitComments");
	};
}

void TRoss::SetUnitEditor(WORD UnitNo, const char* Editor)
{
  try 
  {
     TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	 strcpy (C->Editor,Editor);
  }
  catch (...)
  {
	  ::ErrorMessage ("StructDict", "Error in setting UnitEditor");
  };
}

string TRoss::GetUnitModifTimeStr(WORD UnitNo) const
{
	try {
		char tmpbuf[128]; 
		const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		int year = C->modif_tm.tm_year;
		if (year > 100)
			year -= 100;
		sprintf( tmpbuf, "%i/%i/%i %i:%i:%i", C->modif_tm.tm_mday, C->modif_tm.tm_mon+1, year,
				C->modif_tm.tm_hour, C->modif_tm.tm_min, C->modif_tm.tm_sec);
		return tmpbuf;
	}
	catch (...)
	{
		ErrorMessage("Error in getting UnitComments");
		return "";
	};
}

string TRoss::GetUnitTextHeader(WORD UnitNo) const
{
	string R;
	const CStructEntry& U = m_Units[UnitNo];
	const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);

	R += Format("%s        = %s\r\n", GetTitleFieldName(), U.m_EntryStr);
	R += Format("%s       = %u\r\n", GetSenseFieldName(),U.m_MeanNum);

	if (C && strlen(C->Comments))
		  R += Format("%s       = %s\r\n", GetCommFieldName(),C->Comments);

	
	if (strlen(U.m_AuthorStr) > 0)
		R += Format("%s       = %s\r\n", GetAuthorFieldName(),U.m_AuthorStr);

	
	if (C && strlen(C->Editor))
		R += Format("%s       = %s\r\n", GetRedactFieldName(),C->Editor);

	string t = TRoss::GetUnitModifTimeStr(UnitNo);
	if (!t.empty())
		R += Format("%s       = %s\r\n", GetTimeCreatFieldName(),t.c_str());

	return R;
}








//=============================

TUnitComment::TUnitComment() 
{
		modif_tm = RmlGetCurrentTime();
		Comments[0] = 0;
		Editor[0] = 0;
};

TUnitComment::TUnitComment(int _UnitId)
{
	m_EntryId = _UnitId;
};



//===================================================================
//==================== CDictionary    ===============================
//===================================================================

CDictionary::CDictionary() : TRoss(3)
{
}
vector<CStructEntry>& CDictionary::GetUnits()
{
	return m_Units;
};
BYTE CDictionary::GetCortegeFieldNo(size_t i) const
{
	return GetCortege(i)->m_FieldNo;

};
BYTE CDictionary::GetCortegeLeafId(size_t i) const
{
	return GetCortege(i)->m_LeafId;
};
BYTE CDictionary::GetCortegeBracketLeafId(size_t i) const
{
	return GetCortege(i)->m_BracketLeafId;

};

const char*  CDictionary::GetDomItemStrInner(int ItemNo) const 
{
	return TRoss::GetDomItemStr(m_DomItems[ItemNo]);
};

bool CDictionary::IsEmptyArticle(WORD UnitNo) const
{ 
	return m_Units[UnitNo].HasEmptyArticle();
};

int CDictionary::GetUnitStartPos(WORD UnitNo) const
{
	return m_Units[UnitNo].m_StartCortegeNo; 
};

int CDictionary::GetUnitEndPos(WORD UnitNo) const
{
	return  m_Units[UnitNo].m_LastCortegeNo; 
};

BYTE	CDictionary::GetFieldNoByFieldStr(const char* Str) const
{
	return  TRoss::GetFieldNoByFieldStrInner(Str);
};

BYTE CDictionary::GetDomItemDomNo (int ItemNo) const
{
	return m_DomItems[ItemNo].m_DomNo;
}


int	CDictionary::GetCortegeItem(long CortegeNo, BYTE PositionInCortege) const
{
	return	GetCortege(CortegeNo)->GetItem(PositionInCortege);
};

const char* CDictionary::GetDomItemStr(int ItemNo) const
{
	return  (ItemNo == - 1) ? NULL : GetDomItemStrInner(ItemNo);
}
string	CDictionary::GetEntryStr (WORD EntryNo) const
{
	return m_Units[EntryNo].m_EntryStr;
};

BYTE		CDictionary::GetUnitMeanNum(WORD EntryNo) const
{
	return m_Units[EntryNo].m_MeanNum;
};

bool CDictionary::IncludeArticle(WORD UnitNo, string Article) const
{
	CTempArticle A1;
	A1.m_pRoss = const_cast<CDictionary*>(this);
	A1.ReadFromDictionary(UnitNo, false, true);

	CTempArticle A2;
	A2.m_pRoss = const_cast<CDictionary*>(this);
	A2.SetArticleStr(Article.c_str());
	A2.MarkUp();
	A2.BuildCortegeList();
	return A2.IsPartOf(&A1, true);
}


//=============    Import from Text file   ==========================


bool IsRubicon (const string& S)
{
	return S.find("====") != string::npos;
}


bool FindRubicon (vector<CSourceLine>& L, size_t& pos)
{
	while (      (pos < L.size()) 
			&&  (!IsRubicon(L[pos].m_Line))
		)
	pos++;

	return pos < L.size();
}

void DeleteEmptyLines (vector<CSourceLine>& L)
{
	for (int i=0; i < L.size(); )
	{
		Trim(L[i].m_Line);
		if (L[i].m_Line.empty())
		  L.erase(L.begin() + i);
	    else
		  i++;
	};
};

int NumArt (vector<CSourceLine>& L) 
{
	int Res = 0;

	for (int i=0;  i+1 < L.size(); i++)
		if (IsRubicon(L[i].m_Line))
			Res++;

	return Res;
};


bool GetValue (string Pair, string FldName, string& Value)
{
	StringTokenizer tok(Pair.c_str(), " \t");
	if (tok.next_token() != FldName) return false;
	if (tok.next_token() != "=") return false;
	Value = tok.get_rest();
	Trim(Value);
	return true;
};


void CutComments (vector<CSourceLine>& L)
{
	for (size_t i=0; i<L.size(); i++)
	{         
		int k = L[i].m_Line.find("//");
		if (k != string::npos)
			L[i].m_Line.erase(k);
	}
}



void AddMessage (string Message, int LineNo, string& Messages)
{
	Trim(Message);
	if (LineNo != -1)
		Message += Format(" (line %i)", LineNo);
	Message +=  "\r\n";
	Messages += Message;
};


bool CDictionary::ImportFromText(string FileName, bool bSimulating, ImportConflictEnum ConflictSolver, int StartEntry,string& Messages) 
{
	Messages = "";

	vector<CSourceLine> L;
	{
		FILE* fp = fopen(FileName.c_str(), "r");
		if (!fp)
		{
			Messages += Format("Cannot read input file %s\n", FileName.c_str());
			return false;
		};
		char buffer[1000];
		int CurrentLineNo = 0;
		while (fgets(buffer, 1000, fp))
		{
			string S = buffer;
			Trim(S);
			L.push_back(CSourceLine(S, CurrentLineNo));
			CurrentLineNo++;
		};
		fclose (fp);
	}
	
	
	int ErrorsCount = 0;

	CutComments (L);
	DeleteEmptyLines(L);
	size_t NumOfArt = NumArt(L);
	Messages += Format ("Number of found entries: %i\n", NumOfArt);
	size_t start = 0;
	size_t last = 0;
	size_t NumOfGoodArt = 0;
	size_t SaveDomItemCount = GetDomItemsSize();

	
    for (int i=0; i<NumOfArt; i++)
	{ 
		if (FindRubicon (L, start))
		start++;
		last = start;
		FindRubicon (L, last);
		if ( i+1<StartEntry) continue;
   
		if (ProcessOneArticle (L, start, last, bSimulating, ConflictSolver, Messages))
	        NumOfGoodArt ++;
		else
		   ErrorsCount++;
	};

	Messages += Format ("Number of %s entries: %i\n",(bSimulating?" tested" : "loaded"), NumOfGoodArt);
	Messages += Format ("Number of new constants: %i\n",GetDomItemsSize() - SaveDomItemCount);
	return ErrorsCount == 0;
}


bool CDictionary::ProcessOneArticle ( vector<CSourceLine>& L, int start, int last, bool bSimulating, ImportConflictEnum ConflictSolver, string& Messages)
{
   size_t RealStart = start; 
   if (L.size() == 1) return false;
   string S;
   if (GetValue(L[RealStart].m_Line,"Дескриптор",S)) RealStart++;

   string Lemma;
   const char* Field = GetTitleFieldName();
   if (!GetValue(L[RealStart].m_Line,Field,Lemma))
   {
	   AddMessage (Format("Cannot read field %s\n", Field).c_str(), L[start].m_SourceLineNo+1, Messages);
	   return false;
   }
   RealStart++;

   Field = GetSenseFieldName();
   if (!GetValue(L[RealStart].m_Line,Field,S))
   { 
		AddMessage (Format("Cannot read field %s", Field).c_str(), L[start].m_SourceLineNo+1, Messages);
		return false;
   }

   BYTE MeanNum = atoi (S.c_str());
   if (MeanNum == 0)
   {
		AddMessage (Format("Cannot read field %s", Field).c_str(), L[start].m_SourceLineNo+1, Messages);
		return false;
   }
   if ((m_MaxMeanNum == 1) &&  (MeanNum>1))
   {
		AddMessage ("No different senses are possible  for this dictionary", L[start].m_SourceLineNo+1, Messages);
		return false;
   }

   string Comments = "";
   Field = GetCommFieldName();
   if (RealStart+1 < L.size())
   if (GetValue(L[RealStart+1].m_Line,Field,S))
   {
	   Comments = S; 
	   RealStart++;
   };

   string Author = "";
   Field = GetAuthorFieldName();
   if (RealStart+1 < L.size())
   if (GetValue(L[RealStart+1].m_Line,Field,S))
   {
	   Author = S; 
	   RealStart++;
   };
   string ModifTime = "";
   Field = GetTimeCreatFieldName();
   if (RealStart+1 < L.size())
   if (GetValue(L[RealStart+1].m_Line,Field,S))
   {
	   ModifTime = S; 
	   RealStart++;
   };

   string UnitEditor = "";
   Field = GetRedactFieldName();
   if (RealStart+1 < L.size())
   if (GetValue(L[RealStart+1].m_Line,Field,S))
   {
	   UnitEditor = S; 
	   RealStart++;
   };

   Field = GetTimeCreatFieldName();
   if (RealStart+1 < L.size())
   if (GetValue(L[RealStart+1].m_Line,Field,S))
   {
	   ModifTime = S; 
	   RealStart++;
   };


   WORD UnitNo = LocateUnit(Lemma.c_str(), MeanNum);

   if  (UnitNo != ErrUnitNo) 
	   if (ConflictSolver == iceSkip)
		   return true;
	   else
		   if	(		(ConflictSolver == iceOverwrite)
					&&	!m_Units[UnitNo].HasEmptyArticle()
					&&	!bSimulating
			   )
   		     ClearUnit(UnitNo);
		   else;
   else
		if (!bSimulating)
		    UnitNo = InsertUnit (Lemma.c_str(), MeanNum); 
		

  CTempArticle A1;
  A1.m_pRoss = this;
  if (UnitNo != ErrUnitNo)
    A1.ReadFromDictionary(UnitNo, false, false);
  
  string NewArticle;
  for (int i = RealStart+1; i < last; i++)
     NewArticle += L[i].m_Line + string("\n");

  CTempArticle A2;
  

  try
  {
  	 A2.m_pRoss = this;
	 if (!A2.SetArticleStr(NewArticle.c_str()))
	 {
	 	   int LocalLineNo = A2.m_ErrorLine - 1;
		   if (LocalLineNo < 0)
			   LocalLineNo = 0;
		   AddMessage(A2.m_LastError, L[LocalLineNo+start+(RealStart-start)].m_SourceLineNo+1, Messages);
		   if (A2.m_LastError.empty())
			 Messages += "an error occurred!\n";
		   return false;
	 };

	 if (		(ConflictSolver == iceAppend) 
			&&	(A1.IntersectByFields(&A2) > 0)
		)
	 {
		 Messages += "You cannot add one entry to another, because they both contain the same fields\n";
		 return false;
	 };

	 if (!A1.AddArticle(&A2))
	 {
	 	   int LocalLineNo = A1.m_ErrorLine - 1;
		   if (LocalLineNo < 0)
			   LocalLineNo = 0;
		   AddMessage(A1.m_LastError, L[LocalLineNo+start+(RealStart-start)].m_SourceLineNo+1, Messages);
		   if (A1.m_LastError.empty())
			 Messages += "an error occurred!\n";
		   return false;
	 };

	 if (bSimulating)
		 return true;

	 // запись комментария
	 SetUnitCommentStr(UnitNo, Comments.c_str());
	 SetUnitAuthor(UnitNo, Author.c_str());
	 if (ModifTime != "")
	   SetUnitModifTimeStr(UnitNo, ModifTime.c_str());

	 if (UnitEditor != "")
		SetUnitEditor(UnitNo, UnitEditor.c_str());

	 
	 // запись кортежей	 
	 A1.WriteToDictionary();
   }
   catch  (...)
   {
	   int LocalLineNo = A2.m_ErrorLine - 1;
	   if (LocalLineNo < 0)
		   LocalLineNo = 0;
	   AddMessage (m_LastError, L[LocalLineNo+start+(RealStart-start)].m_SourceLineNo+1, Messages);
	   return false;
   }
   

  return true;
};

void CDictionary::SetUnitCurrentTime(WORD UnitNo) 
{
	TUnitComment* C = GetCommentsByUnitId(GetUnits()[UnitNo].m_EntryId);
	C->modif_tm = RmlGetCurrentTime();
}


bool CDictionary::AddField(string FieldStr)
{
	if (FieldStr.empty())
		FieldStr = "_";
	if (FieldStr.length()+1 >=FieldStrLen )
	{
		ErrorMessage("Field is too long");
		return false;
	};
	int ItemNo;
	if (!TRoss::InsertDomItem(FieldStr.c_str(), FieldDomNo, ItemNo))
	{
		ErrorMessage(m_LastError);
		return false;
	};

	CField T;
	T.FieldId = 0;
	strcpy (T.FieldStr, FieldStr.c_str());
	Fields.push_back(T);
    return true;
}


string CDictionary::GetUnitEditor(WORD UnitNo) const
{
	try 
	{
		const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		return C->Editor;
	}
	catch (...)
	{
		ErrorMessage ("Error in setting UnitEditor");
	};
	return "";
}

void  CDictionary::SetUnitStr(WORD UnitNo, const char*  UnitStr)
{
	strcpy (m_Units[UnitNo].m_EntryStr, UnitStr);
	sort (m_Units.begin(),  m_Units.end());
}
