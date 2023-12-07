// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma warning  (disable : 4530)
#include "StdRoss.h"
#include <stdio.h>
#include "morph_dict/common/bserialize.h"
#include "Ross.h"
#include "morph_dict/common/util_classes.h"
#include "TempArticle.h"


TRoss::TRoss() 
{
	m_bShouldSaveComments = false;
};


TRoss::~TRoss()
{
	m_Domens.clear();
	ClearFields();
	ClearUnits();
	ClearCorteges();
}

const std::string& TRoss::GetDictName() const {
	return Config.DictName;
}

const std::string& TRoss::GetDictFolder() const {
	return Config.DictFolder;
}

const TDictConfig& TRoss::GetConfig() const {
	return Config;
}


void MakePathAndCheck(const std::string path, const std::string fileName, std::string& fullPath) {
    fullPath = MakePath(path, fileName);
    if (!FileExists(fullPath.c_str())) {
        throw CExpc("File does not exist " + fullPath);
    }
}

std::string TRoss::GetCortegeFilePath() const {
	return MakePath(GetDictFolder(), "cortege.bin");
}

std::string TRoss::GetDomItemsFilePath() const {
	return MakePath(GetDictFolder(), "domitems.tsv");
}

std::string TRoss::GetUnitsFilePath() const {
	return MakePath(GetDictFolder(), "units.bin");
}

std::string TRoss::GetCommentsFilePath() const {
	return MakePath(GetDictFolder(), "comments.bin");
}

void TRoss::LoadDictScheme(std::string folder)
{
	Config.ReadConfig(folder);
	BuildDomens(MakePath(folder, "domains.json"));
	BuildDomItems(GetDomItemsFilePath());
	BuildFields(MakePath(folder, "fields.json"));

};


bool  TRoss::Save()
{
	if (m_bShouldSaveComments)
		WriteVector<TUnitComment>(GetCommentsFilePath(), m_UnitComments);

	WriteCorteges(GetCortegeFilePath().c_str());
	WriteVector<CStructEntry>(GetUnitsFilePath(), m_Units);
	WriteDomItems(GetDomItemsFilePath());
	return true;
};



const char* TRoss::GetTitleFieldName() const
{
	return "TITLE";
};


const char* TRoss::GetSenseFieldName() const
{
	return "SENSE";
};


const char* TRoss::GetCommFieldName() const
{
	return "COMM";
};

const char* TRoss::GetAuthorFieldName() const
{
	return "AUTHOR";
};

const char* TRoss::GetTimeCreatFieldName() const
{
	return "TIME_CREATE";
};

const char* TRoss::GetRedactFieldName() const
{
	return "REDACT";
};

void   TRoss::BuildUnits() {
	ClearUnits();
	ReadVector<CStructEntry>(GetUnitsFilePath(), m_Units);
}


bool   TRoss::ClearUnits()
{
	m_Units.clear();
	return true;
}

void   TRoss::ClearUnit(uint16_t UnitNo)
{
	if (!m_Units[UnitNo].HasEmptyArticle())
		DelCorteges(m_Units[UnitNo].m_StartCortegeNo, m_Units[UnitNo].m_LastCortegeNo + 1);
	m_Units[UnitNo].MakeEmpty();
}



void  TRoss::DelUnit(uint16_t unit_no)
{
	auto& unit = m_Units[unit_no];
	if (!unit.HasEmptyArticle())
		DelCorteges(unit.m_StartCortegeNo, unit.m_LastCortegeNo + 1);
	auto C = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(unit.m_EntryId));
	assert(C->m_EntryId == unit.m_EntryId);
	m_UnitComments.erase(C);
	m_Units.erase(m_Units.begin() + unit_no);
};


uint16_t TRoss::LocateUnit(const char* EntryStr, int MeanNum) const
{
	CStructEntry T(EntryStr, MeanNum);
	std::vector<CStructEntry>::const_iterator It = lower_bound(m_Units.begin(), m_Units.end(), T);
	if (It == m_Units.end()) return ErrUnitNo;
	if (!(T == *It)) return ErrUnitNo;
	return It - m_Units.begin();
};



