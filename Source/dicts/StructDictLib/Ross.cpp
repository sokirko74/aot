// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma warning  (disable : 4530)
#include "StdRoss.h"
#include <stdio.h>
#include "morph_dict/common/bserialize.h"
#include "Ross.h"
#include "morph_dict/common/util_classes.h"
#include "TempArticle.h"


inline size_t get_size_in_bytes(const CStructEntry& t)
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
	return get_size_in_bytes(i) + 3;
};

inline size_t restore_from_bytes(CStructEntry& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_EntryId, buf);
	memcpy(i.m_EntryStr, buf, EntryStrSize);
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
	return get_size_in_bytes(i) + 3;
};



//=========================================================
//============           TRoss                 ============
//=========================================================
TRoss::TRoss(BYTE MaxNumDom) : TCortegeContainer(MaxNumDom)
{
	m_bShouldSaveComments = false;
	m_bDontLoadExamples = false;
	m_MaxMeanNum = 7;

};


TRoss::~TRoss()
{
	m_Domens.clear();
	ClearFields();
	ClearUnits();
	ClearCorteges();
}

void MakePathAndCheck(const std::string path, const std::string fileName, std::string& fullPath) {
    fullPath = MakePath(path, fileName);
    if (!FileExists(fullPath.c_str())) {
        throw CExpc("File does not exist " + fullPath);
    }
}


void TRoss::LoadOnlyConstants(const char* _RossPath)
{
	RossPath = _RossPath;
	MakePathAndCheck(RossPath, "config.txt", ConfigFile);
	MakePathAndCheck(RossPath, "domitems.tsv", DomItemsTextFile);
	MakePathAndCheck(RossPath, "items.bin", ItemsFile);
	MakePathAndCheck(RossPath, "domains.json", DomensFile);
	MakePathAndCheck(RossPath, "fields.json", FieldsFile);

	if (!ReadConfig())
	{
		throw CExpc("Cannot parse struct_dict config ");
	};

	{
		char LastReadLine[1000];
		if (!BuildDomens(LastReadLine))
		{
            throw CExpc(" Cannot build domens: the last read line=%s", LastReadLine);
		}
	};

	if (!BuildDomItems())
	{
		throw CExpc("Cannot build domitems");
	};

	BuildFields(m_MaxNumDom);
	CortegeFile = MakePath(RossPath, "cortege.bin");
	UnitsFile = MakePath(RossPath, "units.bin");
	UnitCommentsFile = MakePath(RossPath, "comments.bin");
};

bool TRoss::FullLoad(const char* _RossPath)
{
	LoadOnlyConstants(_RossPath);

	MakePathAndCheck(RossPath, "cortege.bin", CortegeFile);
	MakePathAndCheck(RossPath, "units.bin", UnitsFile);
	BuildUnits();

	if (!BuildCorteges())
	{
		m_LastError = "Cannot build corteges";
		return false;
	}

	return true;
}

bool  TRoss::Save()
{
	if (m_bShouldSaveComments)
		WriteVector<TUnitComment>(UnitCommentsFile, m_UnitComments);

	WriteCorteges(CortegeFile.c_str());
	WriteVector<CStructEntry>(UnitsFile, m_Units);
	WriteDomItems();
	WriteFields();
	WriteDomens();
	return true;

};

