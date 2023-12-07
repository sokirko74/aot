// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"



typedef std::vector<CHomonym*> HomonymVector;
typedef std::vector<CLineIter> InputWordVector;


enum FioItemTypeEnum { fiName, fiSurname, fiMiddle, fiAbbr, fiStop, fiRoman, fiProbName, fiString, fiOrdinal };
struct CFIOItem {
	FioItemTypeEnum m_fiType;
	std::string			m_ItemStr;
	CFIOItem(FioItemTypeEnum fiType) : m_fiType(fiType) {};
	CFIOItem(FioItemTypeEnum fiType, std::string itemStr) : m_fiType(fiType), m_ItemStr() {};
};


struct CFIOFormat
{
	bool   m_GleicheCase;
	std::vector<CFIOItem> m_FioItems;
	CFIOFormat() {};

	CFIOFormat(std::vector< CFIOItem> fio_items, bool GleicheCase)
	{
		m_FioItems = fio_items;
		m_GleicheCase = GleicheCase;
	};
};


static bool IsPartFio(const CMAPost& C, const CFIOItem& I, const CLemWord& Word, const CHomonym* pH)
{
	if (Word.HasDes(OFAM1) || Word.HasDes(OFAM2))    return false;
	switch (I.m_fiType)
	{
		case fiName:  return pH->HasGrammem(rName);
		case fiSurname:  return pH->HasGrammem(rSurName) || (Word.m_bFirstUpperAlpha
			&& (   endswith(pH->GetLemma(), "ИЙ")
				|| endswith(pH->GetLemma(), "АЯ")
				|| endswith(pH->GetLemma(), "ОВ")
				|| endswith(pH->GetLemma(), "ОВА")
				));
		case fiMiddle:  return pH->HasGrammem(rPatronymic);

		case fiAbbr:  return       Word.m_bFirstUpperAlpha
								&& (Word.m_LettersCount == 1)
								&& Word.HasDes(ORLE);

		case fiStop:  return Word.m_strWord == ".";
		case fiRoman:  return Word.HasDes(ORoman);
		case fiOrdinal:  return pH->HasPos(NUMERAL_P)
			&& Word.m_bFirstUpperAlpha //отбрасываем "Петр первый" вместо "Петр Первый"
			&& !isdigit((BYTE)Word.m_strWord[0]);
		case fiProbName:  return Word.m_Register != LowLow;
		default:  return pH->GetLemma() == I.m_ItemStr;
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
		CLemWord& W = *it;
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
		CLemWord& W = *FioWords[i];
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
    LOGV << "create OFAM1 and OFAM2 for " << FioWords[0]->m_strWord << " ... "  << FioWords.back()->m_strWord;

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
	std::vector<CFIOFormat> FioFormats = {
		{ { fiName, fiMiddle, fiSurname }, true },
		{ { fiName, fiMiddle, fiSurname }, true},
		{ { fiSurname, fiName, fiMiddle }, true},
		{ { fiName, fiSurname }, true},
		{ { fiSurname, fiName }, true},
		{ { fiName, fiMiddle }, true},
		{ { fiName, fiName, fiProbName }, false}, // Эрих Мария Ремарк
		{ { fiAbbr, fiStop, fiAbbr, fiStop, fiSurname }, false}, // М.Горбачев
		{ { fiAbbr, fiStop, fiSurname }, false}, // М.Горбачев
		{ { fiName, fiName, fiRoman }, false}, // Иоанн Павел II
		{ { fiName, fiRoman }, false}, // Александр II
		{ { fiName, fiOrdinal }, true}, // Александр Второй
		{ { fiName, fiName, fiOrdinal}, false}, // Иоанн Павел Второй
		{ { {fiString, "ДОН"}, { fiString, "ЖУАН"} }, false } // Дон Жуан
	};
	

	for (CLineIter it = m_Words.begin(); it != m_Words.end(); it++)
	{
		for (long FormatNo = 0; FormatNo < FioFormats.size(); FormatNo++)
			if (SetFioFormat(&FioFormats[FormatNo], it))
			{
				break;
			}
	};
	
};

