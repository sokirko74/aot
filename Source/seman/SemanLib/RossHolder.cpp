#include "stdafx.h"
#include "RossHolder.h"
#include "morph_dict/common/utilit.h"

CRossHolder::CRossHolder(CTranslatorHolder* TranslatorHolder)
{
	GetDictName() = "unknown";
	m_LastUpdateTime = 0;
	m_bOwnDictionary = true;
	m_TranslatorHolder = TranslatorHolder;
};

CRossHolder::~CRossHolder()
{

};

std::string  CRossHolder::GetDictName() {
	return m_Ross.GetDictName();
}

bool CRossHolder::OpenRossHolder(const std::string strPathName, bool bDontLoadExamples)
{
	m_LastUpdateTime = 1;
	char RossDir[_MAX_PATH];
	strcpy(RossDir, strPathName.c_str());
	char* q = strrchr(RossDir, '\\');
	if (q == 0)
		q = strrchr(RossDir, '/');

	if (q == 0)
	{
		assert(q);
		return false;
	}
	*q = 0;


	m_Ross.Load(RossDir);

	if (!bDontLoadExamples)
		if (!m_Ross.ReadUnitComments())
			return false;

	InitConsts();
	GetDictName() = m_Ross.GetDictName();
	return true;
};


void CRossHolder::InitFieldsAndDomains() {
	SynRelDomNo = GetRoss()->GetDomenNoByDomStr("D_SYN_REL");
	SemFetDomNo = GetRoss()->GetDomenNoByDomStr("D_SF");
	SynGroupDomNo = GetRoss()->GetDomenNoByDomStr("D_GROUPS");
	LemGroupBeginingDomNo = GetRoss()->GetDomenNoByDomStr("D_1");
	ActDomNo = GetRoss()->ActantsDomNo;
	MNADomNo = GetRoss()->GetDomenNoByDomStr("D_MUA");
	PositionDomNo = GetRoss()->GetDomenNoByDomStr("D_POSITION");
	VerbFetDomNo = GetRoss()->GetDomenNoByDomStr("D_VP_SPECIF");
	LexFunctDomNo = GetRoss()->GetDomenNoByDomStr("D_LF");
	CaseDomNo = GetRoss()->GetDomenNoByDomStr("D_CASE");
	CaseNumberDomNo = GetRoss()->GetDomenNoByDomStr("D_CASE_NUMBER");


	ValFieldNo = GetRoss()->GetFieldNoByFieldStr("VAL");
	LexFunctFieldNo = GetRoss()->GetFieldNoByFieldStr("LF");
	GramFetFieldNo = GetRoss()->GetFieldNoByFieldStr("GF");
	RefFieldNo = GetRoss()->GetFieldNoByFieldStr("REF");
	POFieldNo = GetRoss()->GetFieldNoByFieldStr("DOMAIN");
	GramRestrFieldNo = GetRoss()->GetFieldNoByFieldStr("RESTR");
	LexFetFieldNo = GetRoss()->GetFieldNoByFieldStr("LEX");
	NESOVMFieldNo = GetRoss()->GetFieldNoByFieldStr("INCONS");
	ObligLexFetFieldNo = GetRoss()->GetFieldNoByFieldStr("MANLEX");
	ArticleFieldNo = GetRoss()->GetFieldNoByFieldStr("DETERM");
	SemFetFieldNo = GetRoss()->GetFieldNoByFieldStr("SF");

}

bool CRossHolder::InitConsts()
{
	InitFieldsAndDomains();

	InitDomainsConsts();

	if (is_null(MainWordVarNo)) return	false;

	MNANo = EmptyDomItemId;
	if (MNADomNo != ErrUChar)
		MNANo = GetItemNoByItemStr1("MUA", MNADomNo);

	S_And_InstrNo = EmptyDomItemId;
	if (LemGroupBeginingDomNo != ErrUChar)
		S_And_InstrNo = GetItemNoByItemStr1("с+Т", LemGroupBeginingDomNo);
	return true;
};

bool CRossHolder::HasCX(uint16_t UnitNo, const std::string CX, const std::string DomStr) const
{
	return HasItem(UnitNo, "SF", CX, DomStr, 0, 0);
};

const std::string& CRossHolder::GetDomItemStrWrapper(dom_item_id_t item_id) const
{
	if (is_null(item_id))
		return "";
	else
		return  m_Ross.GetDomItemStr(item_id);
};

