#include "stdafx.h"
#include "struct_dict_holder.h"
#include "morph_dict/common/utilit.h"

CStructDictHolder::CStructDictHolder(DictTypeEnum dictType)
{
	m_LastUpdateTime = 0;
	m_DictType = dictType;
};

bool CStructDictHolder::HasBeenModified(long T) const
{
	return m_LastUpdateTime > T;
};

void CStructDictHolder::SetUpdateTime(long T) 
{
	m_LastUpdateTime = T;
}

CStructDictHolder::~CStructDictHolder()
{

};

std::string  CStructDictHolder::GetDictName() const {
	return m_Ross.GetDictName();
}

CDictionary* CStructDictHolder::GetRoss() { 
	return &m_Ross; 
}

DictTypeEnum CStructDictHolder::GetRossType() const {
	return m_DictType; 
}

const CDictionary* CStructDictHolder::GetRoss() const {
	return &m_Ross; 
}

const std::string& CStructDictHolder::GetDictPath() const {
	return m_Ross.GetConfig().DictFolder; 
}


bool CStructDictHolder::LoadStructDict(const std::string directory, bool bDontLoadExamples, long last_update_time)
{
	m_LastUpdateTime = last_update_time;
	m_Ross.Load(directory.c_str());
	if (!bDontLoadExamples)
		if (!m_Ross.ReadUnitComments())
			return false;
	InitConsts();
	return true;
};


void CStructDictHolder::InitFieldsAndDomains() {
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

static TCortege GetSubjCortege(const CStructDictHolder* RossDoc)
{
	TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;
	C.m_LeafId = 1;
	C.m_LevelId = 0;
	bool b = RossDoc->GetRoss()->ReadFromStrConst("подл : И", C);
	if (!b) {
		throw CExpc("GetSubjCortege failed");
	}
	return C;
};

static TCortege GetFullGleicheRightCortege(CStructDictHolder* RossDoc, BYTE LeafId)
{
	TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;
	C.m_LeafId = LeafId;
	C.m_LevelId = 0;
	bool b = RossDoc->GetRoss()->ReadFromStr("> X! : ИГ", C);
	if (!b) {
		throw CExpc("GetFullGleicheRightCortege failed");
	}
	return C;
};


static TCortege GetIndirObjInGenitive(CStructDictHolder* RossDoc)
{
	TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;
	C.m_LeafId = 2;
	C.m_LevelId = 0;
	bool b = RossDoc->GetRoss()->ReadFromStr("к_доп : Р", C);
	if (!b) {
		throw CExpc("GetIndirObjInGenitive failed");
	}
	return C;
};

static TCortege GetInstrObj(CStructDictHolder* RossDoc)
{
	TCortege C;
	C.m_FieldNo = RossDoc->GramFetFieldNo;
	C.m_LeafId = 2;
	C.m_LevelId = 0;
	bool b = RossDoc->GetRoss()->ReadFromStr("к_доп : Т", C);
	if (!b) {
		throw CExpc("GetInstrObj failed");
	}
	return C;
};



bool CStructDictHolder::InitConsts()
{
	InitFieldsAndDomains();

	InitDomainsConsts();

	if (m_DictType == Ross || m_DictType == CollocRoss || m_DictType == TimeRoss) {
		rus_subj_gf = GetSubjCortege(this);
		adj_gf_1 = GetFullGleicheRightCortege(this, 1);
		adj_gf_2 = GetFullGleicheRightCortege(this, 2);
		indir_genitive_gf = GetIndirObjInGenitive(this);
		indir_instr_gf = GetInstrObj(this);
	}
	
	return true;
};

bool CStructDictHolder::HasCX(uint16_t UnitNo, const std::string CX, const std::string DomStr) const
{
	return HasItem(UnitNo, "SF", CX, DomStr, 0, 0);
};

const std::string empty_string;
const std::string& CStructDictHolder::GetDomItemStrWrapper(dom_item_id_t item_id) const
{
	if (is_null(item_id))
		return empty_string;
	else
		return  m_Ross.GetDomItemStr(item_id);
};

const std::string& CStructDictHolder::GetDomItemStrWrapper1(size_t cortege_no, BYTE item_no) const {
	return GetDomItemStrWrapper(GetRoss()->GetCortege(cortege_no).GetItem(item_no));
}

dom_item_id_t CStructDictHolder::GetItemNoByItemStr1(const std::string& ItemStr, BYTE dom_no, bool throw_exception) const
{
	auto item_id = GetRoss()->GetItemIdByItemStr(ItemStr, dom_no);
	if (throw_exception && is_null(item_id)) {
		throw CExpc("cannot find %s in domain %zi, struct dict name %s", ItemStr.c_str(), dom_no, m_Ross.GetDictName());
	}
	return item_id;
}


dom_item_id_t CStructDictHolder::GetItemNoByItemStr(const std::string& ItemStr, const char* DomStr, bool throw_exception) const
{
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr);
	return GetItemNoByItemStr1(ItemStr, DomNo, throw_exception);
}