uint16_t     TRoss::GetSelectedUnitNo(uint16_t i) const
{
	i++;

	uint16_t k = 0;

	for (; (i > 0) && (k < m_Units.size()); k++)
		if (m_Units[k].m_bSelected)
			i--;
	return k - 1;
};


uint16_t	TRoss::GetSelectedUnitsSize() const
{
	uint16_t  i = 0;
	uint16_t  k = 0;
	for (; k < m_Units.size(); k++)
		if (m_Units[k].m_bSelected)
			i++;

	return i;
};


uint16_t    TRoss::InsertUnit(CStructEntry& T)
{
	std::vector<CStructEntry>::iterator It = lower_bound(m_Units.begin(), m_Units.end(), T);
	T.m_EntryId = (m_UnitComments.size() == 0) ? 1 : m_UnitComments[m_UnitComments.size() - 1].m_EntryId + 1;
	uint16_t res = It - m_Units.begin();
	m_Units.insert(It, T);
	InsertUnitComment(T.m_EntryId);
	return res;
};

uint16_t   TRoss::InsertUnit(const char* EntryStr, BYTE MeanNum)
{
	return TRoss::InsertUnit(CStructEntry(EntryStr, MeanNum));
}

void TRoss::WriteUnitCorteges(uint16_t unit_no, const TCortegeContainer& corteges)
{
	auto& unit = m_Units[unit_no];

	if (!unit.HasEmptyArticle())
		DelCorteges(unit.m_StartCortegeNo, unit.m_LastCortegeNo + 1);

	unit.m_StartCortegeNo = _GetCortegesSize();
	ConcatOtherContainer(corteges);
	unit.m_LastCortegeNo = _GetCortegesSize() - 1;
	if (corteges._GetCortegesSize() == 0)
	{
		unit.MakeEmpty();
	};

}

const std::vector<CStructEntry>& TRoss::GetEntries()
{
	return m_Units;
};

uint16_t    TRoss::UnitsLowerBound(const char* EntryStr)  const
{
	return lower_bound(m_Units.begin(), m_Units.end(), CStructEntry(EntryStr, 1)) - m_Units.begin();
};



inline size_t get_size_in_bytes(const tm& t)
{
	return get_size_in_bytes(t.tm_hour) * 9;
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

inline size_t get_size_in_bytes(const TUnitComment& t)
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
	memcpy(i.Editor, buf, AuthorNameSize);
	buf += AuthorNameSize;

	memcpy(i.Comments, buf, UnitCommentSize);
	buf += UnitCommentSize;

	buf += restore_from_bytes(i.modif_tm, buf);
	return get_size_in_bytes(i);
};



bool   TRoss::ReadUnitComments()
{
	m_UnitComments.clear();
	ReadVector<TUnitComment>(GetCommentsFilePath(), m_UnitComments);
	sort(m_UnitComments.begin(), m_UnitComments.end());
	assert(m_UnitComments.size() == m_Units.size());
	m_bShouldSaveComments = true;
	return true;
};


uint16_t    TRoss::InsertUnitComment(uint16_t EntryId)
{
	TUnitComment C;
	C.m_EntryId = EntryId;
	std::vector<TUnitComment>::iterator Ic = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), C);
	uint16_t No = Ic - m_UnitComments.begin();
	m_UnitComments.insert(Ic, C);
	return No;
};


TUnitComment* TRoss::GetCommentsByUnitId(uint16_t EntryId)
{
	auto it = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(EntryId));
	if ((it == m_UnitComments.end()) || (it->m_EntryId != EntryId))
		throw CExpc("cannot find unit comment by entry id = %i", EntryId);
	return &(*it);
};


const TUnitComment* TRoss::GetCommentsByUnitId(uint16_t EntryId)   const
{
	auto it = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(EntryId));
	if ((it == m_UnitComments.end()) || (it->m_EntryId != EntryId))
		throw CExpc("cannot find unit comment by entry id = %i", EntryId);
	return &(*it);
};


void   TRoss::BuildCorteges()
{
	ClearCorteges();
	ReadCorteges(GetCortegeFilePath().c_str());
}


void TRoss::DelCorteges(size_t start, size_t last)
{
	for (size_t i = 0; i < m_Units.size(); i++)
		if (!m_Units[i].HasEmptyArticle()
			&& m_Units[i].m_StartCortegeNo >= last)
		{
			m_Units[i].m_StartCortegeNo -= (last - start);
			m_Units[i].m_LastCortegeNo -= (last - start);
		};

	EraseCorteges(start, last);
};





