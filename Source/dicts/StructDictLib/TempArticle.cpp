// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#include "StdRoss.h"
#include "Ross.h"
#include "StructDictConsts.h"
#include "TempArticle.h"
#include "morph_dict/common/util_classes.h"

article_parse_error::article_parse_error(const std::string what_arg, int _source_line_no) :
	std::exception(what_arg.c_str()), source_line_no(_source_line_no) {

}

const char* article_parse_error::what() const  {
	message = Format("%s, line no = %i", std::exception::what(), source_line_no);
	return message.c_str();
}


CTempArticle::CTempArticle(CDictionary* pRoss) : TCortegeContainer()
{
	m_pRoss = pRoss;
	m_ReadOnly = false;
	m_UnitNo = ErrUnitNo;
	
}



const TCortege& CTempArticle::GetRossCortege(size_t i) const
{
	return m_pRoss->GetCortege(i);
};


size_t CTempArticle::GetCortegesSize() const
{
	if (m_ReadOnly)
		if ((m_UnitNo != ErrUnitNo) && (m_pRoss->m_Units[m_UnitNo].HasEmptyArticle()))
			return 0;
		else
			return m_pRoss->m_Units[m_UnitNo].m_LastCortegeNo - m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo + 1;
	else
		return _GetCortegesSize();
};


const TCortege& CTempArticle::GetCortege(size_t i)  const
{
	if (m_ReadOnly)
		return GetRossCortege(m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo + i);
	else
		return TCortegeContainer::GetCortege(i);
};


std::string CTempArticle::ConstructFldName(BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId)
{
	std::string s(m_pRoss->Fields[FieldNo].FieldStr);

	if (LeafId > 0)
		s += (LeafId == ErrUChar) ? '*' : ('0' + LeafId);

	if (BracketLeafId > 0)
	{
		s += '(';
		s += (BracketLeafId == ErrUChar) ? '*' : ('0' + BracketLeafId);
		s += ')';
	};

	return s;
};


int CTempArticle::IntersectByFields(const CTempArticle* Article) const
{

	size_t sum = 0;

	for (size_t i = 0; i < GetCortegesSize(); i++)
		for (size_t k = 0; k < Article->GetCortegesSize(); k++)
			if ((GetCortege(i).m_FieldNo == Article->GetCortege(k).m_FieldNo)
				&& (GetCortege(i).m_LeafId == Article->GetCortege(k).m_LeafId)
				&& (GetCortege(i).m_BracketLeafId == Article->GetCortege(k).m_BracketLeafId)
				)
				sum++;

	return sum;
}


bool CTempArticle::AddArticle(const CTempArticle* Article)
{
	assert(!m_ReadOnly);

	for (size_t k = 0; k < Article->GetCortegesSize(); k++)
	{
		size_t i = 0;
		for (; i < GetCortegesSize(); i++)
			if (GetCortege(i).EqualCortege(Article->GetCortege(k)))
				break;

		if (i == GetCortegesSize())
			_AddCortege(Article->GetCortege(k));
	};

	CheckCortegeVector();

	return true;
}


bool CTempArticle::IsPartOf(const CTempArticle* Article, bool UseWildCards) const
{
	// normally one of the articles is very short, no sense to sort
	for (size_t i = 0; i < GetCortegesSize(); i++)
	{
		size_t k = 0;

		for (; k < Article->GetCortegesSize(); k++) {
			if (UseWildCards) {
				if (GetCortege(i).IsEqualWithWildCard(Article->GetCortege(k), m_pRoss->WildCardDomItemNo)) {
					break;
				}
			}
			else {
				if (GetCortege(i).EqualCortege(Article->GetCortege(k))) {
					break;
				}
			}
		}

		if (k == Article->GetCortegesSize())
			return false;
	};


	return true;
}


const size_t MaxOfTheFirstPart = 200;

inline int  VisualFieldOrder(BYTE LeafId, BYTE BracketLeafId, int OrderId)
{
	if ((LeafId == 0) && (BracketLeafId == 0))
		return OrderId;
	else
		return OrderId + MaxOfTheFirstPart + LeafId * MaxOfTheFirstPart + BracketLeafId;
}


