// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"
#include "common/PlmLine.h"



typedef std::vector<CHomonym*> HomonymVector;
typedef std::vector<CLineIter> InputWordVector;


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

bool get_next_variant(const std::vector<HomonymVector>& base, std::vector<size_t>& cur_variant) {

	if (cur_variant.empty()) {
		cur_variant.resize(base.size(), 0);
		return true;
	}
	assert(base.size() == cur_variant.size());

	for (size_t i = 0; i < cur_variant.size(); ++i) {
		if (cur_variant[i] + 1 < base[i].size()) {
			++cur_variant[i];
			return true;
		}
		cur_variant[i] = 0;
	}
	return false;
}

bool CMAPost::SetFioFormat(const CFIOFormat* Format, CLineIter it)
{
	std::vector<HomonymVector> Hypots;
	InputWordVector FioWords;
	size_t variants_count = 1;
	for (auto& fio_item: Format->m_FioItems)
	{
		CPostLemWord& W = *it;
		HomonymVector homs;
		for (int HomNo = 0; HomNo < W.GetHomonymsCount(); HomNo++)
		{
			CHomonym* pH = W.GetHomonym(HomNo);
			// иначе "Т.Е. ОТКАЗАТЬСЯ" будет ФИО
			if (IsPartOfNonSingleOborot(pH)) return false;

			if (IsPartFio(*this, fio_item, W, pH)) {
				homs.push_back(pH);
			}
		};
		if (homs.empty()) {
			return false;
		}
		variants_count *= homs.size();
		Hypots.push_back(homs);
		FioWords.push_back(it);

		it = NextNotSpace(it);
		if (it == m_Words.end()) {
			break;
		}
	};

	if (FioWords.size() != Format->m_FioItems.size()) return false; // не достроилось
	assert(FioWords.size() == Hypots.size());

	std::vector<size_t> best_var;
	if (!Format->m_GleicheCase) {
		best_var.resize(Hypots.size(), 0);
	}
	else {
	std::vector<size_t> cur_variant;
		size_t processed_vars_count = 0;
		int max_variant_weight = -1;
		while (get_next_variant(Hypots, cur_variant))
		{
			uint64_t Grammems = rAllCases | rAllNumbers;
			for (size_t i = 0; i < cur_variant.size(); ++i)
			{
				Grammems &= Hypots[i][cur_variant[i]]->m_iGrammems;
			};
			int variant_weight = 0;
			if ((Grammems & rAllCases) && (Grammems & rAllNumbers))
			{
				variant_weight = 1;
			};
			if ((Grammems & rAllCases) && (Grammems & _QM(rSingular)))
			{
				variant_weight = 2;
			};
			if (variant_weight > max_variant_weight) {
				max_variant_weight = variant_weight;
				best_var = cur_variant;
			}
 			processed_vars_count++;
			if (processed_vars_count > 1000) {
				break;;
			}
		};
	}

	if (best_var.empty()) return false;
	
	for (size_t i = 0; i < FioWords.size(); i++)
	{
		CPostLemWord& W = *FioWords[i];
		CHomonym* pH = Hypots[i][best_var[i]];
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