void CStructDictHolder::InitDomainsConsts()
{
	SelfLabelNo = GetItemNoByItemStr1("C", ActDomNo);
	bool is_english = GetDictLanguage(m_DictType) == morphEnglish;
	bool is_russian = GetDictLanguage(m_DictType) == morphRussian;
	AdvAdjMainNo = GetItemNoByItemStr("НАР:нар_опр", "D_GF_MAIN", is_russian);
	NounMainNo = GetItemNoByItemStr("СУЩ:ИГ", "D_GF_MAIN", is_russian);
	VerbMainNo = GetItemNoByItemStr("ГЛ:ГГ", "D_GF_MAIN", is_russian);
	AdjMainNo = GetItemNoByItemStr("ПРИЛ:с_опр", "D_GF_MAIN", is_russian);
	PronMainNo = GetItemNoByItemStr("МЕСТОИМ:с_опр", "D_GF_MAIN", is_russian);
	ClauseGrpNo = GetItemNoByItemStr("ПРИД_ПР", "D_GROUPS", false);
	NounGrpNo = GetItemNoByItemStr("ИГ", "D_GROUPS", is_russian);
	AdverbialGrpNo = GetItemNoByItemStr("ОБСТ_ГР", "D_GROUPS", false);
	VerbGrpNo = GetItemNoByItemStr("ГГ", "D_GROUPS", is_russian);
	CopulNo = GetItemNoByItemStr("Copul", "D_GROUPS", false);
	ModalCopulNo = GetItemNoByItemStr("ModalCopul", "D_GROUPS", false);
	NumerComplexNo = GetItemNoByItemStr("ЦК", "D_GRAFEM_DESC");
	NumerSymbComplexNo = GetItemNoByItemStr("ЦБК", "D_GRAFEM_DESC");
	HyphenNo = GetItemNoByItemStr("ДЕФ", "D_GRAFEM_DESC");
	AdvNo = GetItemNoByItemStr("НАР", "D_PART_OF_SPEECH", false);
	AdjNo = GetItemNoByItemStr("ПРИЛ", "D_PART_OF_SPEECH", false);
	NumeralNo = GetItemNoByItemStr("ЧИСЛ", "D_PART_OF_SPEECH", false);
	PossPronNo = GetItemNoByItemStr("ПРИТ_МЕСТМ", "D_PART_OF_SPEECH", false);

	NominativeNo = GetItemNoByItemStr1("И", CaseDomNo, is_russian);
	InstrumentalisNo = GetItemNoByItemStr1("Т", CaseDomNo, is_russian);
	GenitivNo = GetItemNoByItemStr1("Р", CaseDomNo, is_russian);
	DativNo = GetItemNoByItemStr1("Д", CaseDomNo, is_russian);
	VocativNo = GetItemNoByItemStr1("П", CaseDomNo, is_russian);
	AccusativNo = GetItemNoByItemStr1("В", CaseDomNo, is_russian);

	NominativePluralisNo = GetItemNoByItemStr1("И_мн", CaseNumberDomNo, false);
	InstrumentalisPluralisNo = GetItemNoByItemStr1("Т_мн", CaseNumberDomNo, false);
	GenitivPluralisNo = GetItemNoByItemStr1("Р_мн", CaseNumberDomNo, false);
	DativPluralisNo = GetItemNoByItemStr1("Д_мн", CaseNumberDomNo, false);
	VocativPluralisNo = GetItemNoByItemStr1("П_мн", CaseNumberDomNo, false);

	NominativeSingularNo = GetItemNoByItemStr1("И_ед", CaseNumberDomNo, false);
	InstrumentalisSingularNo = GetItemNoByItemStr1("Т_ед", CaseNumberDomNo, false);
	GenitivSingularNo = GetItemNoByItemStr1("Р_ед", CaseNumberDomNo, false);
	DativSingularNo = GetItemNoByItemStr1("Д_ед", CaseNumberDomNo, false);
	VocativSingularNo = GetItemNoByItemStr1("П_ед", CaseNumberDomNo, false);
	AccusativSingularNo = GetItemNoByItemStr1("В_ед", CaseNumberDomNo, false);
	AccusativPluralisNo = GetItemNoByItemStr1("В_мн", CaseNumberDomNo, false);


	InstrumentalisAdjNo = GetItemNoByItemStr("Т_ПРИЛ", "D_CASE_POS");
	InfinitiveNo = GetItemNoByItemStr("инф", "D_VP_SPECIF", is_russian);
	NegativeNo = GetItemNoByItemStr("отр", "D_VP_SPECIF", false);
	PassiveNo = GetItemNoByItemStr("стр", "D_VP_SPECIF", false);
	QuoteMarkNo = GetItemNoByItemStr("квч", "D_GRAFEM_DESC");
	ILENo = GetItemNoByItemStr("ИЛЕ", "D_GRAFEM_DESC", false);
	A0LexFunctNo = GetItemNoByItemStr1("A0", LexFunctDomNo, false);
	S0LexFunctNo = GetItemNoByItemStr1("S0", LexFunctDomNo, false);
	RightDirectionNo = GetItemNoByItemStr(">", "D_POSITION", false);

	IndirObjSynONo = GetItemNoByItemStr1("к_доп", SynRelDomNo, is_russian);
	ReverseSynONo = GetItemNoByItemStr1("X!", SynRelDomNo);
	UncoordAttrSynONo = GetItemNoByItemStr1("нс_опр", SynRelDomNo, is_russian);
	CoordAttrSynONo = GetItemNoByItemStr1("с_опр", SynRelDomNo, is_russian);
	DirectObjSynONo = GetItemNoByItemStr1("п_доп", SynRelDomNo, is_russian);
	SubjSynONo = GetItemNoByItemStr1("подл", SynRelDomNo, is_russian);
	EnglSubjSynONo = GetItemNoByItemStr1("subj", SynRelDomNo, is_english);
	ParatAttrSynONo = GetItemNoByItemStr1("прим_опр", SynRelDomNo, is_russian);
	
};