bool   CTempArticle::PutCortegeOnTheRigthPosition(const TCortege& C)
{
	// Номер места получим в переменной i. Вначале переменная i равна нулю.
  // Проходим все поля, у которых порядок меньше порядка текущего поля
	size_t i = 0;
	for (;
		(i < GetCortegesSize())
		&& (VisualFieldOrder(C.m_LeafId, C.m_BracketLeafId, m_pRoss->Fields[C.m_FieldNo].OrderId) >
			VisualFieldOrder(GetCortege(i).m_LeafId, GetCortege(i).m_BracketLeafId, m_pRoss->Fields[GetCortege(i).m_FieldNo].OrderId));
		i++);

	//  Проходим записи, которые относятся к данному полю, чтобы дописать текущую запись
	//  после последней.
	//  Опираемся на то, что OrderId - уникален
	for (;
		(i < GetCortegesSize())
		&& (C.m_LeafId == GetCortege(i).m_LeafId)
		&& (C.m_BracketLeafId == GetCortege(i).m_BracketLeafId)
		&& (C.m_FieldNo == GetCortege(i).m_FieldNo)
		&& (C.m_LevelId >= GetCortege(i).m_LevelId);
		i++);


	// Добавление  к списку значений
	_InsertCortege(i, C);

	return true;
};


bool CTempArticle::ArticleToText()
{
	int CurrLevelId = 0;
	BYTE CurrLeafId = ErrUChar;
	BYTE CurrBracketLeafId = ErrUChar;
	BYTE CurrFieldNo = ErrUChar;
	std::string LeftPart;
	char RightPart[2000];
	char Line[2000];
	std::string S;
	m_ArticleStr = "";


	for (size_t i = 0; i < GetCortegesSize(); i++)
	{
		TCortege C = GetCortege(i);
		RightPart[0] = 0;

		// смена текущей функции - значит, смена уровня
		if ((CurrLeafId != C.m_LeafId)
			|| (CurrBracketLeafId != C.m_BracketLeafId)
			|| (C.m_FieldNo != CurrFieldNo)
			)
			CurrLevelId = 0;

		if (C.m_LevelId > 0)
			if (C.m_LevelId > CurrLevelId)
			{
				char s[20];
				CurrLevelId = C.m_LevelId;
				if (C.m_LevelId == ErrUChar)
					strcpy(s, "*");
				else
					sprintf(s, "%i", C.m_LevelId);
				strcpy(RightPart, s);
				strcat(RightPart, " ");
			}
			else
				strcpy(RightPart, "  ");

		auto s = m_pRoss->WriteToString(C);
		strcat(RightPart, s.c_str());

		// пошло новое поле
		if ((CurrFieldNo != C.m_FieldNo)
			|| (CurrLeafId != C.m_LeafId)
			|| (CurrBracketLeafId != C.m_BracketLeafId)
			)
		{
			LeftPart = ConstructFldName(C.m_FieldNo, C.m_LeafId, C.m_BracketLeafId);

			sprintf(Line, C.GetFieldFormat(), LeftPart.c_str(), RightPart);
			CurrLevelId = C.m_LevelId;
			CurrLeafId = C.m_LeafId;
			CurrBracketLeafId = C.m_BracketLeafId;
			CurrFieldNo = C.m_FieldNo;
		}
		else
			// продолжается запись значения старого поля
			sprintf(Line, "%10s%s", " ", RightPart);

		m_ArticleStr += Line;
		m_ArticleStr += "\n";
	};
	return true;
};

void DeleteEmptyLines(std::string& s)
{
	for (int i = 0; i < s.length(); i++)
		while ((s[i] == '\n')
			&& (i < s.length() - 1)
			)
		{
			int k = i + 1;

			for (; k < s.length(); k++)
				if ((s[k] != ' ')
					&& (s[k] != '\t')
					&& (s[k] != '\r')
					)
					break;

			if ((k == s.length()) || (s[k] == '\n'))
				s.erase(i + 1, k + 1 - (i + 1));
			else
				break;
		};
};


std::string GetLine(std::string s, size_t LineNo)
{
	size_t  start = 0;
	for (size_t i = 0; i < LineNo; i++)
	{
		start = s.find('\n', start) + 1;
	};

	size_t end = s.find('\n', start);
	if (end == s.npos)
		end = s.length();

	return s.substr(start, end - start);
};


