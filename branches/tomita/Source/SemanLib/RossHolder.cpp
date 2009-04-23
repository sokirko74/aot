#include "stdafx.h"
#include "RossHolder.h"
#include "../common/utilit.h"

CRossHolder	::CRossHolder (CTranslatorHolder* TranslatorHolder)	
{
	m_DictName	= "unknown";
	m_LastUpdateTime =	0;
	m_bOwnDictionary =	true;
	m_TranslatorHolder	= TranslatorHolder;
};

CRossHolder::~CRossHolder()
{

};

bool CRossHolder::OpenRossHolder (const string strPathName, bool bDontLoadExamples) 
{
	m_LastUpdateTime = 1;
	char RossDir[_MAX_PATH];
	strcpy (RossDir, strPathName.c_str());
	char* q = strrchr (RossDir,'\\'); 
	if (q == 0)
		q = strrchr (RossDir,'/'); 

	if (q == 0)
	{
		assert (q);
		return false;
	}
	*q = 0;

	
	if (!m_Ross.Load(RossDir))
		return false;

	if (!bDontLoadExamples)
		if (!m_Ross.ReadUnitComments())
			return false;

	InitConsts();
	m_DictName = m_Ross.m_DictName;
	return true;
};






bool CRossHolder::InitConsts() 
{
	SynRelDomNo	= GetRoss()->GetDomenNoByDomStr("D_SYN_REL");
	SemFetDomNo	= GetRoss()->GetDomenNoByDomStr("D_SF");
	SynGroupDomNo =	GetRoss()->GetDomenNoByDomStr("D_GROUPS");
	LemGroupBeginingDomNo =	GetRoss()->GetDomenNoByDomStr("D_1");

	IndirObjSynONo =  GetRoss()->GetItemNoByItemStr	("к_доп", SynRelDomNo);
	ReverseSynONo =	 GetRoss()->GetItemNoByItemStr ("X!", SynRelDomNo);
	UncoordAttrSynONo =	 GetRoss()->GetItemNoByItemStr ("нс_опр", SynRelDomNo);
	CoordAttrSynONo	=  GetRoss()->GetItemNoByItemStr ("с_опр", SynRelDomNo);
	DirectObjSynONo	=  GetRoss()->GetItemNoByItemStr ("п_доп", SynRelDomNo);
	SubjSynONo =  GetRoss()->GetItemNoByItemStr	("подл", SynRelDomNo);
	EnglSubjSynONo =  GetRoss()->GetItemNoByItemStr	("subj", SynRelDomNo);
	ParatAttrSynONo	=  GetRoss()->GetItemNoByItemStr ("прим_опр", SynRelDomNo);
	FromPredicSynONo =	GetRoss()->GetItemNoByItemStr ("отпредик", SynRelDomNo);
	PostSpecifSynONo =	GetRoss()->GetItemNoByItemStr ("уточн",	SynRelDomNo);


	

	ValFieldNo = GetRoss()->GetFieldNoByFieldStr ("VAL");
	LexFunctFieldNo	= GetRoss()->GetFieldNoByFieldStr ("LF");
	GramFetFieldNo = GetRoss()->GetFieldNoByFieldStr ("GF");
	RefFieldNo = GetRoss()->GetFieldNoByFieldStr ("REF");
	POFieldNo =	GetRoss()->GetFieldNoByFieldStr	("DOMAIN");
	GramRestrFieldNo = GetRoss()->GetFieldNoByFieldStr ("RESTR");
	LexFetFieldNo =	GetRoss()->GetFieldNoByFieldStr	("LEX");
	NESOVMFieldNo =	GetRoss()->GetFieldNoByFieldStr	("INCONS");
	ObligLexFetFieldNo = GetRoss()->GetFieldNoByFieldStr ("MANLEX");
	ArticleFieldNo = GetRoss()->GetFieldNoByFieldStr ("DETERM");

	SemFetFieldNo  = GetRoss()->GetFieldNoByFieldStr ("SF");
	


	ActDomNo = GetRoss()->GetDomenNoByDomStr("D_ACTANTS");
//	GroupBeginAndGrammemDomNo =	GetRoss()->GetDomenNoByDomStr("D_1");
	MNADomNo = GetRoss()->GetDomenNoByDomStr("D_MUA");
	SelfLabelNo	= GetRoss()->GetItemNoByItemStr("C",ActDomNo);
	PositionDomNo =	GetRoss()->GetDomenNoByDomStr("D_POSITION");
	VerbFetDomNo = GetRoss()->GetDomenNoByDomStr("D_VP_SPECIF");
	LexFunctDomNo =	GetRoss()->GetDomenNoByDomStr("D_LF");

	MainWordVarNo =	GetRoss()->GetItemNoByItemStr ("C",	ActDomNo);
	

	InitDomainsConsts();
	
	

	if (MainWordVarNo == -1) return	false;

	MNANo =	 -1;
	if (MNADomNo !=	ErrUChar)
			MNANo =	GetRoss()->GetItemNoByItemStr("MUA",MNADomNo);

	S_And_InstrNo =	-1;
	if (LemGroupBeginingDomNo != ErrUChar)
		S_And_InstrNo =	GetRoss()->GetItemNoByItemStr("с+Т",LemGroupBeginingDomNo);
	return true;
};