const std::string& CRossHolder::GetDomItemStrWrapper1(size_t cortege_no, BYTE item_no) const {
	return GetDomItemStrWrapper(GetRoss()->GetCortege(cortege_no).GetItem(item_no));
}

dom_item_id_t CRossHolder::GetItemNoByItemStr1(const std::string& ItemStr, BYTE dom_no) const
{
	return GetRoss()->GetItemIdByItemStr(ItemStr, dom_no);
}


dom_item_id_t CRossHolder::GetItemNoByItemStr(const std::string& ItemStr, const	char* DomStr) const
{
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr);
	return GetItemNoByItemStr1(ItemStr, DomNo);
}



bool CRossHolder::InitDomainsConsts()
{
	SelfLabelNo = GetItemNoByItemStr1("C", ActDomNo);

	AdvAdjMainNo = GetItemNoByItemStr("НАР:нар_опр", "D_GF_MAIN");
	NounMainNo = GetItemNoByItemStr("СУЩ:ИГ", "D_GF_MAIN");
	VerbMainNo = GetItemNoByItemStr("ГЛ:ГГ", "D_GF_MAIN");
	AdjMainNo = GetItemNoByItemStr("ПРИЛ:с_опр", "D_GF_MAIN");
	PronMainNo = GetItemNoByItemStr("МЕСТОИМ:с_опр", "D_GF_MAIN");
	ClauseGrpNo = GetItemNoByItemStr("ПРИД_ПР", "D_GROUPS");
	NounGrpNo = GetItemNoByItemStr("ИГ", "D_GROUPS");
	AdverbialGrpNo = GetItemNoByItemStr("ОБСТ_ГР", "D_GROUPS");
	VerbGrpNo = GetItemNoByItemStr("ГГ", "D_GROUPS");
	CopulNo = GetItemNoByItemStr("Copul", "D_GROUPS");
	ModalCopulNo = GetItemNoByItemStr("ModalCopul", "D_GROUPS");
	NumerComplexNo = GetItemNoByItemStr("ЦК", "D_GRAFEM_DESC");
	NumerSymbComplexNo = GetItemNoByItemStr("ЦБК", "D_GRAFEM_DESC");
	HyphenNo = GetItemNoByItemStr("ДЕФ", "D_GRAFEM_DESC");
	AdvNo = GetItemNoByItemStr("НАР", "D_PART_OF_SPEECH");
	AdjNo = GetItemNoByItemStr("ПРИЛ", "D_PART_OF_SPEECH");
	NumeralNo = GetItemNoByItemStr("ЧИСЛ", "D_PART_OF_SPEECH");
	PossPronNo = GetItemNoByItemStr("ПРИТ_МЕСТМ", "D_PART_OF_SPEECH");

	NominativeNo = GetItemNoByItemStr1("И", CaseDomNo);
	InstrumentalisNo = GetItemNoByItemStr1("Т", CaseDomNo);
	GenitivNo = GetItemNoByItemStr1("Р", CaseDomNo);
	DativNo = GetItemNoByItemStr1("Д", CaseDomNo);
	VocativNo = GetItemNoByItemStr1("П", CaseDomNo);
	AccusativNo = GetItemNoByItemStr1("В", CaseDomNo);
	NominativePluralisNo = GetItemNoByItemStr1("И_мн", CaseNumberDomNo);
	InstrumentalisPluralisNo = GetItemNoByItemStr1("Т_мн", CaseNumberDomNo);
	GenitivPluralisNo = GetItemNoByItemStr1("Р_мн", CaseNumberDomNo);
	DativPluralisNo = GetItemNoByItemStr1("Д_мн", CaseNumberDomNo);
	VocativPluralisNo = GetItemNoByItemStr1("П_мн", CaseNumberDomNo);

	NominativeSingularNo = GetItemNoByItemStr1("И_ед", CaseNumberDomNo);
	InstrumentalisSingularNo = GetItemNoByItemStr1("Т_ед", CaseNumberDomNo);
	GenitivSingularNo = GetItemNoByItemStr1("Р_ед", CaseNumberDomNo);
	DativSingularNo = GetItemNoByItemStr1("Д_ед", CaseNumberDomNo);
	VocativSingularNo = GetItemNoByItemStr1("П_ед", CaseNumberDomNo);
	AccusativSingularNo = GetItemNoByItemStr1("В_ед", CaseNumberDomNo);
	AccusativPluralisNo = GetItemNoByItemStr1("В_мн", CaseNumberDomNo);

	InstrumentalisAdjNo = GetItemNoByItemStr("Т_ПРИЛ", "D_CASE_POS");
	InfinitiveNo = GetItemNoByItemStr("инф", "D_VP_SPECIF");
	NegativeNo = GetItemNoByItemStr("отр", "D_VP_SPECIF");
	PassiveNo = GetItemNoByItemStr("стр", "D_VP_SPECIF");
	QuoteMarkNo = GetItemNoByItemStr("квч", "D_GRAFEM_DESC");
	ILENo = GetItemNoByItemStr("ИЛЕ", "D_GRAFEM_DESC");
	A0LexFunctNo = GetItemNoByItemStr1("A0", LexFunctDomNo);
	S0LexFunctNo = GetItemNoByItemStr1("S0", LexFunctDomNo);
	RightDirectionNo = GetItemNoByItemStr(">", "D_POSITION");


	return true;
};