bool	TRoss::ReadConfig()
{
	std::string Config;
	{
		FILE* fp = fopen(ConfigFile.c_str(), "rb");
		if (!fp) return false;
		char buffer[1024];
		while (fgets(buffer, 1024, fp))
			Config += buffer;
		fclose(fp);
	};

	StringTokenizer lines(Config.c_str(), "\n\r");
	while (lines())
	{
		std::string Line = lines.val();
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer Items(Line.c_str(), " \t\n\r");

		std::string Field = Items.next_token();
		std::string Value = Items.next_token();
		if (Field.empty() || Value.empty()) return  false;
		if (Field == "MaxNumDom")
		{
			m_MaxNumDom = atoi(Value.c_str());
			if ((m_MaxNumDom != 3)
				&& (m_MaxNumDom != 10)
				)
				return false;
		}
		else
			if (Field == "MaxMeanNum")
			{
				int u = atoi(Value.c_str());
				if ((u < 1)
					|| (u > 15)
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

bool IsBinFile(const std::string FileName) {
	return     (FileName.length() > 3)
		&& FileName.substr(FileName.length() - 3) == std::string("bin");
};

void   TRoss::BuildUnits() {
	ClearUnits();

	if (IsBinFile(UnitsFile))
		ReadVector<CStructEntry>(UnitsFile, m_Units);
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
	m_Units[UnitNo].m_StartCortegeNo = InitialStartPos;
	m_Units[UnitNo].m_LastCortegeNo = InitialEndPos;
}



void  TRoss::DelUnit(std::vector<CStructEntry>::iterator It)
{
	if (!It->HasEmptyArticle())
		DelCorteges(It->m_StartCortegeNo, It->m_LastCortegeNo + 1);
	std::vector<TUnitComment>::iterator C = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(It->m_EntryId));
	assert(C->m_EntryId == It->m_EntryId);
	m_UnitComments.erase(C);
	m_Units.erase(It);
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
	CStructEntry T;
	T.m_MeanNum = MeanNum;
	strcpy(T.m_EntryStr, EntryStr);
	T.m_AuthorStr[0] = 0;
	return TRoss::InsertUnit(T);
}



static void   EstablishOneToOneCorrespondenceBetweenEntriesAndComments(TRoss& R)
{
	assert(!R.m_Units.empty());
	R.m_UnitComments.clear();
	for (size_t i = 0; i < R.m_Units.size(); i++)
	{
		R.m_Units[i].m_EntryId = i;
		R.InsertUnitComment((uint16_t)i);
	};
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

	UnitCommentsFile[0] = 0;

	MakePathAndCheck(RossPath, "comments.bin", UnitCommentsFile);
	if (!IsBinFile(UnitCommentsFile)) return false;


	ReadVector<TUnitComment>(UnitCommentsFile, m_UnitComments);
	sort(m_UnitComments.begin(), m_UnitComments.end());

	//  handle the error with comments in some previous versions
	if (m_UnitComments.size() != m_Units.size())
	{
		EstablishOneToOneCorrespondenceBetweenEntriesAndComments(*this);
	};

	assert(m_UnitComments.size() == m_Units.size());

	m_bShouldSaveComments = true;
	return true;
};

uint16_t    TRoss::InsertUnitComment(uint16_t EntryId)
{
	try {
		TUnitComment C;
		C.m_EntryId = EntryId;
		std::vector<TUnitComment>::iterator Ic = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), C);
		uint16_t No = Ic - m_UnitComments.begin();
		m_UnitComments.insert(Ic, C);
		return No;
	}
	catch (...)
	{
		ErrorMessage("Error in inserting one UnitComment");
		return 0;
	};
};

TUnitComment* TRoss::GetCommentsByUnitId(uint16_t EntryId)
{
	std::vector<TUnitComment>::iterator It = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(EntryId));
	assert((It != m_UnitComments.end())
		&& (It->m_EntryId == EntryId)
	);
	return &(*It);
};

const TUnitComment* TRoss::GetCommentsByUnitId(uint16_t EntryId)   const
{
	std::vector<TUnitComment>::const_iterator It = lower_bound(m_UnitComments.begin(), m_UnitComments.end(), TUnitComment(EntryId));
	assert((It != m_UnitComments.end())
		&& (It->m_EntryId == EntryId)
	);
	return &(*It);
};