bool CRossHolder::HasCX (WORD UnitNo, const string CX, const string DomStr) const 
{
	return HasItem (UnitNo, "SF", CX, DomStr,0,0);
};

const char* CRossHolder::GetDomItemStrInner (long ItemNo) const 
{
	if (ItemNo == -1) 
		return "";
	else
		return  m_Ross.GetDomItemStrInner(ItemNo);
};



string WriteToString (const CDictionary* Ross, const char* Frmt, const TCortege10& C)
{
	BYTE	Counter	 = 0 ;
	string Result  =	"";
	BYTE	BufferLen =	0;
	if (Frmt	== 0) return Result;
	BYTE	len	= strlen(Frmt);


	for (BYTE i=0; i	< len; i++)
		if	(	 (Frmt[i] == '%') 
			&& (i+1 <	len) 
			&& (Frmt[i+1]	== 's')
			)
		{
			long ItemId = C.m_DomItemNos[Counter];
			if	(ItemId	!= -1)
		 {
			 Result+= Ross->GetDomItemStrInner(ItemId);
			 Counter++;
		 };
			i++;
		}
		else
			Result+= Frmt[i];

	return	Result;
}




const int LoginSize	= 10;




long CRossHolder::GetItemNoByItemStr(const char* ItemStr, const	char* DomStr) const
{
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr);
	return GetRoss()->GetItemNoByItemStr(ItemStr, DomNo);
}

 


