#include "MAPostMain.h"
#include "morph_dict/LemmatizerBaseLib/rus_numerals.h"
#include "morph_dict/AgramtabLib/agramtab_.h"


bool CMAPost::is_russian_numeral(std::string& word) const {
	std::vector<CFormInfo> Paradigms;
	m_pRusLemmatizer->CreateParadigmCollection(false, word, false, false, Paradigms);
	for (auto& p : Paradigms)
	{
		auto form = p.GetWordForm(0);
		if (RussianNumerals.CheckIsNumeral(form)) {
			return true;
		}
	}
	return false;
}

std::string CMAPost::GetSimilarNumAncode(const std::string& Lemma, const std::string& Flexia, bool IsNoun)
{
	if (Lemma.length() == 0) return "";
	std::vector<CFormInfo> Paradigms;
	std::string h = Lemma;
	m_pRusLemmatizer->CreateParadigmCollection(false, h, false, false, Paradigms);
	if (Paradigms.size() == 0) return ""; // например, нету в Ё-словаре слова ЧЕТВЕРТЫЙ
	// ищем числительное
	long k = 0;
	for (; k < Paradigms.size(); k++)
	{
		std::string AnCode = Paradigms[k].GetAncode(0);
		BYTE POS = m_pRusGramTab->GetPartOfSpeech(AnCode.c_str());
		if (IsNoun)
		{
			if (POS == NOUN)
				break;
		}
		if ((POS == NUMERAL) || (POS == NUMERAL_P) || Lemma == _R("НУЛЕВОЙ"))
			break;
	};
	assert(k < Paradigms.size());
	const CFormInfo& P = Paradigms[k];

	// ищем максимальное совпадение с конца 
	std::string AnCodes;
	for (k = 0; k < P.GetCount(); k++)
	{
		std::string Form = P.GetWordForm(k);
		EngRusMakeLower(Form);
		if (IsNoun && Form != h && std::string(_R("ао,ап,ат,ау,ац,ач,аъ")).find(P.GetAncode(k)) != std::string::npos) // 1000 - не аббр, "свыше 1000 человек"
			continue;
		if (Form.length() > Flexia.length())
			if (Flexia == "" || Flexia == Form.substr(Form.length() - Flexia.length()))
				AnCodes += P.GetAncode(k);
	};

	return m_pRusGramTab->UniqueGramCodes(AnCodes);


};