bool is_ascii_string(const std::string&  s) {
	for (auto c: s) {
		if (c < 0) {
			return false;
		}
	}
	return true;
}

bool is_empty_line(const char* t)
{
	if (*t == 0) return true;

	while ( (*t != '\r') && (*t != '\n'))
	{
		if ((BYTE)t[0] > ' ')
			return false;
		t++;
	}

	return true;
}


bool   TRoss::ReadFromStrWithOneSignatura(const char* s, TCortege& C, const CSignat& Sgn)
{ 
	std::vector<std::string> item_strs;
	const char* q = s;

	auto doms = Sgn.GetDomsWithDelims();
	size_t i = 0;
	for (; i < doms.size(); i++)
	{
		BYTE DomNo = doms[i].m_DomNo;
		bool IsMult = doms[i].m_IsMult;
		const CDomen& D = m_Domens[DomNo];
		bool IsDelim = D.DomainIsDelim();
		bool FlagLastItem = (i == doms.size() - 1);
		char delims[10];
		delims[0] = 0;

		bool FlagNextDelim = false;
		if (!FlagLastItem) {
			if ((i < doms.size() - 1)
				&& m_Domens[doms[i + 1].m_DomNo].DomainIsDelim()
				&& !m_Domens[doms[i + 1].m_DomNo].IsEmpty()
				) 
			{
				FlagNextDelim = true;
				strcat(delims, m_Domens[doms[i + 1].m_DomNo].GetFirstDomStr().c_str());
			}
		}

		if (!FlagLastItem && !FlagNextDelim)
			strcat(delims, " ");

		while (isspace((unsigned  char)q[0])) q++;

		size_t item_str_len = IsDelim ? 1 : strcspn(q, delims);
		std::string item_str(q, q + item_str_len);
		q += item_str_len;

		if (!IsDelim) {
			TrimRight(item_str);
		}

		/*
			если есть два формата для одного поля:
				D_ENGL
				D_RLE
			и пришло русское слово, тогда не будем добавлять его в
				в D_ENGL
		*/
		if (DomNo == EnglDomNo && (!startswith(item_str, "#") && !is_ascii_string(item_str)))
		{
			return false;
		}

		//если строка прежедвременно закончилалсь,
		// то выйти из процедуры с неудачей
		if (!FlagLastItem && is_empty_line(q))  return false;

		/*
		если данный элемент есть повторение некоторых констант их другого домена
		Например, "D_GRAMMEMS+" означает, что на этом месте стоит перечень элементов
		из домена D_GRAMMEMS, разделенных запятыми.
		Нужно проверить, что перечень синтаксически правилен и записать его в домен
		D_MULT одной константой.
		*/
		if (IsMult)
		{
			StringTokenizer tok(item_str.c_str(), " ,");
			while (tok())
				if (GetItemIdByItemStr(tok.val(), DomNo) == EmptyDomItemId)
					return false;
					
			DomNo = GetDomenNoByDomStr("D_MULT");
		};

		if (IsDelim) {
			if (D.GetFirstDomStr() != item_str) {
				return false;
			}
		}
		else {
			// #### Поиск найденной  строки в домене
			dom_item_id_t item_id;
			if (item_str == "*")
			{
				item_id = WildCardDomItemNo;
			}
			else {
				item_id = GetItemIdByItemStr(item_str, DomNo);
				if (!D.IsFree && (DomNo != LexPlusDomNo) && is_null(item_id))
					return false;
			};
			C.SetItem(item_strs.size(), item_id);
			item_strs.push_back(item_str);
		}
	};
	
	if (!is_empty_line(q) || (i < doms.size())) return false;

	for (size_t i = item_strs.size(); i < MaxNumDom; i++) {
		C.SetItem(i, EmptyDomItemId);
	}



	for (BYTE i = 0; i < item_strs.size(); i++) {
		if (C.is_null(i)) // не определено значение
		{
			dom_item_id_t item_id = InsertDomItem(item_strs[i].c_str(), Sgn.GetDomsWoDelims()[i]);

			if (is_null(item_id))
				return false;

			C.SetItem(i, item_id);
		};
	}
		
	return true;
};


