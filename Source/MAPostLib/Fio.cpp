// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"
#include "../common/PlmLine.h"



typedef CSmallVector<CHomonym*, 5> SmallHomonymsVec;
typedef CSmallVector<CLineIter, 5> SmallWordsVec;


enum FioItemTypeEnum { fiName, fiSurname, fiMiddle, fiAbbr, fiStop, fiRoman, fiProbName, fiString, fiOrdinal };
struct CFIOItem {
	FioItemTypeEnum m_fiType;
	std::string			m_ItemStr;
};


struct CFIOFormat
{
	std::string m_FormatStr;
	bool   m_GleicheCase;
	std::vector<CFIOItem> m_FioItems;
	CFIOFormat() {};

	CFIOFormat(std::string  FormatStr, bool GleicheCase)
	{
		m_FormatStr = FormatStr;
		m_GleicheCase = GleicheCase;
		StringTokenizer tok(FormatStr.c_str(), " ");
		while (tok())
		{
			std::string s = tok.val();
			FioItemTypeEnum t;
			if (s == _R("ИМЯ"))
				t = fiName;
			else
				if (s == _R("ФАМИЛИЯ"))
					t = fiSurname;
				else
					if (s == _R("ОТЧЕСТВО"))
						t = fiMiddle;
					else
						if (s == _R("ИНИЦИАЛ"))
							t = fiAbbr;
						else
							if (s == _R("ТОЧКА"))
								t = fiStop;
							else
								if (s == _R("РИМСК_ЦК"))
									t = fiRoman;
								else
									if (s == _R("ИМ?"))
										t = fiProbName;
									else
										if (s == _R("ПОРЯД_ЧИСЛ"))
											t = fiOrdinal;
										else
										{
											t = fiString;
										};
			CFIOItem I;
			I.m_fiType = t;
			I.m_ItemStr = s;
			m_FioItems.push_back(I);
		};
	};
};


static void GetCommonVariants(const std::vector<SmallHomonymsVec>& Parents,
	SmallHomonymsVec& V,
	std::vector<SmallHomonymsVec>& Variants,
	long       Position)
{
	if (Variants.size() > 1000) return;
	if (Position < V.size())
		for (long i = 0; i < Parents[Position].size(); i++)
		{
			V.m_VectorItems[Position] = Parents[Position].m_VectorItems[i];
			GetCommonVariants(Parents, V, Variants, Position + 1);
		}
	else
		Variants.push_back(V);

};


static bool IsPartFio(const CMAPost& C, const CFIOItem& I, const CPostLemWord& Word, const CHomonym* pH)
{
	if (Word.HasDes(OFAM1) || Word.HasDes(OFAM2))    return false;
	switch (I.m_fiType)
	{
		case fiName:  return pH->HasGrammem(rName);
		case fiSurname:  return pH->HasGrammem(rSurName) || (Word.m_bFirstUpperAlpha
			&& (   endswith(pH->m_strLemma, _R("ИЙ"))
				|| endswith(pH->m_strLemma, _R("АЯ"))
				|| endswith(pH->m_strLemma, _R("ОВ"))
				|| endswith(pH->m_strLemma, _R("ОВА"))
				));
		case fiMiddle:  return pH->HasGrammem(rPatronymic);

		case fiAbbr:  return      Word.m_bFirstUpperAlpha
			&& (Word.m_strWord.size() == 1)
			&& Word.HasDes(ORLE);

		case fiStop:  return Word.m_strWord == ".";
		case fiRoman:  return Word.HasDes(ORoman);
		case fiOrdinal:  return pH->HasPos(NUMERAL_P)
			&& (unsigned char)Word.m_strWord[0] < 224 //отбрасываем "Петр первый" вместо "Петр Первый"
			&& !isdigit((BYTE)Word.m_strWord[0]);
		case fiProbName:  return Word.HasDes(ONam);
		default:  return pH->m_strLemma == I.m_ItemStr;
	}


};


bool IsPartOfNonSingleOborot(const CHomonym* pH)
{
	return pH->m_bOborot1 != pH->m_bOborot2;
}