bool CRossHolder::InitDomainsConsts()
{
	AdvAdjMainNo = GetItemNoByItemStr ("НАР:нар_опр", "D_GF_MAIN");
	NounMainNo = GetItemNoByItemStr	("СУЩ:ИГ", "D_GF_MAIN");
	VerbMainNo = GetItemNoByItemStr	("ГЛ:ГГ", "D_GF_MAIN");
	AdjMainNo =	GetItemNoByItemStr ("ПРИЛ:с_опр", "D_GF_MAIN");
	PronMainNo = GetItemNoByItemStr	("МЕСТОИМ:с_опр", "D_GF_MAIN");
	ClauseGrpNo	=  GetItemNoByItemStr ("ПРИД_ПР", "D_GROUPS");
	NounGrpNo =	 GetItemNoByItemStr	("ИГ", "D_GROUPS");
	AdverbialGrpNo =  GetItemNoByItemStr ("ОБСТ_ГР", "D_GROUPS");
	VerbGrpNo =	 GetItemNoByItemStr	("ГГ", "D_GROUPS");
	CopulNo	=  GetItemNoByItemStr ("Copul",	"D_GROUPS");
	ModalCopulNo =	GetItemNoByItemStr ("ModalCopul", "D_GROUPS");
	NumerComplexNo =  GetItemNoByItemStr ("ЦК",	"D_GRAFEM_DESC");
	NumerSymbComplexNo =  GetItemNoByItemStr ("ЦБК", "D_GRAFEM_DESC");
	HyphenNo =	GetItemNoByItemStr ("ДЕФ", "D_GRAFEM_DESC");
	AdvNo =	 GetItemNoByItemStr	("НАР",	"D_PART_OF_SPEECH");
	AdjNo =	 GetItemNoByItemStr	("ПРИЛ", "D_PART_OF_SPEECH");
	NumeralNo =	 GetItemNoByItemStr	("ЧИСЛ", "D_PART_OF_SPEECH");
	PossPronNo =  GetItemNoByItemStr ("ПРИТ_МЕСТМ",	"D_PART_OF_SPEECH");
	NominativeNo =	GetItemNoByItemStr ("И", "D_CASE");
	InstrumentalisNo =	GetItemNoByItemStr ("Т", "D_CASE");
	GenitivNo =	 GetItemNoByItemStr	("Р", "D_CASE");
	DativNo	=  GetItemNoByItemStr ("Д",	"D_CASE");
	VocativNo =	 GetItemNoByItemStr	("П", "D_CASE");
	AccusativNo	=  GetItemNoByItemStr ("В",	"D_CASE");
	NominativePluralisNo =	GetItemNoByItemStr ("И_мн",	"D_CASE_NUMBER");
	InstrumentalisPluralisNo =	GetItemNoByItemStr ("Т_мн",	"D_CASE_NUMBER");
	GenitivPluralisNo =	 GetItemNoByItemStr	("Р_мн", "D_CASE_NUMBER");
	DativPluralisNo	=  GetItemNoByItemStr ("Д_мн", "D_CASE_NUMBER");
	VocativPluralisNo =	 GetItemNoByItemStr	("П_мн", "D_CASE_NUMBER");

	NominativeSingularNo =	GetItemNoByItemStr ("И_ед",	"D_CASE_NUMBER");
	InstrumentalisSingularNo =	GetItemNoByItemStr ("Т_ед",	"D_CASE_NUMBER");
	GenitivSingularNo =	 GetItemNoByItemStr	("Р_ед", "D_CASE_NUMBER");
	DativSingularNo	=  GetItemNoByItemStr ("Д_ед", "D_CASE_NUMBER");
	VocativSingularNo =	 GetItemNoByItemStr	("П_ед", "D_CASE_NUMBER");
	AccusativSingularNo	=  GetItemNoByItemStr ("В_ед", "D_CASE_NUMBER");
	

	AccusativPluralisNo	=  GetItemNoByItemStr ("В_мн", "D_CASE_NUMBER");
	InstrumentalisAdjNo	=  GetItemNoByItemStr ("Т_ПРИЛ", "D_CASE_POS");
	InfinitiveNo =	GetItemNoByItemStr ("инф", "D_VP_SPECIF");
	NegativeNo =  GetItemNoByItemStr ("отр", "D_VP_SPECIF");
	PassiveNo =	 GetItemNoByItemStr	("стр",	"D_VP_SPECIF");
	QuoteMarkNo	=  GetItemNoByItemStr ("квч", "D_GRAFEM_DESC");
	ILENo =	 GetItemNoByItemStr	("ИЛЕ",	"D_GRAFEM_DESC");
	A0LexFunctNo = GetRoss()->GetItemNoByItemStr ("A0",	LexFunctDomNo);
	S0LexFunctNo = GetRoss()->GetItemNoByItemStr ("S0",	LexFunctDomNo);
	RightDirectionNo = GetItemNoByItemStr (">",	"D_POSITION");


	return true;
};

// ================	 функции для проверки извлечения и проверки	значений полей.
// поле	всегда адресуется следующими параметрами
//	FieldStr - имя поля
//	UnitNo - номер словарной статьи
//	LeafId - номер актанта
//	BracketLeafId -	номер подактанта



// По полю выдает набор	кортежей, которые ему приписано
void CRossHolder::GetFieldValues(string	strFieldName, long UnitNo, vector<TCortege>& vectorCorteges, BYTE LeafId, BYTE BracketLeafId) const	
{
	if (UnitNo == ErrUnitNo)	return;
	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for( int i = GetRoss()->GetUnitStartPos(UnitNo)	; i	<= GetRoss()->GetUnitEndPos(UnitNo)	; i++)
		{
			if(		(strFieldName == GetRoss()->Fields[GetRoss()->GetCortegeFieldNo(i)].FieldStr)	
				&&	(LeafId	== GetRoss()->GetCortegeLeafId(i))
				&&	(BracketLeafId == GetRoss()->GetCortegeBracketLeafId(i))
				)
				vectorCorteges.push_back(GetCortege(GetRoss(),i));		
		}
}