bool   TRoss::ReadFromStr(const char* str, TCortege& C)
{
	for (const auto& s : Fields[C.m_FieldNo].m_Signats)
		if (ReadFromStrWithOneSignatura(str, C, s))
		{
			C.SetSignatId(s.SignatId);
			return true;
		};

	return false;
};


std::string TRoss::WriteToString(const TCortege& C) const
{
	const CSignat& signat = GetSignat(C);
	auto frmt = signat.GetFrmt();
	std::string result;
	BYTE item_index = 0;
	for (BYTE i = 0; i < frmt.length(); i++) {
		if ((frmt[i] == '%') && (i + 1 < frmt.length()) && (frmt[i + 1] == 's'))
		{
			dom_item_id_t itemId = C.GetItem(item_index);
			BYTE dom_no = get_dom_no(itemId);
			if (!is_null(itemId))
			{
				result += m_Domens[dom_no].GetDomItemStrById(itemId);
				++item_index;
			};
			i++;
		}
		else
			result += frmt[i];
	}
	return result;

};


const CSignat& TRoss::GetSignat(const TCortege& C) const {
	const CField& f = Fields[C.m_FieldNo];
	BYTE signat_no = f.SignatId2SignatNo[C.GetSignatId()];
	return f.m_Signats[signat_no];
}


void CDictionary::Load(const char* Path)
{
	if (!Path) {
		throw CExpc("cannot load ross by empty path");
	}
	LoadDictScheme(Path);
	BuildUnits();
	BuildCorteges();
};


void TRoss::SetUnitCommentStr(uint16_t UnitNo, const char* Str)
{
	TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	int l = strlen(Str);
	if (l > UnitCommentSize - 1)
		l = UnitCommentSize - 1;
	strncpy(C->Comments, Str, l);
	C->Comments[l] = 0;
}

void TRoss::SetUnitAuthor(uint16_t UnitNo, const char* Author)
{
	m_Units[UnitNo].SetUnitAuthor(Author);
}

tm Str2Tm(std::string TimeStr)
{
	tm output;
	sscanf(TimeStr.c_str(), "%i/%i/%i %i:%i:%i", &output.tm_mday, &output.tm_mon, &output.tm_year, &output.tm_hour, &output.tm_min, &output.tm_sec);
	output.tm_mon--;
	return output;
};


void TRoss::SetUnitModifTimeStr(uint16_t UnitNo, const char* TimeStr)
{
	TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	C->modif_tm = Str2Tm(TimeStr);
}

void TRoss::SetUnitEditor(uint16_t UnitNo, const char* Editor)
{
	TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	strcpy(C->Editor, Editor);
}

std::string TRoss::GetUnitModifTimeStr(uint16_t UnitNo) const
{
	char tmpbuf[128];
	const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	int year = C->modif_tm.tm_year;
	if (year > 100)
		year -= 100;
	sprintf(tmpbuf, "%i/%i/%i %i:%i:%i", C->modif_tm.tm_mday, C->modif_tm.tm_mon + 1, year,
		C->modif_tm.tm_hour, C->modif_tm.tm_min, C->modif_tm.tm_sec);
	return tmpbuf;
}

std::string TRoss::GetUnitTextHeader(uint16_t UnitNo) const
{
	std::string R;
	const CStructEntry& U = m_Units[UnitNo];
	const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);

	R += Format("%s        = %s\n", GetTitleFieldName(), U.GetEntryStr().c_str());
	R += Format("%s       = %u\n", GetSenseFieldName(), U.m_MeanNum);

	if (C && strlen(C->Comments))
		R += Format("%s       = %s\n", GetCommFieldName(), C->Comments);


	if (!U.GetAuthorStr().empty())
		R += Format("%s       = %s\n", GetAuthorFieldName(), U.GetAuthorStr().c_str());


	if (C && strlen(C->Editor))
		R += Format("%s       = %s\n", GetRedactFieldName(), C->Editor);

	std::string t = TRoss::GetUnitModifTimeStr(UnitNo);
	if (!t.empty())
		R += Format("%s       = %s\n", GetTimeCreatFieldName(), t.c_str());

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