void CTempArticle::AddCortegeToVector(CTextField& F)
{
	BYTE LevelId = 0;

	bool IsEqual = false;

	for (size_t i = F.StartLine; i <= F.EndLine; i++)
	{
		std::string s = GetLine(m_ArticleStr, i);
		int  j = s.length() - 1;
		for (;
			(j >= 0) && isspace((unsigned char)s[j]);
			j--);
		j++;

		s.erase(s.begin() + j, s.end());


		if (i == F.StartLine)
		{
			int p = s.find('=');
			s.erase(0, p + 1);
			if ((unsigned char)s[0] == '=')
			{
				s.erase(0, 1);
				IsEqual = true;
			};


		};



		int k = s.find_first_not_of(" \t");
		if (k != s.npos)
			s.erase(0, k);
		else
			s = "";



		std::string FieldStr = ConstructFldName(F.FieldNo, F.LeafId, F.BracketLeafId);

		if ((m_pRoss->Fields[F.FieldNo].TypeRes == frFormula)
			&& !s.empty())
		{
			int j = atoi(s.c_str());
			if ((j > 0)
				|| (s[0] == '*')
				)
			{
				LevelId = (s[0] == '*') ? ErrUChar : j;
				int k = s.find(' ');
				if (k == std::string::npos)
				{
					s = "";
				}
				else
				{
					s.erase(0, k + 1);
					s.erase(0, s.find_first_not_of(" \t"));
				};
			};

			if (LevelId < 1)
			{
				auto m = Format("Error: No level number (field %s)", FieldStr.c_str());
				throw article_parse_error(m, i + 1);
			};

			if ((LevelId > 40) && (LevelId != ErrUChar))
			{
				auto m =  Format("Error: Level number is too large (field %s) ", FieldStr.c_str());
				throw article_parse_error(m, i + 1);
			};


		};

		TCortege C;

		C.m_FieldNo = F.FieldNo;
		C.m_LeafId = F.LeafId;
		C.m_BracketLeafId = F.BracketLeafId;
		C.m_LevelId = LevelId;
		if (!s.empty())
		{
			bool b;
			b = m_pRoss->ReadFromStr(s.c_str(), C);
			if (IsEqual)
				C.SetEqual();
			if (!b)
			{
				auto m = Format("\nCannot read line  (\"%s\")! ", s.c_str());
				throw article_parse_error(m, i + 1);
			}
		};;

		if ((C.m_LevelId == 0) && (m_pRoss->Fields[F.FieldNo].TypeRes == frFormula))
			C.m_LevelId = 1;

		_AddCortege(C);
		
	};
};


void CTempArticle::CheckCortegeVector()
{

	int ActantNo = 1;
	for (size_t i = 0; i < GetCortegesSize(); i++)
	{
		std::string FldName = ConstructFldName(GetCortege(i).m_FieldNo, GetCortege(i).m_LeafId, GetCortege(i).m_BracketLeafId);
		if (GetCortege(i).m_FieldNo == ErrUChar)
		{
			throw CExpc("Unknown field ");
		};
		
		for (BYTE k = 0; k < m_pRoss->GetSignat(GetCortege(i)).GetDomsWoDelims().size(); k++)
			if (GetCortege(i).is_null(k))
			{
				throw CExpc("no all values in field %s", FldName.c_str());
			}


		bool FoundPrevLevelId = !(GetCortege(i).m_LevelId > 1 && (GetCortege(i).m_LevelId != ErrUChar));
		for (size_t k = 0; k < GetCortegesSize(); k++)
			if ((i != k)
				&& (GetCortege(i).m_FieldNo == GetCortege(k).m_FieldNo)
				&& (GetCortege(i).m_LeafId == GetCortege(k).m_LeafId)
				&& (GetCortege(i).m_BracketLeafId == GetCortege(k).m_BracketLeafId)
				)
			{
				if (m_pRoss->Fields[GetCortege(i).m_FieldNo].TypeRes == frOne)
				{
					throw CExpc(Format("Field \"%s\" is repeated", FldName.c_str()));
				};

				if (!FoundPrevLevelId)
					if (GetCortege(k).m_LevelId == GetCortege(i).m_LevelId - 1)
						FoundPrevLevelId = true;
			};

		if (!FoundPrevLevelId)
		{
			throw CExpc("Bad level numbers in \"%s\"", FldName.c_str());
		};


		for (size_t k = 0; k < GetCortegesSize(); k++)
		{
			if ((i != k)
				&& (GetCortege(i).EqualCortege(GetCortege(k)))
				&& (m_pRoss->Fields[GetCortege(i).m_FieldNo].TypeRes == frOne)
				)
			{
				throw CExpc("Two equal items on one level in \"%s\"", FldName.c_str());
			};


		};


		if (FldName == "VAL")
		{
			char q[200] = "\0";

			const char* s = m_pRoss->GetDomItemStr(i, 1).c_str();
			strcat(q, s);
			s = m_pRoss->GetDomItemStr(i, 2).c_str();
			strcat(q, s);
			int l = 0;
			if (strlen(q) == 3)
				if ((isdigit((unsigned char)q[1]) && (ActantNo + '0' != q[1]))
					|| (isdigit((unsigned char)q[2]) && (ActantNo + '0' != q[2]))
					)
				{
					throw CExpc("Error in valency numbering (field \"%s\")", FldName.c_str());
				};

			ActantNo++;
		};

	}
}