bool   TRoss::BuildCorteges()
{
	ClearCorteges();

	if (IsBinFile(CortegeFile))
		ReadCorteges(CortegeFile.c_str());

	return true;
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



struct TItemStr
{
	char ItemStr[100];
	TItemStr(char* s)
	{
		strcpy(ItemStr, s);
	};

};


bool   TRoss::ReadFromStrWithOneSignatura(const char* s, TCortege10& C, const CSignat& Sgn)
{ 
	int CurrItemNo = 0;

	std::vector<TItemStr> ItemStrVec;
	const char* q = s;

	auto doms = Sgn.GetDomsWithDelims();
	size_t i = 0;
	for (; i < doms.size(); i++)
	{
		BYTE DomNo = doms[i].m_DomNo;
		bool IsMult = doms[i].m_IsMult;
		bool IsDelim = m_Domens[DomNo].DomainIsDelim();
		bool FlagLastItem = (i == doms.size() - 1);
		char Delim[10];
		Delim[0] = 0;

		bool FlagNextDelim = false;
		if (!FlagLastItem)
			if ((i < doms.size() - 1)
				&& m_Domens[doms[i + 1].m_DomNo].DomainIsDelim()
				&& !m_Domens[doms[i + 1].m_DomNo].IsEmpty()
				)
				FlagNextDelim = true;

		if (FlagNextDelim)
			strcat(Delim, m_Domens[doms[i + 1].m_DomNo].GetFirstDomStr().c_str());

		if (!FlagLastItem && !FlagNextDelim)
			strcat(Delim, " ");

		while (isspace((unsigned  char)q[0])) q++;

		char ItemStr[100];
		size_t ItemStrLen = IsDelim ? 1 : strcspn(q, Delim);
		if (ItemStrLen > 99)  return false;
		strncpy(ItemStr, q, ItemStrLen);
		ItemStr[ItemStrLen] = 0;
		if (!IsDelim) rtrim(ItemStr);

		/*
			если есть два формата для одного поля:
				D_ENGL
				D_RLE
			и пришло русское слово, тогда не будем добавлять его в
				в D_ENGL
		*/
		if (IsRussian(ItemStr))
			if (m_Domens[DomNo].GetDomStr() == "D_ENGL")
				return false;


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
			StringTokenizer tok(ItemStr, " ,");
			while (tok())
				if (GetItemIdByItemStr(tok.val(), DomNo) == EmptyDomItemId)
					return false;

			DomNo = GetDomenNoByDomStr("D_MULT");
		};

		// #### Поиск найденной  строки в домене
		if ((strlen(ItemStr) == 1)
			&& ((unsigned char)ItemStr[0] == '*'))
		{
			C.SetItem(CurrItemNo, WildCardDomItemNo);
		}
		else
			C.SetItem(CurrItemNo, GetItemIdByItemStr(ItemStr, DomNo));



		if ( !m_Domens[DomNo].IsFree  && (DomNo != LexPlusDomNo) && C.is_null(CurrItemNo))
			// Отрицательный результат
			return false;

		if (!IsDelim)
		{
			ItemStrVec.push_back(ItemStr);
			CurrItemNo++;
		};

	};


	if (!IsEmptyLine(q) || (i < doms.size())) return false;

	for (size_t i = ItemStrVec.size(); i < m_MaxNumDom; i++)
		C.SetItem(i, EmptyDomItemId);



	for (size_t i = 0; i < ItemStrVec.size(); i++)
		if (C.is_null(i)) // не определено значение
		{
			dom_item_id_t item_id = InsertDomItem(ItemStrVec[i].ItemStr, Sgn.GetDomsWoDelims()[i]);

			if (is_null(item_id))
				return false;

			C.SetItem(i, item_id);
		};

	return true;
};


bool   TRoss::ReadFromStr(const char* str, TCortege10& C)
{
	for (const auto& s : Fields[C.m_FieldNo].m_Signats)
		if (ReadFromStrWithOneSignatura(str, C, s))
		{
			C.SetSignatId(s.SignatId);
			return true;
		};

	return false;
};