void CMAPost::Cifrdef()
{

	// Ищем последовательность чисел и окончаний типа
	// 1960-го                           2 6 DSC
	//и лемматизируем этого слово как ЧИСЛ и ЧИСЛ_П
	CLineIter dollar = m_Words.end();
	for (CLineIter it = m_Words.begin(); it != m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
		CLineIter next_it = it;
		CLineIter prev_it = it;
		next_it++;
		if (it != m_Words.begin()) prev_it--;

		size_t hyp = W.m_strWord.find("-");

		bool foundHyp = (hyp != std::string::npos) && (hyp > 0);
		if (W.IsInOborot()) continue;

		// Доллары
		if (!isdigit((BYTE)W.m_strWord[0]) && !(foundHyp && is_russian_alpha((BYTE)W.m_strWord.back())))
			if (dollar == prev_it)//$9,4 млрд
			{
				if (is_russian_numeral(W.m_strWord)) {
					if ((*it).HasDes(OSentEnd)) { 
						(*it).DelDes(OSentEnd); 
						(*prev_it).AddDes(OSentEnd); 
					}
					iter_swap(prev_it, it);
					dollar++;
				}
			}
			else
				if (isdigit((BYTE)W.m_strWord.back()) && foundHyp && is_russian_alpha((BYTE)W.m_strWord[0]))
				{
					W.DelDes(ONumChar);
					W.AddDes(ORLE);
					W.DeleteAllHomonyms();
					CHomonym* pNew = W.AddNewHomonym();
					std::vector<CFormInfo> Paradigms;
					std::string TmpStr = W.m_strWord.substr(0, hyp);
					m_pRusLemmatizer->CreateParadigmCollection(false, TmpStr, false, false, Paradigms);
					if (Paradigms.size() > 0) // плутония-238
					{
						pNew->m_lPradigmID = Paradigms[0].GetParadigmId();
						pNew->m_CommonGramCode = Paradigms[0].GetCommonAncode();
						pNew->SetGramCodes(Paradigms[0].GetSrcAncode());
						pNew->m_LemSign = '+';
						W.m_bWord = true;
						W.m_bPredicted = false;
					}
					else
					{
						pNew->SetGramCodes(m_DURNOVOGramCode);
						pNew->SetMorphUnknown();
					}
					pNew->m_strLemma = W.m_strUpperWord;
					pNew->InitAncodePattern();
				}
				else
					continue;
		// первая часть - цифры, второая - русская, если есть окончание
		std::string NumWordForm = !foundHyp ? it->m_strWord : it->m_strWord.substr(0, hyp);
		std::string Flexia = !foundHyp ? "" : it->m_strWord.substr(hyp + 1);

		if (Flexia == "" && next_it != m_Words.end() && isdigit((BYTE)next_it->m_strWord[0]) && next_it->m_strWord.length() == 3) // "в 1 300 световых годах" -> 1300
		{
			next_it->m_strWord = W.m_strWord + next_it->m_strWord;
			W.DeleteAllHomonyms();
			m_Words.erase(it, it);
			continue;
		}
		//  Идем с  конца ищем числительное, которое максимально совпадает с конца с числительным во входном тексте.
		std::string NumWordForm2 = NumWordForm;
		while (atoi(NumWordForm2.c_str()) >= 1000 && NumWordForm2.substr(NumWordForm2.length() - 3) == "000")
			NumWordForm2 = NumWordForm2.substr(0, NumWordForm2.length() - 3);

		const CNumeralToNumber* numeral = nullptr;
		for (auto& n: RussianNumerals.GetAllNumeralReverse())
		{
			if (n.m_Number == 0 && NumWordForm != "0") {
				continue;
			}
			std::string NumValue;
			if (W.HasDes(ORoman))
				NumValue = n.m_RomanNumber;
			else
				NumValue = DoubleToStr(n.m_Number);

			if (NumValue.length() > 0 && endswith(NumWordForm2, NumValue))
			{
				numeral = &n;
				break;
			}
		};
		if (numeral == nullptr)  continue;

		EngRusMakeLower(Flexia);
		std::string AnCodes = GetSimilarNumAncode(numeral->m_Cardinal, Flexia, numeral->m_bNoun);
		if (AnCodes.empty() && Flexia != "")
			AnCodes = GetSimilarNumAncode(numeral->m_Ordinal, Flexia, numeral->m_bNoun);
		if (numeral->m_Cardinal == _R("ОДИН")) {
			AnCodes = _R("эжэзэиэйэкэлэмэнэоэпэрэсэтэуэфэхэцэч"); //все грамкоды с родом
		}
		std::string AnCodes0 = AnCodes; //числ
		if (NumWordForm != "0") {
			AnCodes = GetSimilarNumAncode(numeral->m_Ordinal, Flexia, numeral->m_bNoun);
		}
		if (Flexia == "") {
			// 1. удаляем грамкоды множес. числа
			// можно просто не удалять, тогда будет очень много вариантов на синтаксисе (чистая оптимизация)
			//2. добавил условие для примера  "в 1960 годы", где порядковое существительное 1960 стоит в отчетливом множественном числе
			if (W.m_strWord.length() != 4) { // "1960"
				AnCodes = m_pRusGramTab->FilterGramCodes(rAllNumbers, AnCodes, _QM(rSingular));
			}
		}
		if (FindFloatingPoint(NumWordForm.c_str()) != -1 || AnCodes0 == AnCodes)
			AnCodes = "";
		if (AnCodes.empty() && AnCodes0.empty())
		{
			// "20-летний"
			if (W.LemmatizeForm(Flexia, m_pRusLemmatizer))
			{
				W.DelDes(ONumChar);
				W.AddDes(ORLE);
				W.SetWordStr(NumWordForm + "#" + Flexia, morphRussian);
			}
		}
		else
		{

			W.AddDes(ORLE);
			W.AddDes(OLw);
			W.DeleteAllHomonyms();

			if (!(AnCodes0.empty() || (next_it != m_Words.end() && (next_it->m_strUpperWord == _R("ГГ"))))) // ЧИСЛ
			{
				CHomonym* pH = W.AddNewHomonym();
				pH->SetMorphUnknown();
				pH->SetGramCodes(AnCodes0);
				pH->m_strLemma = NumWordForm;
				pH->InitAncodePattern();
			}
			CLineIter spec_it = it;
			if (next_it != m_Words.end() && (next_it->m_strWord == "%" || next_it->m_strWord == "$"))  //доллары, проценты
				spec_it = next_it;
			else if (it != m_Words.begin() && (prev_it->m_strWord == "$" || prev_it->m_strWord == "№"))
				spec_it = prev_it;
			if (it != spec_it)
			{
				CPostLemWord& W2 = *spec_it;
				std::vector<CFormInfo> Paradigms;
				W2.DeleteOborotMarks();
				W2.AddDes(ORLE);
				W2.DelDes(OPun);
				W2.DeleteAllHomonyms();
				CHomonym* pH = W2.AddNewHomonym();
				pH->SetMorphUnknown();
				pH->m_CommonGramCode = _R("Фа");
				pH->SetGramCodes(_R("ао"));
				if (W2.m_strWord == "%")
				{
					W2.m_strUpperWord = W2.m_strWord = _R("ПРОЦ");
					pH->m_strLemma = _R("ПРОЦЕНТ");
				}
				else  if (W2.m_strWord == "$")
				{
					if (spec_it == prev_it) //$12
						dollar = prev_it;
					W2.m_strUpperWord = W2.m_strWord = _R("ДОЛЛ");
					pH->m_strLemma = _R("ДОЛЛАР");
				}
				else  if (W2.m_strWord == "№")
				{
					W2.m_strUpperWord = W2.m_strWord = "№";
					pH->m_strLemma = _R("НОМЕР");
					//pH->m_GramCodes = "";
				}
				m_pRusLemmatizer->CreateParadigmCollection(true, pH->m_strLemma, true, false, Paradigms);
				pH->m_lPradigmID = Paradigms[0].GetParadigmId();
				W2.m_bWord = true;
				W2.m_bPredicted = false;
				pH->m_LemSign = '+';
				pH->InitAncodePattern();
			}
			else
				if (!((next_it != m_Words.end() && (next_it->m_strUpperWord == _R("ЛЕТ")))	// "в течение 2 лет"
					//|| (it !=  m_Words.begin() && prev_it->HasGrammem(rComparative) && prev_it->HasPos(NUMERAL)) // "совершила более 40 тяжких преступлений"
					))
					if (!AnCodes.empty())  //ЧИСЛ-П
					{
						CHomonym* pH = W.AddNewHomonym();
						pH->SetMorphUnknown();
						pH->SetGramCodes(AnCodes);
						pH->m_strLemma = NumWordForm;
						pH->InitAncodePattern();
					}
			if (dollar == prev_it)
			{
				if ((*it).HasDes(OSentEnd)) { (*it).DelDes(OSentEnd); (*prev_it).AddDes(OSentEnd); }
				iter_swap(prev_it, it);
				dollar++;
			}
		}

	};

};