inline bool ProcessBracketLeafId(std::string& FldName, BYTE& BracketLeafId)
{
	int i = FldName.length() - 2;

	for (; i > 0; i--)
		if (!isspace((BYTE)FldName[i]))
			break;

	if (i == 0) return false;

	if (isdigit((BYTE)FldName[i]))
		BracketLeafId = FldName[i] - '0';
	else
		if (FldName[i] == '*')
			BracketLeafId = ErrUChar;
		else return false;

	for (i--; i > 0; i--)
		if (!isspace((BYTE)FldName[i]))
			break;

	if (i == 0) return false;

	if (((BYTE)FldName[i]) != '(') return false;

	for (i--; i > 0; i--)
		if (!isspace((BYTE)FldName[i]))
			break;
	if (i == 0) return false;
	FldName.erase(i + 1);
	return true;
};





inline bool SplitFldName(std::string& FldName, BYTE& LeafId, BYTE& BracketLeafId)
{
	BracketLeafId = 0;

	if (FldName[FldName.length() - 1] == ')')
		ProcessBracketLeafId(FldName, BracketLeafId);

	LeafId = FldName[FldName.length() - 1] - '0';

	if (((LeafId > 9)
		|| (LeafId == 0))
		&& (FldName[FldName.length() - 1] != '*')
		)
		LeafId = 0;
	else
	{
		if (FldName[FldName.length() - 1] == '*')
			LeafId = ErrUChar;
		FldName[FldName.length() - 1] = 0;
	};

	return true;
};


void CTempArticle::ReadFromDictionary(uint16_t UnitNo, bool VisualOrder, bool ReadOnly)
{
	m_UnitNo = UnitNo;
	m_ReadOnly = ReadOnly;
	ClearCorteges();
	const CStructEntry& U = m_pRoss->GetUnits()[UnitNo];
	m_EntryStr = U.GetEntryStr();
	m_MeanNum = U.m_MeanNum;

	if (!m_ReadOnly)
		if (!m_pRoss->GetUnits()[UnitNo].HasEmptyArticle())
			for (int i = m_pRoss->GetUnits()[UnitNo].m_StartCortegeNo; i <= m_pRoss->GetUnits()[UnitNo].m_LastCortegeNo; i++)
				if (VisualOrder)
					PutCortegeOnTheRigthPosition(GetRossCortege(i));
				else
					_AddCortege(m_pRoss->GetCortege(i));

}


void CTempArticle::ReadFromUtf8String(const char* s)
{
	if (m_ReadOnly)
	{
		throw CExpc("Article is readonly");
	};

	m_ArticleStr = s;
	DeleteEmptyLines(m_ArticleStr);
	MarkUp();
	BuildCortegeList();
}