std::string TRoss::WriteToString(const TCortege10& C) const
{
	const CSignat& signat = GetSignat(C);
	auto frmt = signat.GetFrmt();
	std::string result;
	BYTE item_index = 0;
	for (BYTE i = 0; i < frmt.length(); i++) {
		if ((frmt[i] == '%') && (i + 1 < frmt.length()) && (frmt[i + 1] == 's'))
		{
			dom_item_id_t itemId = C.GetItem(item_index);
			BYTE dom_no = signat.GetDomsWoDelims()[item_index];
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


const CSignat& TRoss::GetSignat(const TCortege10& C) const {
	const CField& f = Fields[C.m_FieldNo];
	BYTE signat_no = f.SignatId2SignatNo[C.GetSignatId()];
	return f.m_Signats[signat_no];
}


inline bool IsTitle(const char* s)
{
	if (!s) return false;
	for (int i = 0; i < strlen(s); i++)
		if (isdigit((unsigned char)s[i])
			)
			return false;
	return true;

};

dom_item_id_t   TRoss::InsertDomItem(const char* ItemStr, BYTE DomNo)
{
	if (DomNo == TitleDomNo)
		if (!IsTitle(ItemStr))
		{
			throw CExpc("Warning! Cannot add \"%s\" to title domen!", ItemStr);
		};


	if (DomNo == LexDomNo)
		if (!IsStandardRusLexeme(ItemStr))
		{
			throw CExpc("Warning! Cannot add \"%s\" to lexeme domen!", ItemStr);
		};

	if (DomNo == LexPlusDomNo)
	{
		DomNo = GetDomNoForLePlus(ItemStr);
		if (DomNo == ErrUChar)
		{
			throw CExpc("Warning! Cannot add \"%s\" to the extended lexeme domen!", ItemStr);
		};
	};

	return  m_Domens[DomNo].AddItemByEditor(ItemStr);
};


bool CDictionary::Load(const char* Path)
{
	if (!Path) {
		m_LastError = "cannot load ross by empty path";
		TItemContainer::ErrorMessage(m_LastError);
		return false;
	}
	if (!FullLoad(Path))
	{
		TItemContainer::ErrorMessage(m_LastError);
		return  false;
	}
	else
		return   true;

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
	int l = strlen(Author);
	if (l > AuthorNameSize - 1)
		l = AuthorNameSize - 1;
	strncpy(m_Units[UnitNo].m_AuthorStr, Author, l);
	m_Units[UnitNo].m_AuthorStr[l] = 0;
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
	try {
		TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		C->modif_tm = Str2Tm(TimeStr);
	}
	catch (...)
	{
		::ErrorMessage("StructDict", "Error in setting UnitComments");
	};
}

void TRoss::SetUnitEditor(uint16_t UnitNo, const char* Editor)
{
	try
	{
		TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		strcpy(C->Editor, Editor);
	}
	catch (...)
	{
		::ErrorMessage("StructDict", "Error in setting UnitEditor");
	};
}

std::string TRoss::GetUnitModifTimeStr(uint16_t UnitNo) const
{
	try {
		char tmpbuf[128];
		const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		int year = C->modif_tm.tm_year;
		if (year > 100)
			year -= 100;
		sprintf(tmpbuf, "%i/%i/%i %i:%i:%i", C->modif_tm.tm_mday, C->modif_tm.tm_mon + 1, year,
			C->modif_tm.tm_hour, C->modif_tm.tm_min, C->modif_tm.tm_sec);
		return tmpbuf;
	}
	catch (...)
	{
		ErrorMessage("Error in getting UnitComments");
		return "";
	};
}

std::string TRoss::GetUnitTextHeader(uint16_t UnitNo) const
{
	std::string R;
	const CStructEntry& U = m_Units[UnitNo];
	const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);

	R += Format("%s        = %s\n", GetTitleFieldName(), U.m_EntryStr);
	R += Format("%s       = %u\n", GetSenseFieldName(), U.m_MeanNum);

	if (C && strlen(C->Comments))
		R += Format("%s       = %s\n", GetCommFieldName(), C->Comments);


	if (strlen(U.m_AuthorStr) > 0)
		R += Format("%s       = %s\n", GetAuthorFieldName(), U.m_AuthorStr);


	if (C && strlen(C->Editor))
		R += Format("%s       = %s\n", GetRedactFieldName(), C->Editor);

	std::string t = TRoss::GetUnitModifTimeStr(UnitNo);
	if (!t.empty())
		R += Format("%s       = %s\n", GetTimeCreatFieldName(), t.c_str());

	return convert_to_utf8(R, m_Language);
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
std::vector<CStructEntry>& CDictionary::GetUnits()
{
	return m_Units;
};
bool CDictionary::IsEmptyArticle(uint16_t UnitNo) const
{
	return m_Units[UnitNo].HasEmptyArticle();
};

int CDictionary::GetUnitStartPos(uint16_t UnitNo) const
{
	return m_Units[UnitNo].m_StartCortegeNo;
};

int CDictionary::GetUnitEndPos(uint16_t UnitNo) const
{
	return  m_Units[UnitNo].m_LastCortegeNo;
};

BYTE	CDictionary::GetFieldNoByFieldStr(const char* Str) const
{
	return  TRoss::GetFieldNoByFieldStrInner(Str);
};



const std::string& CDictionary::GetDomItemStr(dom_item_id_t item_id) const {
	if (is_null(item_id)) {
		return "";
	}
	else {
		BYTE dom_no = get_dom_no(item_id);
		return m_Domens[dom_no].GetDomItemStrById(item_id);
	}
}

const std::string& CDictionary::GetDomItemStr(long cortegeNo, BYTE positionInCortege) const {
	return GetDomItemStr(GetCortegeItem(cortegeNo, positionInCortege));
}


std::string	CDictionary::GetEntryStr(uint16_t EntryNo) const
{
	return m_Units[EntryNo].m_EntryStr;
};

std::string	CDictionary::GetEntryStrUtf8(uint16_t EntryNo) const
{
	return convert_to_utf8(m_Units[EntryNo].m_EntryStr, this->m_Language);
};

BYTE		CDictionary::GetUnitMeanNum(uint16_t EntryNo) const
{
	return m_Units[EntryNo].m_MeanNum;
};

bool CDictionary::IncludeArticle(uint16_t UnitNo, std::string ArticleUtf8) const
{
	CTempArticle A1(const_cast<CDictionary*>(this));
	A1.ReadFromDictionary(UnitNo, false, true);

	CTempArticle A2(const_cast<CDictionary*>(this));
	A2.ReadFromUtf8String(ArticleUtf8.c_str());
	A2.MarkUp();
	A2.BuildCortegeList();
	return A2.IsPartOf(&A1, true);
}


//=============    Import from Text file   ==========================


bool IsRubicon(const std::string& S)
{
	return S.find("====") != std::string::npos;
}


bool FindRubicon(std::vector<CSourceLine>& L, size_t& pos)
{
	while ((pos < L.size())
		&& (!IsRubicon(L[pos].m_Line))
		)
		pos++;

	return pos < L.size();
}

void DeleteEmptyLines(std::vector<CSourceLine>& L)
{
	for (int i = 0; i < L.size(); )
	{
		Trim(L[i].m_Line);
		if (L[i].m_Line.empty())
			L.erase(L.begin() + i);
		else
			i++;
	};
};

int NumArt(std::vector<CSourceLine>& L)
{
	int Res = 0;

	for (int i = 0; i + 1 < L.size(); i++)
		if (IsRubicon(L[i].m_Line))
			Res++;

	return Res;
};


bool GetValue(std::string Pair, std::string FldName, std::string& Value)
{
	StringTokenizer tok(Pair.c_str(), " \t");
	if (tok.next_token() != FldName) return false;
	if (tok.next_token() != "=") return false;
	Value = tok.get_rest();
	Trim(Value);
	return true;
};


void CutComments(std::vector<CSourceLine>& L)
{
	for (size_t i = 0; i < L.size(); i++)
	{
		int k = L[i].m_Line.find("//");
		if (k != std::string::npos)
			L[i].m_Line.erase(k);
	}
}

void AddMessage(std::string Message, int LineNo, std::string& Messages)
{
	Trim(Message);
	if (LineNo != -1)
		Message += Format(" (line %i)", LineNo);
	Message += "\n";
	Messages += Message;
};


bool CDictionary::ImportFromText(std::string FileName, int StartEntry, std::string& Messages)
{
	Messages = "";

	std::vector<CSourceLine> L;
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
			std::string S = convert_from_utf8(buffer, m_Language);
			Trim(S);
			L.push_back(CSourceLine(S, CurrentLineNo));
			CurrentLineNo++;
		};
		fclose(fp);
	}


	int ErrorsCount = 0;

	CutComments(L);
	DeleteEmptyLines(L);
	size_t NumOfArt = NumArt(L);
	Messages += Format("Number of found entries: %i\n", NumOfArt);
	size_t start = 0;
	size_t last = 0;
	size_t NumOfGoodArt = 0;


	for (int i = 0; i < NumOfArt; i++)
	{
		if (FindRubicon(L, start))
			start++;
		last = start;
		FindRubicon(L, last);
		if (i + 1 < StartEntry) continue;

		if (ProcessOneArticle(L, start, last, Messages))
			NumOfGoodArt++;
		else
			ErrorsCount++;
	};

	Messages += Format("Number of loaded entries: %i\n", NumOfGoodArt);
	return ErrorsCount == 0;
}


bool CDictionary::ProcessOneArticle(std::vector<CSourceLine>& L, int start, int last, std::string& Messages)
{
	size_t RealStart = start;
	if (L.size() == 1) return false;
	std::string S;
	if (GetValue(L[RealStart].m_Line, _R("Дескриптор"), S)) {
		RealStart++;
	}

	std::string Lemma;
	const char* Field = GetTitleFieldName();
	if (!GetValue(L[RealStart].m_Line, Field, Lemma))
	{
		AddMessage(Format("Cannot read field %s\n", Field).c_str(), L[start].m_SourceLineNo + 1, Messages);
		return false;
	}
	RealStart++;

	Field = GetSenseFieldName();
	if (!GetValue(L[RealStart].m_Line, Field, S))
	{
		AddMessage(Format("Cannot read field %s", Field).c_str(), L[start].m_SourceLineNo + 1, Messages);
		return false;
	}

	BYTE MeanNum = atoi(S.c_str());
	if (MeanNum == 0)
	{
		AddMessage(Format("Cannot read field %s", Field).c_str(), L[start].m_SourceLineNo + 1, Messages);
		return false;
	}
	if ((m_MaxMeanNum == 1) && (MeanNum > 1))
	{
		AddMessage("No different senses are possible  for this dictionary", L[start].m_SourceLineNo + 1, Messages);
		return false;
	}

	std::string Comments = "";
	Field = GetCommFieldName();
	if (RealStart + 1 < L.size())
		if (GetValue(L[RealStart + 1].m_Line, Field, S))
		{
			Comments = S;
			RealStart++;
		};

	std::string Author = "";
	Field = GetAuthorFieldName();
	if (RealStart + 1 < L.size()) {
		if (GetValue(L[RealStart + 1].m_Line, Field, S))
		{
			Author = S;
			RealStart++;
		};
	}
	std::string ModifTime = "";
	Field = GetTimeCreatFieldName();
	if (RealStart + 1 < L.size()) {
		if (GetValue(L[RealStart + 1].m_Line, Field, S))
		{
			ModifTime = S;
			RealStart++;
		};
	}

	std::string UnitEditor = "";
	Field = GetRedactFieldName();
	if (RealStart + 1 < L.size())
		if (GetValue(L[RealStart + 1].m_Line, Field, S))
		{
			UnitEditor = S;
			RealStart++;
		};

	Field = GetTimeCreatFieldName();
	if (RealStart + 1 < L.size())
		if (GetValue(L[RealStart + 1].m_Line, Field, S))
		{
			ModifTime = S;
			RealStart++;
		};


	uint16_t UnitNo = LocateUnit(Lemma.c_str(), MeanNum);

	if (UnitNo != ErrUnitNo) {
		return true;
	}
	else {
		UnitNo = InsertUnit(Lemma.c_str(), MeanNum);
	}

	CTempArticle A1(this);
	if (UnitNo != ErrUnitNo)
		A1.ReadFromDictionary(UnitNo, false, false);

	std::string NewArticle;
	for (int i = RealStart + 1; i < last; i++)
		NewArticle += L[i].m_Line + std::string("\n");

	CTempArticle A2(this);
	try
	{
		if ( !A1.ReadFromUtf8String(convert_to_utf8(NewArticle, m_Language).c_str()) )
		{
			int LocalLineNo = A1.m_ErrorLine - 1;
			if (LocalLineNo < 0)
				LocalLineNo = 0;
			AddMessage(A1.m_LastError, L[LocalLineNo + start + (RealStart - start)].m_SourceLineNo + 1, Messages);
			if (A1.m_LastError.empty())
				Messages += "an error occurred!\n";
			return false;
		};
		SetUnitCommentStr(UnitNo, Comments.c_str());
		SetUnitAuthor(UnitNo, Author.c_str());
		if (ModifTime != "")
			SetUnitModifTimeStr(UnitNo, ModifTime.c_str());
		if (UnitEditor != "")
			SetUnitEditor(UnitNo, UnitEditor.c_str());
		A1.WriteToDictionary();
	}
	catch (...)
	{
		int LocalLineNo = A2.m_ErrorLine - 1;
		if (LocalLineNo < 0)
			LocalLineNo = 0;
		AddMessage(m_LastError, L[LocalLineNo + start + (RealStart - start)].m_SourceLineNo + 1, Messages);
		return false;
	}


	return true;
};

void CDictionary::SetUnitCurrentTime(uint16_t UnitNo)
{
	TUnitComment* C = GetCommentsByUnitId(GetUnits()[UnitNo].m_EntryId);
	C->modif_tm = RmlGetCurrentTime();
}




std::string CDictionary::GetUnitEditor(uint16_t UnitNo) const
{
	try
	{
		const TUnitComment* C = GetCommentsByUnitId(m_Units[UnitNo].m_EntryId);
		return C->Editor;
	}
	catch (...)
	{
		ErrorMessage("Error in setting UnitEditor");
	};
	return "";
}

void  CDictionary::SetUnitStr(uint16_t UnitNo, const char* UnitStr)
{
	strcpy(m_Units[UnitNo].m_EntryStr, UnitStr);
	sort(m_Units.begin(), m_Units.end());
}