// ================	 функции для проверки извлечения и проверки	значений полей.
// поле	всегда адресуется следующими параметрами
//	FieldStr - имя поля
//	UnitNo - номер словарной статьи
//	LeafId - номер актанта
//	BracketLeafId -	номер подактанта



// По полю выдает набор	кортежей, которые ему приписано
void CRossHolder::GetFieldValues(std::string	strFieldName, long UnitNo, std::vector<TCortege>& vectorCorteges, BYTE LeafId, BYTE BracketLeafId) const
{
	if (UnitNo == ErrUnitNo)	return;
	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (int i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
		{
			if ((strFieldName == GetRoss()->Fields[GetRoss()->GetCortegeFieldNo(i)].FieldStr)
				&& (LeafId == GetRoss()->GetCortegeLeafId(i))
				&& (BracketLeafId == GetRoss()->GetCortegeBracketLeafId(i))
				)
				vectorCorteges.push_back(GetCortegeCopy(i));
		}
}



// Проверяет, стоит	ли на первом месте одного из кортежей поля strFieldName	константа strValue
bool CRossHolder::HasFieldValue(std::string strFieldName, std::string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
{
	std::vector<TCortege> corteges;
	GetFieldValues(strFieldName, UnitNo, corteges, LeafId);
	for (auto& c: corteges)
	{
		if (!c.is_null(0))
			if (strValue == GetDomItemStrWrapper(c.GetItem(0)))
				return true;
	}

	return false;
}

// Проверяет, является ли одним	из значений	поля strFieldName константа	strValue
bool CRossHolder::HasFullFieldValue(std::string strFieldName, std::string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
{
	std::vector<TCortege> corteges;
	GetFieldValues(strFieldName, UnitNo, corteges, LeafId);
	Trim(strValue);
	for (int i = 0; i < corteges.size(); i++)
	{
		TCortege& C = corteges[i];
		std::string FullFieldValue = GetRoss()->WriteToString(C);
		Trim(FullFieldValue);
		if (FullFieldValue == strValue)	return true;
	}

	return false;
}


// По полю выдает набор	значений, которые приписаны	этому полю
void CRossHolder::GetFullFieldItemsFromArticle(long UnitNo, std::string	FieldStr, BYTE LeafId, BYTE	BracketLeafId, StringVector& Items) const
{
	Items.clear();
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());
	if (FieldNo == ErrUChar) return;

	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if ((GetRoss()->GetCortegeFieldNo(i) == FieldNo)
				&& (GetRoss()->GetCortegeLeafId(i) == LeafId)
				&& (GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId)
				)
			{
				const TCortege& C = GetCortegeCopy(i);
				std::string	FullFieldValue = GetRoss()->WriteToString(C);
				Items.push_back(FullFieldValue);
			};
};


// По полю выдает набор	строковых констант,	которые	стоят на первом	месте кортежей,	которые	приписаны этому	полю
void CRossHolder::GetSimpleFieldItemsFromArticle(long UnitNo, std::string FieldStr, BYTE LeafId, BYTE BracketLeafId, StringVector& Items)	const
{
	Items.clear();
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());
	if (FieldNo == ErrUChar) return;

	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if ((GetRoss()->GetCortegeFieldNo(i) == FieldNo)
				&& (GetRoss()->GetCortegeLeafId(i) == LeafId)
				&& (GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId)
				)
			{
				std::string	Item = GetDomItemStrWrapper1(i, 0);
				Items.push_back(Item);
			};
};

// функционально выполняет то же, что и	CRossHolder::HasFieldValue,	за исключением того, что на	вход подается
// имя домена, в котором сначала ищется	константа ItemStr. 
// Работает	немного	быстрее	CRossHolder::HasFieldValue,	поскольку вся работа со	строками 
// вынесена	за пределы основного цикла
bool CRossHolder::HasItem(uint16_t	UnitNo, const std::string FieldStr, const std::string	ItemStr, const std::string DomStr, BYTE LeafId, BYTE	BracketLeafId) const
{
	if (UnitNo == ErrUnitNo)  return false;
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr.c_str());
	if (DomNo == ErrUChar) return false;
	dom_item_id_t ItemNo = GetItemNoByItemStr1(ItemStr, DomNo);
	if (is_null(ItemNo)) return false;
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr(FieldStr.c_str());

	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if ((GetRoss()->GetCortegeFieldNo(i) == FieldNo)
				&& (GetRoss()->GetCortegeLeafId(i) == LeafId)
				&& (GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId)
				&& (GetRoss()->GetCortege(i).GetItem(0) == ItemNo)
				)
				return	true;
	return false;
};