// ================	 функции для проверки извлечения и проверки	значений полей.
// поле	всегда адресуется следующими параметрами
//	FieldStr - имя поля
//	UnitNo - номер словарной статьи
//	LeafId - номер актанта
//	BracketLeafId -	номер подактанта



// По полю выдает набор	кортежей, которые ему приписано
void CStructDictHolder::GetFieldValues(std::string	strFieldName, long UnitNo, std::vector<TCortege>& vectorCorteges, BYTE LeafId, BYTE BracketLeafId) const
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
bool CStructDictHolder::HasFieldValue(std::string strFieldName, std::string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
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
bool CStructDictHolder::HasFullFieldValue(std::string strFieldName, std::string	strValue, long UnitNo, BYTE	LeafId /*= 0  */, BYTE BracketLeafId) const
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
void CStructDictHolder::GetFullFieldItemsFromArticle(long UnitNo, std::string	FieldStr, BYTE LeafId, BYTE	BracketLeafId, StringVector& Items) const
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
void CStructDictHolder::GetSimpleFieldItemsFromArticle(long UnitNo, std::string FieldStr, BYTE LeafId, BYTE BracketLeafId, StringVector& Items)	const
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

// функционально выполняет то же, что и	CStructDictHolder::HasFieldValue,	за исключением того, что на	вход подается
// имя домена, в котором сначала ищется	константа ItemStr. 
// Работает	немного	быстрее	CStructDictHolder::HasFieldValue,	поскольку вся работа со	строками 
// вынесена	за пределы основного цикла
bool CStructDictHolder::HasItem(uint16_t	UnitNo, const std::string FieldStr, const std::string	ItemStr, const std::string DomStr, BYTE LeafId, BYTE	BracketLeafId) const
{
	if (UnitNo == ErrUnitNo)  return false;
	BYTE DomNo = GetRoss()->GetDomenNoByDomStr(DomStr.c_str());
	if (DomNo == ErrUChar) return false;
	dom_item_id_t ItemNo = GetItemNoByItemStr1(ItemStr, DomNo, false);
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

uint16_t CStructDictHolder::LocateUnit(const char* UnitStr, BYTE	MeanNum) const
{
	return GetRoss()->LocateUnit(UnitStr, MeanNum);
};

long  CStructDictHolder::GetDopFields(long UnitNo, std::vector<CDopField>& DopFields) const
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
				TCortege c = GetCortegeCopy(k);
				if (c.is_null(0) || c.is_null(1) || c.is_null(2))
					throw CExpc("bad format in field AUX, dict = %s, entry %s", GetRoss()->GetDictName().c_str(),
						GetRoss()->GetEntryStr(UnitNo).c_str());
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


bool CStructDictHolder::GetVal(long UnitNo, CValency& V) const
{
	for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
		if (GetRoss()->GetCortegeFieldNo(i) == ValFieldNo)
		{
			V = CValency(GetRoss()->GetCortege(i), this);
			return true;
		};
	return false;
};

// эта функция не обнуляет  вектор OutVector!!!
void CStructDictHolder::GetLexFuncts(size_t UnitNo, std::vector<CLexicalFunctionField>& OutVector, DictTypeEnum type /*NoneRoss*/, const CStructDictHolder* pRossHolderObor /*= NULL*/)  const
{
	if (!GetRoss()->IsEmptyArticle(UnitNo))
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if (GetRoss()->GetCortegeFieldNo(i) == LexFunctFieldNo)
			{
				std::string Lemma = GetDomItemStrWrapper1(i, 1);
				MakeUpperUtf8(Lemma);
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
									if (is_english_alpha((BYTE)S[0]))
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


CValency CStructDictHolder::GetSemRelOfPrepOrConj(uint16_t  UnitNo) const
{
	if ((UnitNo != ErrUnitNo)
		&& (!GetRoss()->IsEmptyArticle(UnitNo))
		)
		for (size_t i = GetRoss()->GetUnitStartPos(UnitNo); i <= GetRoss()->GetUnitLastPos(UnitNo); i++)
			if (GetRoss()->GetCortegeFieldNo(i) == ValFieldNo)
				return  CValency(GetDomItemStrWrapper1(i, 0), A_C, this);

	return CValency("ACT", A_C);
};


long CStructDictHolder::GetSemMainWordFromArticle(long UnitNo) const
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
bool CStructDictHolder::IsVerbFet(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == VerbFetDomNo;
};

// проверяет, что ItemNo принадлежит домену D_SYN_REL
bool CStructDictHolder::IsSynRel(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == SynRelDomNo;
};

// проверяет, что ItemNo принадлежит домену D_1
bool CStructDictHolder::IsLemGroupBegining(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == LemGroupBeginingDomNo;
};

// проверяет, что ItemNo принадлежит домену D_GROUPS
bool CStructDictHolder::IsSynGroup(dom_item_id_t item_id) const
{
	return get_dom_no(item_id) == SynGroupDomNo;
};

// проверяет, что ItemNo принадлежит домену D_POSITION
bool CStructDictHolder::IsPosition(dom_item_id_t item_id) const
{
	if (PositionDomNo == ErrUChar) return false;
	return get_dom_no(item_id) == PositionDomNo;
};

dom_item_id_t CStructDictHolder::GetSynRel(const  TCortege& C) const
{
	BYTE pos = (IsPosition(C.GetItem(0)) || IsVerbFet(C.GetItem(0))) ? 1 : 0;
	return C.GetItem(pos);
};

dom_item_id_t CStructDictHolder::GetSynFet(const  TCortege& C) const
{
	BYTE pos = (IsPosition(C.GetItem(0)) || IsVerbFet(C.GetItem(0))) ? 2 : 1;
	return C.GetItem(pos);
};


bool	CStructDictHolder::IsCompAdjCortege(const  TCortege& C) const
{
	dom_item_id_t item_id = GetSynFet(C);
	return  !is_null(item_id)
		&& (GetDomItemStrWrapper(item_id) == "сравн");
};

bool CStructDictHolder::IsCase(dom_item_id_t item_id) const {
	auto dom_no = get_dom_no(item_id);
	return dom_no == CaseDomNo || dom_no == CaseNumberDomNo;
}

TCortege CStructDictHolder::GetCortegeCopy(size_t cortege_no) const {
	return GetRoss()->GetCortegeCopy(cortege_no);

}

bool CStructDictHolder::IsGenitiv(dom_item_id_t ItemNo) const
{
	return     (ItemNo == GenitivSingularNo)
		|| (ItemNo == GenitivPluralisNo)
		|| (ItemNo == GenitivNo);
};