bool CMAPost::SetFioFormat(const CFIOFormat* Format, CLineIter it)
{
	std::vector<SmallHomonymsVec> Hypots;
	SmallWordsVec FioWords;

	Hypots.resize(Format->m_FioItems.size());

	int CountOfVariants = 1;
	for (long ItemNo = 0; ItemNo < Format->m_FioItems.size() && it != m_Words.end(); ItemNo++, it = NextNotSpace(it))
	{
		FioWords.Add(it);
		CPostLemWord& W = *it;

		for (int HomNo = 0; HomNo < W.GetHomonymsCount(); HomNo++)
		{
			CHomonym* pH = W.GetHomonym(HomNo);
			// иначе "Т.Е. ОТКАЗАТЬСЯ" будет ФИО
			if (IsPartOfNonSingleOborot(pH)) return false;

			if (IsPartFio(*this, Format->m_FioItems[ItemNo], W, pH))
				Hypots[ItemNo].Add(pH);
		};
		if (Hypots[ItemNo].empty()) return false;
		CountOfVariants *= Hypots[ItemNo].size();
	};

	if (FioWords.size() != Format->m_FioItems.size()) return false; // не достроилось

	SmallHomonymsVec V; // текущий вариант 
	std::vector<SmallHomonymsVec> Variants;
	Variants.reserve(CountOfVariants);
	V.m_ItemsCount = Hypots.size();
	GetCommonVariants(Hypots, V, Variants, 0);

	if (Format->m_GleicheCase)
		for (long VarNo = 0; VarNo < Variants.size(); VarNo++)
		{
			uint64_t Grammems = rAllCases | rAllNumbers;
			for (long i = 0; i < Variants[VarNo].size(); i++)
			{
				Grammems &= Variants[VarNo].m_VectorItems[i]->m_iGrammems;
			};
			if ((Grammems & rAllCases) == 0 || (Grammems & rAllNumbers) == 0)
			{
				Variants.erase(Variants.begin() + VarNo);
				VarNo--;
			};

		};

	if (Variants.empty()) return false;

	for (size_t i = 0; i < FioWords.size(); i++)
	{
		CPostLemWord& W = *FioWords[i];
		CHomonym* pH = Variants[0].m_VectorItems[i];
		W.SetHomonymsDel(true);
		pH->m_bDelete = false;
		pH->DeleteOborotMarks(); // удаляем однословные оборотыб (многословных там быть не может)
		W.DeleteMarkedHomonymsBeforeClauses();
		if (W.HasDes(OSentEnd))
		{
			/*
			  если ФИО содержало конец предложения, а после ФИО нет ни одной пометы
			  конца предложения, тогда надо поставить SENT_END на последнюю строчку ФИО.
			*/
			W.DelDes(OSentEnd);
			if (W.m_strWord != ".")
				FioWords.back()->AddDes(OSentEnd);
		}
	}

	// ставим графем. пометы
	FioWords[0]->AddDes(OFAM1);
	FioWords.back()->AddDes(OFAM2);

	return true;

};


/*
Петр Владимирович Иванов,
Иванов Петр Владимирович,
Петр Иванов,
Иванов Петр,
Петр Владимирович,
Райнер Мария Рильке,
Иоанн Павел II
М.Горбачев
*/

void CMAPost::Rule_Fio()
{
	std::vector<CFIOFormat> FioFormats;
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ОТЧЕСТВО ФАМИЛИЯ"), true));
	FioFormats.push_back(CFIOFormat(_R("ФАМИЛИЯ ИМЯ ОТЧЕСТВО"), true));
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ФАМИЛИЯ"), true));
	FioFormats.push_back(CFIOFormat(_R("ФАМИЛИЯ ИМЯ"), true));
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ОТЧЕСТВО"), true));
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ИМЯ ИМ?"), false)); // Эрих Мария Ремарк
	FioFormats.push_back(CFIOFormat(_R("ИНИЦИАЛ ТОЧКА ИНИЦИАЛ ТОЧКА ФАМИЛИЯ"), false)); // М.Горбачев
	FioFormats.push_back(CFIOFormat(_R("ИНИЦИАЛ ТОЧКА ФАМИЛИЯ"), false)); // М.Горбачев
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ИМЯ РИМСК_ЦК"), false)); // Иоанн Павел II
	FioFormats.push_back(CFIOFormat(_R("ИМЯ РИМСК_ЦК"), false)); // Александр II
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ПОРЯД_ЧИСЛ"), true)); // Александр Второй
	FioFormats.push_back(CFIOFormat(_R("ИМЯ ИМЯ ПОРЯД_ЧИСЛ"), false)); // Иоанн Павел Второй
	FioFormats.push_back(CFIOFormat(_R("ДОН ЖУАН"), false)); // Дон Жуан


	for (CLineIter it = m_Words.begin(); it != m_Words.end(); it++)
	{
		for (long FormatNo = 0; FormatNo < FioFormats.size(); FormatNo++)
			if (SetFioFormat(&FioFormats[FormatNo], it))
			{
				break;
			}
	};

};