uint16_t CRossHolder::LocateUnit(const char* UnitStr, BYTE	MeanNum) const
{
	return GetRoss()->LocateUnit(UnitStr, MeanNum);
};

long  CRossHolder::GetDopFields(long UnitNo, std::vector<CDopField>& DopFields) const
{
	// в этом	векторе	все	узлы термина
	std::vector<long> AllItems;
	// в	этом векторе лежат узлы	термина, в которые вошло хотя бы одно отношение
	std::vector<long> SubItems;
	BYTE DopFldName = GetRoss()->GetFieldNoByFieldStr("AUX");
	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t k = GetRoss()->GetUnitStartPos(UnitNo); k <= GetRoss()->GetUnitLastPos(UnitNo); k++)
			if (DopFldName == GetRoss()->GetCortegeFieldNo(k))
			{
				TCortege C = GetCortegeCopy(k);
				if (!C.is_null(0) || !C.is_null(1) || !C.is_null(2))
					continue;
				CDopField Field;
				std::string	S = GetDomItemStrWrapper1(k, 1);
				//	 Word1 - зависимое слово
				Field.m_IsActant1 = (S[0] == 'A');
				Field.m_Word1 = S[1] - '0';
				S = GetDomItemStrWrapper1(k, 2);
				//	 Word1 - главное слово
				Field.m_IsActant2 = (S[0] == 'A');
				Field.m_Word2 = S[1] - '0';
				Field.m_RelationStr = GetDomItemStrWrapper1(k, 0);
				DopFields.push_back(Field);
				SubItems.push_back(Field.m_Word1);
				AllItems.push_back(Field.m_Word1);
				AllItems.push_back(Field.m_Word2);
			};

	long	Result = -1;
	// устанавливаем флаг	для	главного узла (узел, в который не вошло	ни одного отношения)
	for (long i = 0; i < AllItems.size();i++)
		if (!_find(SubItems, AllItems[i]))
		{
			Result = AllItems[i];
			break;
		};
	return  Result;

};


bool CRossHolder::GetVal(long UnitNo, CValency& V) const
{
	for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
		if (GetRoss()->GetCortegeFieldNo(i) == ValFieldNo)
		{
			V = CValency(GetCortegeCopy(i), MainWordVarNo, this);
			return true;
		};
	return false;
};

