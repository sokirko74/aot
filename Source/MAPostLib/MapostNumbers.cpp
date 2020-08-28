#include "MAPostMain.h"
#include "../common/rus_numerals.h"


bool CMAPost::is_russian_numeral(std::string& word) const {
	std::vector<CFormInfo> Paradigms;
	m_pRusLemmatizer->CreateParadigmCollection(false, word, false, false, Paradigms);
	if (Paradigms.empty())
		return false;
	for (auto& p : Paradigms)
	{
		std::string Form = p.GetWordForm(0);
		for (int i = NumeralToNumberCount; i >= 0; i--) {
			if (NumeralToNumber[i].m_Cardinal == Form || NumeralToNumber[i].m_Ordinal == Form)
			{
				return true;
			}
		}
	}
	return false;
}


void CMAPost::Cifrdef()
{

	// Ищем последовательность чисел и окончаний типа
	// 1960-го                           2 6 DSC
	//и лемматизируем этого слово как числительное 
	//	1960-го                            4 0 4 RLE -= 1848 яя -1 #0
	// А так же нулевые окончания
	CLineIter dollar = m_Words.end();
	for (CLineIter it = m_Words.begin(); it != m_Words.end(); it++)
	{
		CPostLemWord& W = *it;
		CLineIter next_it = it;
		CLineIter prev_it = it;
		next_it++;
		if (it != m_Words.begin()) prev_it--;
		//if (W.m_strWord.length() < 3) continue;
		int hyp = W.m_strWord.find("-");
		//if (hyp == std::string::npos) continue;
		if (W.IsInOborot()) continue;

		// Доллары
		if (!isdigit((BYTE)W.m_strWord[0]) && !(hyp > 0 && is_russian_alpha((BYTE)W.m_strWord.back())))
			if (dollar == prev_it)//$9,4 млрд
			{
				if (is_russian_numeral(W.m_strWord)) {
					if ((*it).HasDes(OSentEnd)) { (*it).DelDes(OSentEnd); (*prev_it).AddDes(OSentEnd); }
					if ((*it).HasDes(CS_Undef)) { (*it).DelDes(CS_Undef); (*prev_it).AddDes(CS_Undef); }
					iter_swap(prev_it, it);
					dollar++;
				}
			}
			else
				if (isdigit((BYTE)W.m_strWord.back()) && hyp > 0 && is_russian_alpha((BYTE)W.m_strWord[0]))
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
		std::string NumWordForm = hyp < 0 ? it->m_strWord : it->m_strWord.substr(0, hyp);
		std::string Flexia = hyp < 0 ? "" : it->m_strWord.substr(hyp + 1);

		if (Flexia == "" && next_it != m_Words.end() && isdigit((BYTE)next_it->m_strWord[0]) && next_it->m_strWord.length() == 3) // "в 1 300 световых годах" -> 1300
		{
			next_it->m_strWord = W.m_strWord + next_it->m_strWord;
			W.DeleteAllHomonyms();
			m_Words.erase(it, it);
			continue;
		}
		//  Идем с  конца ищем числительное, которое максимально совпадает с конца с числительным во входном тексте.
		int i = NumeralToNumberCount + (NumWordForm == "0" ? 0 : -1); //включая ноль
		std::string NumWordForm2 = NumWordForm;
		while (atoi(NumWordForm2.c_str()) >= 1000 && NumWordForm2.substr(NumWordForm2.length() - 3) == "000")
			NumWordForm2 = NumWordForm2.substr(0, NumWordForm2.length() - 3);
		for (; i >= 0; i--)
		{
			std::string NumValue;
			if (W.HasDes(ORoman))
				NumValue = NumeralToNumber[i].m_RomanNumber;
			else
				NumValue = IntToStr(NumeralToNumber[i].m_Number);

			if (NumValue.length() > 0)
				if (NumWordForm2.length() >= NumValue.length())
					if (NumValue == NumWordForm2.substr(NumWordForm2.length() - NumValue.length()))
						break;
		};
		if (i < 0)  continue;

		EngRusMakeLower(Flexia);
		std::string AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Cardinal, Flexia, NumeralToNumber[i].m_bNoun);
		if (AnCodes.empty() && Flexia != "")
			AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Ordinal, Flexia, NumeralToNumber[i].m_bNoun);
		if (NumeralToNumber[i].m_Cardinal == _R("ОДИН")) {
			AnCodes = _R("эжэзэиэйэкэлэмэнэоэпэрэсэтэуэфэхэцэч"); //все грамкоды с родом
		}
		std::string AnCodes0 = AnCodes; //числ
		if (NumWordForm != "0") {
			AnCodes = GetSimilarNumAncode(NumeralToNumber[i].m_Ordinal, Flexia, NumeralToNumber[i].m_bNoun);
		}
		if (Flexia == "")
			AnCodes = m_pRusGramTab->FilterGramCodes(rAllNumbers, AnCodes, _QM(rSingular));
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
				if ((*it).HasDes(CS_Undef)) { (*it).DelDes(CS_Undef); (*prev_it).AddDes(CS_Undef); }
				iter_swap(prev_it, it);
				dollar++;
			}
		}

	};

};