CDictionary::CDictionary() : TRoss()
{
}

bool CDictionary::IsEmptyArticle(uint16_t UnitNo) const
{
	return m_Units[UnitNo].HasEmptyArticle();
};

int CDictionary::GetUnitStartPos(uint16_t UnitNo) const
{
	return m_Units[UnitNo].m_StartCortegeNo;
};

int CDictionary::GetUnitLastPos(uint16_t UnitNo) const
{
	return  m_Units[UnitNo].m_LastCortegeNo;
};

BYTE	CDictionary::GetFieldNoByFieldStr(const char* Str) const
{
	return  TRoss::GetFieldNoByFieldStrInner(Str);
};


const std::string empty_string;

const std::string& CDictionary::GetDomItemStr(dom_item_id_t item_id) const {
	if (is_null(item_id)) {
		return empty_string;
	}
	else {
		BYTE dom_no = get_dom_no(item_id);
		return m_Domens[dom_no].GetDomItemStrById(item_id);
	}
}

const std::string& CDictionary::GetDomItemStr(long cortegeNo, BYTE positionInCortege) const {
	return GetDomItemStr(GetCortege(cortegeNo).GetItem(positionInCortege));
}


std::string	CDictionary::GetEntryStr(uint16_t EntryNo) const
{
	return m_Units[EntryNo].GetEntryStr();
};


BYTE		CDictionary::GetUnitMeanNum(uint16_t EntryNo) const
{
	return m_Units[EntryNo].m_MeanNum;
};

bool CDictionary::IncludesArticle(uint16_t UnitNo, const CTempArticle* part) const
{
	CTempArticle Main(const_cast<CDictionary*>(this));
	Main.ReadFromDictionary(UnitNo, false, true);
	return part->IsPartOf(&Main, true);
}


//=============    Import from Text file   ==========================

bool GetValue(std::string Pair, std::string FldName, std::string& Value)
{
	StringTokenizer tok(Pair.c_str(), " \t");
	if (tok.next_token() != FldName) return false;
	if (tok.next_token() != "=") return false;
	Value = tok.get_rest();
	Trim(Value);
	return true;
};


void CDictionary::ProcessOneArticle(std::vector<CSourceLine>& L)
{
	std::string S;
	if (L.empty()) {
		throw article_parse_error("empty struct dict article", 0);
	}
	std::string Lemma;
	size_t line_no = 0;
	const char* Field = GetTitleFieldName();
	if (!GetValue(L[line_no].m_Line, Field, Lemma))
	{
		throw article_parse_error("Cannot read field TITLE", L[line_no].m_SourceLineNo);
	}

	Field = GetSenseFieldName();
	line_no += 1;
	if (!GetValue(L[line_no].m_Line, Field, S))
	{
		throw article_parse_error("Cannot read field SENSE", L[line_no].m_SourceLineNo);
	}

	BYTE MeanNum = atoi(S.c_str());
	if (MeanNum == 0)
	{
		throw article_parse_error("Cannot read SENSE", L[line_no].m_SourceLineNo);
	}
	if ((Config.MaxMeanNum == 1) && (MeanNum > 1))
	{
		throw article_parse_error("No different senses are possible  for this dictionary", L[0].m_SourceLineNo);
	}

	std::string Comments = "";
	Field = GetCommFieldName();
	if (line_no + 1 < L.size())
		if (GetValue(L[line_no + 1].m_Line, Field, S))
		{
			Comments = S;
			line_no++;
		};

	std::string Author = "";
	Field = GetAuthorFieldName();
	if (line_no + 1 < L.size()) {
		if (GetValue(L[line_no + 1].m_Line, Field, S))
		{
			Author = S;
			line_no++;
		};
	}
	std::string ModifTime = "";
	Field = GetTimeCreatFieldName();
	if (line_no + 1 < L.size()) {
		if (GetValue(L[line_no + 1].m_Line, Field, S))
		{
			ModifTime = S;
			line_no++;
		};
	}

	std::string UnitEditor = "";
	Field = GetRedactFieldName();
	if (line_no + 1 < L.size())
		if (GetValue(L[line_no + 1].m_Line, Field, S))
		{
			UnitEditor = S;
			line_no++;
		};

	Field = GetTimeCreatFieldName();
	if (line_no + 1 < L.size())
		if (GetValue(L[line_no + 1].m_Line, Field, S))
		{
			ModifTime = S;
			line_no++;
		};


	uint16_t UnitNo = LocateUnit(Lemma.c_str(), MeanNum);

	if (UnitNo != ErrUnitNo) {
		LOGV << "skip existing article " << Lemma;
		return;
	}
	else {
		UnitNo = InsertUnit(Lemma.c_str(), MeanNum);
		assert(UnitNo != ErrUnitNo);
	}
	
	std::string NewArticle;
	for (int i = line_no + 1; i < L.size(); i++)
		NewArticle += L[i].m_Line + std::string("\n");

	try
	{
		CTempArticle A1(this);
		A1.SetUnitNo(UnitNo);
		A1.ReadFromUtf8String(NewArticle.c_str());
		SetUnitCommentStr(UnitNo, Comments.c_str());
		SetUnitAuthor(UnitNo, Author.c_str());
		if (ModifTime != "")
			SetUnitModifTimeStr(UnitNo, ModifTime.c_str());
		if (UnitEditor != "")
			SetUnitEditor(UnitNo, UnitEditor.c_str());
		A1.WriteToDictionary();
	}
	catch (article_parse_error& a)
	{
		a.source_line_no += line_no + L[0].m_SourceLineNo;
		throw;
	}
	catch (CExpc& e)
	{
		throw article_parse_error(e.what(), L[0].m_SourceLineNo);
	}
};