void  CTempArticle::MarkUp()
{
	m_Fields.clear();
	size_t LineNo = 0;
	int  old_eoln = -1;
	size_t Size = m_ArticleStr.length();
	if (m_ArticleStr.empty()) return;
	for (size_t _eoln = 0; _eoln <= Size; _eoln++)
		if ((_eoln == Size) || (m_ArticleStr[_eoln] == '\n'))
		{
			if ((_eoln == Size) && (old_eoln + 1 == _eoln)) continue;
			std::string Line, FldName, Delim;
			Line = m_ArticleStr.substr(old_eoln + 1, _eoln - old_eoln - 1);
			StringTokenizer tok(Line.c_str(), " \t");
			if (tok())
				FldName = tok.val();
			if (tok())
				Delim = tok.val();
			bool IsGoodField = (!Delim.empty() && (Delim[0] == '='));

			//  checking if  field name and "=" were written without space
			if (!IsGoodField
				&& !FldName.empty()
				&& (FldName[FldName.length() - 1] == '=')
				)
			{
				FldName.erase(FldName.length() - 1);
				IsGoodField = true;
			};

			if (IsGoodField)
			{

				BYTE LeafId, BracketLeafId;
				SplitFldName(FldName, LeafId, BracketLeafId);
				BYTE  FieldNo = m_pRoss->GetFieldNoByFieldStrInner(FldName.c_str());
				if (FieldNo == ErrUChar)
				{
					auto m = Format("Field \"%s\"  is not registered", FldName.c_str());
					m_Fields.clear();
					throw article_parse_error(m, LineNo + 1);
				}

				CTextField T;
				T.StartLine = LineNo;
				T.EndLine = LineNo;
				T.FieldNo = FieldNo;
				T.LeafId = LeafId;
				T.BracketLeafId = BracketLeafId;
				m_Fields.push_back(T);
			}
			else
				if (!m_Fields.empty())
					m_Fields[m_Fields.size() - 1].EndLine++;
				else
				{
					m_Fields.clear();
					auto m = Format("Field \"%s\"  is not registered", FldName.c_str());
					throw article_parse_error(m, LineNo + 1);

				};

			LineNo++;
			old_eoln = _eoln;
		};

}


void CTempArticle::BuildCortegeList()
{
	ClearCorteges();
	for (auto& f : m_Fields) {
		AddCortegeToVector(f);
	}
}


void  CTempArticle::WriteToDictionary()
{
	if (m_ReadOnly)
	{
		throw CExpc("Article is readonly");
	};

	CheckCortegeVector();

	std::vector<CStructEntry>& Units = m_pRoss->m_Units;

	if (!m_pRoss->m_Units[m_UnitNo].HasEmptyArticle())
		m_pRoss->DelCorteges(Units[m_UnitNo].m_StartCortegeNo, Units[m_UnitNo].m_LastCortegeNo + 1);

	Units[m_UnitNo].m_StartCortegeNo = m_pRoss->_GetCortegesSize();

	m_pRoss->ConcatOtherContainer(*this);


	Units[m_UnitNo].m_LastCortegeNo = m_pRoss->_GetCortegesSize() - 1;

	if (GetCortegesSize() == 0)
	{
		Units[m_UnitNo].MakeEmpty();
	};
}

bool CTempArticle::IsModified() const
{

	CTempArticle saved(m_pRoss);
	saved.m_pRoss = m_pRoss;

	int StartPos = m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo;
	int EndPos = m_pRoss->m_Units[m_UnitNo].m_LastCortegeNo;

	if (!m_pRoss->m_Units[m_UnitNo].HasEmptyArticle())
		for (size_t i = StartPos; i <= EndPos; i++)
		{
			saved._AddCortege(m_pRoss->GetCortege(i));
		};
	bool equal = IsPartOf(&saved, false) && saved.IsPartOf(this, false);
	return !equal;
}


std::string CTempArticle::GetArticleStrUtf8(bool check)
{
	if (!ArticleToText()) {
		if (check) {
			throw CExpc("cannot parse %zu", m_UnitNo);
		}
	}
	return m_ArticleStr;
}

void CTempArticle::SetUnitNo(uint16_t UnitNo) {
	m_UnitNo = UnitNo;
}