// Проверяет, стоит	ли на первом месте одного из кортежей поля strFieldName	константа strValue
bool CRossHolder::HasFieldValue(string strFieldName, string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
{
	vector<TCortege> corteges;
	GetFieldValues(strFieldName, UnitNo, corteges, LeafId);
	for(int	i =	0 ;	i <	corteges.size()	; i++ )
	{
		if(	corteges[i].m_DomItemNos[0]	!= -1 )
			if(	strValue ==	GetDomItemStrInner(corteges[i].m_DomItemNos[0])	 )
				return true;
	}

	return false;
}

// Проверяет, является ли одним	из значений	поля strFieldName константа	strValue
bool CRossHolder::HasFullFieldValue(string strFieldName, string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
{
	vector<TCortege> corteges;
	GetFieldValues(strFieldName, UnitNo, corteges, LeafId);
	Trim(strValue);
	for(int	i =	0 ;	i <	corteges.size()	; i++ )
	{
		TCortege& C	= corteges[i];
		string FullFieldValue =	WriteToString(GetRoss(), (char*)(GetRoss()->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
		Trim(FullFieldValue);
		if (FullFieldValue == strValue)	return true;
	}

	return false;
}


// По полю выдает набор	значений, которые приписаны	этому полю
void CRossHolder::GetFullFieldItemsFromArticle (long UnitNo, string	FieldStr, BYTE LeafId, BYTE	BracketLeafId, StringVector& Items) const
{
 Items.clear();
 BYTE FieldNo =	GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());
 if	(FieldNo ==	ErrUChar) return;

 if	(!GetRoss()->IsEmptyArticle(UnitNo))
	for	(size_t	i =	GetRoss()->GetUnitStartPos(UnitNo);	i<=	GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (	   (GetRoss()->GetCortegeFieldNo(i)	== FieldNo)	
			&& (GetRoss()->GetCortegeLeafId(i) == LeafId) 
			&& (GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId)	
		 )
	  {
		 const TCortege& C = GetCortege(GetRoss(), i);
		 string	FullFieldValue = WriteToString(GetRoss(), (char*)(GetRoss()->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
		 Items.push_back (FullFieldValue);
	  };
};


// По полю выдает набор	строковых констант,	которые	стоят на первом	месте кортежей,	которые	приписаны этому	полю
void CRossHolder::GetSimpleFieldItemsFromArticle (long UnitNo, string FieldStr,	BYTE LeafId, BYTE BracketLeafId, StringVector& Items)	const
{
 Items.clear();
 BYTE FieldNo =	GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());
 if	(FieldNo ==	ErrUChar) return;

 if	(!GetRoss()->IsEmptyArticle(UnitNo))
	for	(size_t	i =	GetRoss()->GetUnitStartPos(UnitNo);	i<=	GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (	   (GetRoss()->GetCortegeFieldNo(i)	== FieldNo)	
			&& (GetRoss()->GetCortegeLeafId(i) == LeafId) 
			&& (GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId)	
		 )
	  {
		 string	Item = GetDomItemStrInner(GetRoss()->GetCortegeItem(i,0));
		 Items.push_back (Item);
	  };
};

// функционально выполняет то же, что и	CRossHolder::HasFieldValue,	за исключением того, что на	вход подается
// имя домена, в котором сначала ищется	константа ItemStr. 
// Работает	немного	быстрее	CRossHolder::HasFieldValue,	поскольку вся работа со	строками 
// вынесена	за пределы основного цикла
bool CRossHolder::HasItem (WORD	UnitNo,	const string FieldStr, const string	ItemStr, const string DomStr, BYTE LeafId, BYTE	BracketLeafId) const
{
	if (UnitNo == ErrUnitNo)  return false;
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr.c_str());
	if (DomNo == ErrUChar) return false;
	long ItemNo	= GetRoss()->GetItemNoByItemStr(ItemStr.c_str(), DomNo);
	if (ItemNo == -1) return false;
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());

	if (!GetRoss()->IsEmptyArticle(UnitNo))
	for	(size_t	i =	GetRoss()->GetUnitStartPos(UnitNo);	i<=	GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (	  (GetRoss()->GetCortegeFieldNo(i) == FieldNo) 
		   && (GetRoss()->GetCortegeLeafId(i) == LeafId) 
		   && (GetRoss()->GetCortegeBracketLeafId(i) ==	BracketLeafId) 
		   && (GetRoss()->GetCortegeItem(i,0)  == ItemNo)
		 )
		 return	true;
	return false;
};

WORD CRossHolder::LocateUnit (const	char* UnitStr, BYTE	MeanNum) const
{
	return GetRoss()->LocateUnit(UnitStr, MeanNum);
};

long  CRossHolder::GetDopFields(long UnitNo, vector<CDopField>&	DopFields) const
{
  // в этом	векторе	все	узлы термина
  vector<long> AllItems;
   // в	этом векторе лежат узлы	термина, в которые вошло хотя бы одно отношение
  vector<long> SubItems;
  BYTE DopFldName =	GetRoss()->GetFieldNoByFieldStr("AUX");
  if (!GetRoss()->IsEmptyArticle(UnitNo)) 
  for (size_t k=GetRoss()->GetUnitStartPos(UnitNo);	k <= GetRoss()->GetUnitEndPos(UnitNo); k++)
	 if	(DopFldName	== GetRoss()->GetCortegeFieldNo(k))
	 {
		 TCortege C	= GetCortege(GetRoss(),	k);
		 if	(	(C.m_DomItemNos[0] == -1)
			 ||	(C.m_DomItemNos[1] == -1)
			 ||	(C.m_DomItemNos[2] == -1)
			)
		 continue;
		 CDopField Field;
		 string	S =	 GetDomItemStrInner(GetRoss()->GetCortegeItem(k,1));
		 //	 Word1 - зависимое слово
		 Field.m_IsActant1 = (S[0] == 'A');
		 Field.m_Word1 = S[1] -	'0';
		 S =  GetDomItemStrInner(GetRoss()->GetCortegeItem(k,2));
		 //	 Word1 - главное слово
		 Field.m_IsActant2 = (S[0] == 'A');
		 Field.m_Word2 = S[1] -	'0';
		 Field.m_RelationStr = GetDomItemStrInner(GetRoss()->GetCortegeItem(k,0));
		 DopFields.push_back(Field);
		 SubItems.push_back	(Field.m_Word1);
		 AllItems.push_back	(Field.m_Word1);
		 AllItems.push_back	(Field.m_Word2);
	 };

  long	Result = -1;
  // устанавливаем флаг	для	главного узла (узел, в который не вошло	ни одного отношения)
  for (long	i=0; i < AllItems.size();i++)
	  if (!_find (SubItems,	AllItems[i]))
	  {
		  Result = AllItems[i];
		  break;
	  };
  return  Result;

};


bool CRossHolder::GetVal(long UnitNo, CValency& V) const
{
 for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i<= GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if ( GetRoss()->GetCortegeFieldNo(i) ==  ValFieldNo )
	  {
		  V = CValency (GetCortege(GetRoss(),i), MainWordVarNo, this);
		  return true;
	  };
 return false; 
};

// эта функция не обнуляет  вектор OutVector!!!
void CRossHolder::GetLexFuncts (size_t UnitNo,  vector<CLexicalFunctionField>& OutVector, DictTypeEnum type /*NoneRoss*/, const CRossHolder* pRossHolderObor /*= NULL*/)  const
{
  if (!GetRoss()->IsEmptyArticle(UnitNo))
  for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i<= GetRoss()->GetUnitEndPos(UnitNo); i++)
 	  if  (GetRoss()->GetCortegeFieldNo(i) == LexFunctFieldNo) 
	  {
       string Lemma = GetDomItemStrInner(GetRoss()->GetCortegeItem(i,1));
	   EngRusMakeUpper(Lemma);
	   //Lemma = Lemma.SpanExcluding(" ");
	   string LexFunct = GetDomItemStrInner(GetRoss()->GetCortegeItem(i,0));
	   long MeanNum = -1;
	   CDictUnitInterp interp;
	    if (GetRoss()->GetCortegeItem(i,2) != -1)
		{
			string S  = GetDomItemStrInner(GetRoss()->GetCortegeItem(i,2));
			
			if (S.length() > 0)
			  if ( isdigit((unsigned char)S[0]) )
			     MeanNum = S[0] - '0';
			  else
			  {
				MeanNum = -1;
				if( pRossHolderObor && (type != NoneRoss))
				{
					int ii = S.find("+");
					if ( ii != -1 ) 
					{
						/*
						   отрезать все, что стоит после "+" нужно только, если
						   мы имеем дело с английским словарем 	 
						*/
						if (is_alpha((BYTE)S[0], morphEnglish))
						  S = S.substr(0, ii);

						long UnitNo = pRossHolderObor->LocateUnit(S.c_str(), 1);
						if( UnitNo != ErrUnitNo )
						{						
							interp.m_DictType = type;
							interp.m_UnitNo = UnitNo;
						}
					}
				}
			  }
		};

   	   OutVector.push_back (CLexicalFunctionField(LexFunct, Lemma, MeanNum));
	   if( interp.m_DictType != NoneRoss )
		   OutVector[OutVector.size() - 1].m_Prep = interp;

	  };

};


CValency CRossHolder::GetSemRelOfPrepOrConj(WORD  UnitNo) const
{
	if (     (UnitNo != ErrUnitNo) 
	     &&  (!GetRoss()->IsEmptyArticle(UnitNo)) 
	  )
     for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i<= GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if ( GetRoss()->GetCortegeFieldNo(i) ==  ValFieldNo )
        return  CValency (GetDomItemStrInner(GetCortege(GetRoss(),i).m_DomItemNos[0]), A_C, this);

	return CValency("ACT", A_C);
};

		
void rml_LOG( const	char* format, ... )
{
	va_list arglst;
	char s[10000];
	assert (strlen (format) <	 200);
	va_start(	arglst,	format );
	vsprintf(	s, format, arglst);
	va_end( arglst );
	FILE * fp	= fopen	("c:/seman.log", "a");
	fprintf(fp, "%s",	s);
	fclose(fp);
};


long CRossHolder::GetSemMainWordFromArticle (long UnitNo) const
{
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr("ROOT");

	if (GetRoss()->IsEmptyArticle(UnitNo) == false)
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i<= GetRoss()->GetUnitEndPos(UnitNo); i++)
			if (    (GetRoss()->GetCortegeFieldNo(i) == FieldNo) 
				)
			{
				string S = (const char*)GetRoss()->GetDomItemStr(GetRoss()->GetCortegeItem(i,0));
				return S[0] - '0';
			};
	return -1;
};


// проверяет, что ItemNo принадлежит домену Д_ГГ_уточн
bool CRossHolder::IsVerbFet (DWORD ItemNo) const
{
	if ( VerbFetDomNo == ErrUChar) return false;
	if (ItemNo ==  0xffffffff) return false;
	return GetRoss()->GetDomItemDomNo(ItemNo) == VerbFetDomNo ? true : false;
};
// проверяет, что ItemNo принадлежит домену D_SYN_REL
bool CRossHolder::IsSynRel (DWORD ItemNo) const
{
	if (ItemNo ==  0xffffffff) return false;
	return GetRoss()->GetDomItemDomNo(ItemNo) == SynRelDomNo ? true : false;
};

// проверяет, что ItemNo принадлежит домену D_1
bool CRossHolder::IsLemGroupBegining (DWORD ItemNo) const
{
	if (ItemNo ==  0xffffffff) return false;
	return GetRoss()->GetDomItemDomNo(ItemNo) == LemGroupBeginingDomNo ? true : false;
};

// проверяет, что ItemNo принадлежит домену D_GROUPS
bool CRossHolder::IsSynGroup (DWORD ItemNo) const
{
	if (ItemNo ==  0xffffffff) return false;
	return GetRoss()->GetDomItemDomNo(ItemNo) == SynGroupDomNo ? true : false;
};

// проверяет, что ItemNo принадлежит домену D_POSITION
bool CRossHolder::IsPosition (DWORD ItemNo) const
{
	if ( PositionDomNo == ErrUChar) return false;
	if (ItemNo ==  0xffffffff) return false;
	return (GetRoss()->GetDomItemDomNo(ItemNo) == PositionDomNo) ? true : false;
};

// выдает номер первой константы, из CortegeNo-го кортежа массиве m_GramCorteges 
// (Значение до запятой)
long	CRossHolder::GetSynRel(const  TCortege& C) const
{
	return (IsPosition(C.m_DomItemNos[0]) || IsVerbFet(C.m_DomItemNos[0]))
		? C.m_DomItemNos[1] : C.m_DomItemNos[0];
};

// выдает номер второй константы, из CortegeNo-го кортежа массиве m_GramCorteges 
// (Значение после запятой)1
long CRossHolder::GetSynFet(const  TCortege& C) const 
{
   return IsPosition(C.m_DomItemNos[0]) || IsVerbFet(C.m_DomItemNos[0]) 
	   ? C.m_DomItemNos[2] : C.m_DomItemNos[1];
};


long	CRossHolder::IsCompAdjCortege(const  TCortege& C) const
{
   long ItemNo = GetSynFet(C);
   return  (ItemNo != -1) 
	   &&  (string( GetDomItemStrInner(ItemNo)) == "сравн");
};