static std::vector<std::vector<CSourceLine> > get_articles_from_file(std::string FileName) {
	std::vector<std::vector<CSourceLine> > articles;
	std::ifstream inp;
	inp.open(FileName);
	if (!inp.good()) {
		throw CExpc("Cannot read input file %s", FileName.c_str());
	};
	size_t line_no = 0;
	std::string line;
	std::vector<CSourceLine>  article_lines;
	while (std::getline(inp, line))
	{
		line_no++;
		int k = line.find("//");
		if (k != std::string::npos)
			line.erase(k);
		Trim(line);
		CSourceLine l(line, line_no);
		if (line.empty()) continue;

		if (line.find("====") != std::string::npos) {
			if (!article_lines.empty()) {
				articles.push_back(article_lines);
				article_lines = std::vector<CSourceLine>();
			}
		}
		else {
			article_lines.push_back(l);
		}
	};
	if (!article_lines.empty()) {
		articles.push_back(article_lines);
	}
	return articles;
}


void CDictionary::LoadAndExportDict(std::string folder, std::string fileName)
{
	
	Load(folder.c_str());

	if (!ReadUnitComments())
	{
		throw CExpc("Cannot load dictionary from %s", folder.c_str());
	};
	std::ofstream outf(fileName, std::ios::binary);
	if (!outf.is_open())
	{
		throw CExpc("Cannot write to %s", fileName.c_str());
	};

	CTempArticle A(this);

	for (uint16_t i = 0; i < m_Units.size(); i++)
	{
		A.ReadFromDictionary(i, true, true);
		outf << "============\n" << GetUnitTextHeader(i);
		outf << A.GetArticleStrUtf8(true);
	};
}



void CDictionary::SetUnitCurrentTime(uint16_t UnitNo)
{
	TUnitComment* C = GetCommentsByUnitId(GetEntries()[UnitNo].m_EntryId);
	C->modif_tm = RmlGetCurrentTime();
}

std::string CDictionary::GetUnitEditor(uint16_t UnitNo) const
{
	const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
	return C->Editor;
}

void  CDictionary::SetUnitStr(uint16_t UnitNo, const char* UnitStr)
{
	m_Units[UnitNo].SetEntryStr(UnitStr);
	sort(m_Units.begin(), m_Units.end());
}

void CDictionary::ImportFromTextFile(std::string fileName) {
	if (!FileExists(fileName.c_str()))
	{
		throw CExpc("Cannot read %s", fileName.c_str());
	};
	m_bShouldSaveComments = true;

	auto articles = get_articles_from_file(fileName);
	LOGV << "Number of found entries: " << articles.size();
	for (auto& a : articles)
	{
		ProcessOneArticle(a);
	};
}