// эта функция не обнуляет  вектор OutVector!!!
void CRossHolder::GetLexFuncts(size_t UnitNo, std::vector<CLexicalFunctionField>& OutVector, DictTypeEnum type /*NoneRoss*/, const CRossHolder* pRossHolderObor /*= NULL*/)  const
{
	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if (GetRoss()->GetCortegeFieldNo(i) == LexFunctFieldNo)
			{
				std::string Lemma = GetDomItemStrWrapper1(i, 1);
				EngRusMakeUpper(Lemma);
				//Lemma = Lemma.SpanExcluding(" ");
				std::string LexFunct = GetDomItemStrWrapper1(i, 0);
				long MeanNum = -1;
				CDictUnitInterp interp;
				if (!is_null(GetRoss()->GetCortege(i).GetItem(2)))
				{
					std::string S = GetDomItemStrWrapper1(i, 2);

					if (S.length() > 0)
						if (isdigit((unsigned char)S[0]))
							MeanNum = S[0] - '0';
						else
						{
							MeanNum = -1;
							if (pRossHolderObor && (type != NoneRoss))
							{
								int ii = S.find("+");
								if (ii != -1)
								{
									/*
									   отрезать все, что стоит после "+" нужно только, если
									   мы имеем дело с английским словарем
									*/
									if (is_alpha((BYTE)S[0], morphEnglish))
										S = S.substr(0, ii);

									long UnitNo = pRossHolderObor->LocateUnit(S.c_str(), 1);
									if (UnitNo != ErrUnitNo)
									{
										interp.m_DictType = type;
										interp.m_UnitNo = UnitNo;
									}
								}
							}
						}
				};

				OutVector.push_back(CLexicalFunctionField(LexFunct, Lemma, MeanNum));
				if (interp.m_DictType != NoneRoss)
					OutVector[OutVector.size() - 1].m_Prep = interp;

			};

};


CValency CRossHolder::GetSemRelOfPrepOrConj(uint16_t  UnitNo) const
{
	if ((UnitNo != ErrUnitNo)
		&& (!GetRoss()->IsEmptyArticle(UnitNo))
		)
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if (GetRoss()->GetCortegeFieldNo(i) == ValFieldNo)
				return  CValency(GetDomItemStrWrapper1(i, 0), A_C, this);

	return CValency("ACT", A_C);
};


long CRossHolder::GetSemMainWordFromArticle(long UnitNo) const
{
	BYTE FieldNo = GetRoss()->GetFieldNoByFieldStr("ROOT");

	if (GetRoss()->IsEmptyArticle(UnitNo) == false)
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if ((GetRoss()->GetCortegeFieldNo(i) == FieldNo)
				)
			{
				std::string S = GetDomItemStrWrapper1(i, 0);
				return S[0] - '0';
			};
	return -1;
};



// проверяет, что ItemNo принадлежит домену Д_ГГ_уточн
bool CRossHolder::IsVerbFet(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == VerbFetDomNo;
};

// проверяет, что ItemNo принадлежит домену D_SYN_REL
bool CRossHolder::IsSynRel(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == SynRelDomNo;
};

// проверяет, что ItemNo принадлежит домену D_1
bool CRossHolder::IsLemGroupBegining(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == LemGroupBeginingDomNo;
};

// проверяет, что ItemNo принадлежит домену D_GROUPS
bool CRossHolder::IsSynGroup(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == SynGroupDomNo;
};

// проверяет, что ItemNo принадлежит домену D_POSITION
bool CRossHolder::IsPosition(dom_item_id_t item_id) const
{
	if (PositionDomNo == ErrUChar) return false;
	return get_dom_no(item_id) == PositionDomNo;
};

dom_item_id_t CRossHolder::GetSynRel(const  TCortege& C) const
{
	BYTE pos = (IsPosition(C.GetItem(0)) || IsVerbFet(C.GetItem(0))) ? 1 : 0;
	return C.GetItem(pos);
};

dom_item_id_t CRossHolder::GetSynFet(const  TCortege& C) const
{
	BYTE pos = (IsPosition(C.GetItem(0)) || IsVerbFet(C.GetItem(0))) ? 2 : 1;
	return C.GetItem(pos);
};


bool	CRossHolder::IsCompAdjCortege(const  TCortege& C) const
{
	dom_item_id_t item_id = GetSynFet(C);
	return  !is_null(item_id)
		&& (GetDomItemStrWrapper(item_id) == "сравн");
};

bool CRossHolder::IsCase(dom_item_id_t item_id) const {
	auto dom_no = get_dom_no(item_id);
	return dom_no == CaseDomNo || dom_no == CaseNumberDomNo;
}

TCortege CRossHolder::GetCortegeCopy(size_t cortege_no) const {
	return GetRoss()->GetCortegeCopy(cortege_no);

}


